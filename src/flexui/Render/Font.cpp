#include "flexui/Render/Font.hpp"

#include <cassert>

// FreeType
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ADVANCES_H

namespace flexui {

    FT_Library Font::s_FT_Library = nullptr;

    Font* Font::LoadFromMemory(const unsigned char* data, const size_t size)
    {
		if (FT_Init_FreeType(&s_FT_Library) != 0) { // TODO: this is not very multithread friendly
			assert(false && "Couldn't initialize FreeType library");
            return nullptr;
		} // TODO: close the library...

        FT_Error error;
        FT_Face face;
        
        if ((error = FT_New_Memory_Face(Font::s_FT_Library, (FT_Byte*)data, (FT_Long)size, 0, &face)) == FT_Err_Ok) {
            // select encoding
            if ((error = FT_Select_Charmap(face, FT_ENCODING_UNICODE)) == FT_Err_Ok) {
                return new Font(face);
            }
        }

        // something went wrong
        return nullptr;
    }

    Font::Font(FT_Face face) :
        m_Face(face),
		m_CurrentSize(0)
    {
    }

    Font::~Font()
    {
        FT_Done_Face(m_Face);
    }

	bool Font::getGlyphMetrics(GlyphCodepoint codepoint, const TextStyle& style, GlyphMetrics& metrics)
	{
		if (!setSize(style.size))
			return false;

		GlyphHash idx = ((GlyphHash)(style.size) << 32) | codepoint; // TODO: add settings to index
		auto metrics_it = m_MetricsCache.find(idx);
		if (metrics_it != m_MetricsCache.end()) {
			metrics = (*metrics_it).second;
			return metrics.valid;
		}

		FT_Error error;
		if ((error = FT_Load_Char(m_Face, codepoint, FT_LOAD_NO_BITMAP)) != FT_Err_Ok) {
			// std::cerr << "Could not load glyph " << codepoint << "! " << FT_Error_String(error) << std::endl;

			// insert invalid metrics so we don't call FT_Load_Char again
			metrics.valid = false;
			m_MetricsCache.insert(std::make_pair(idx, metrics));

			return false;
		}

		metrics = {};
		metrics.width      = (m_Face->glyph->metrics.width        >> 6); // (>> 6) = (/ 64)
		metrics.height     = (m_Face->glyph->metrics.height       >> 6);
		metrics.H_advance  = (m_Face->glyph->metrics.horiAdvance  >> 6);
		metrics.H_bearingX = (m_Face->glyph->metrics.horiBearingX >> 6);
		metrics.H_bearingY = (m_Face->glyph->metrics.horiBearingY >> 6);
		metrics.V_advance  = (m_Face->glyph->metrics.vertAdvance  >> 6);
		metrics.V_bearingX = (m_Face->glyph->metrics.vertBearingX >> 6);
		metrics.V_bearingY = (m_Face->glyph->metrics.vertBearingY >> 6);
		metrics.valid = true;

		m_MetricsCache.insert(std::make_pair(idx, metrics));

		return true;
	}

	bool Font::renderGlyph(GlyphCodepoint codepoint, const TextStyle& style, unsigned char*& data, unsigned int& width, unsigned int& height)
	{
		if (!setSize(style.size))
			return false;

		FT_Int32 load_flags = FT_LOAD_RENDER;
		if (FT_HAS_COLOR(m_Face)) // render colored glyphs like emojis
			load_flags |= FT_LOAD_COLOR;

		FT_Error error;
		if ((error = FT_Load_Char(m_Face, codepoint, load_flags)) != FT_Err_Ok) {
			// std::cerr << "Could not load glyph " << codepoint << "! " << FT_Error_String(error) << std::endl;
			return false;
		}

		if (m_Face->glyph->bitmap.buffer == nullptr)
			return false;

		width = m_Face->glyph->bitmap.width;
		height = m_Face->glyph->bitmap.rows;
		data = (unsigned char*)malloc(width * height * 4);
		if (!data) return false; // malloc failed

		int pixels = width * height;
		auto source = m_Face->glyph->bitmap.buffer;

		switch (m_Face->glyph->bitmap.pixel_mode) {
		case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY:
		{
			for (int i = 0; i < pixels; i++) {
				data[i * 4 + 0] = source[i];
				data[i * 4 + 1] = source[i];
				data[i * 4 + 2] = source[i];
				data[i * 4 + 3] = 255;
			}
			return true;
		}
		case FT_Pixel_Mode::FT_PIXEL_MODE_BGRA:
		{
			for (int i = 0; i < pixels; i++) {
				data[i * 4 + 0] = source[i * 4 + 2];
				data[i * 4 + 1] = source[i * 4 + 1];
				data[i * 4 + 2] = source[i * 4 + 0];
				data[i * 4 + 3] = source[i * 4 + 3];
			}
			return true;
		}
		}

		return false;
	}

    TextLayout Font::generateTextLayout(const std::string& text, const TextStyle& textStyle, const TextLayoutSettings& layoutSettings, bool skip_glyphs)
    {
		TextLayout layout;
		layout.style = textStyle;
		layout.width = 0;
		layout.height = 0;
		if (!skip_glyphs)
			layout.glyphs.reserve(text.size());

		int penX = 0, penY = textStyle.size;
		GlyphMetrics metrics;

		int last_whitespace_i = 0;
		bool last_whitespace_was_wrap = false;

		for (int i = 0; i < (int)text.size(); i++) {
			GlyphCodepoint code = text[i]; // TODO: proper UTF-8

			if (!getGlyphMetrics(code, textStyle, metrics))
				continue;

			float advance = metrics.H_advance;
			bool should_wrap = false;

			if (layoutSettings.wordWrap) {
				// track white spaces
				if (code == ' ' || code == '\t') {
					last_whitespace_i = i;
					last_whitespace_was_wrap = false;
				}
				else {
					if (penX + advance > layoutSettings.wordWrapWidth) { // should wrap?
						if (!last_whitespace_was_wrap) { // avoid infinite loop
							i = last_whitespace_i;
							last_whitespace_was_wrap = true;
							should_wrap = true;

							// remove all glyphs after the last space
							while (layout.glyphs.size() > 0 && layout.glyphs.back().index >= last_whitespace_i)
								layout.glyphs.pop_back();
						}
					}
				}
			}

			if (code == '\n' || code == '\r' || should_wrap) {
				penX = 0;
				penY += textStyle.size;
				continue;
			}

			if (!skip_glyphs && metrics.width > 0 && metrics.height > 0) {
				TextLayout::GlyphInstance instance;

				instance.codepoint = code;
				instance.index = i;
				instance.x = penX + metrics.H_bearingX;
				instance.y = penY - metrics.H_bearingY;
				instance.w = metrics.width;
				instance.h = metrics.height;

				layout.width = std::max(layout.width, instance.x + instance.w);
				layout.height = std::max(layout.height, instance.y + instance.h);

				layout.glyphs.emplace_back(instance);
			}

			penX += advance;

			// apply kerning
			/*if (i + 1 < text.size()) {
				// TODO: cache
				int kerning = getHorizontalKerning(textSettings.size, code, text[i + 1]);
				penX += (float)kerning;
			}*/
		}

		return layout;
    }

	bool Font::setSize(FontSize size)
	{
		if (size == m_CurrentSize)
			return true;

		FT_Error error;

		if (FT_HAS_FIXED_SIZES(m_Face)) {
			// fixed size font, ignore given size... ?
			size = m_Face->available_sizes[0].height;
		}

		if (FT_HAS_FIXED_SIZES(m_Face)) {
			if ((error = FT_Select_Size(m_Face, 0)) != FT_Err_Ok) {
				// std::cerr << "Could not set pixel size!" << FT_Error_String(error) << std::endl;
				return false;
			}
		}
		else {
			if ((error = FT_Set_Pixel_Sizes(m_Face, 0, (FT_UInt)size)) != FT_Err_Ok) {
				// std::cerr << "Could not set pixel size!" << FT_Error_String(error) << std::endl;
				return false;
			}
		}

		m_CurrentSize = size;

		return true;
	}

}

