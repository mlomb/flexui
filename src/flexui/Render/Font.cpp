#include "flexui/Render/Font.hpp"

#include "flexui/Log.hpp"

// FreeType 2
#include <ft2build.h>
#include FT_FREETYPE_H

constexpr auto MAX_VALID_UNICODE = 0x10FFFF;

// https://github.com/skeeto/branchless-utf8
static int utf8_decode(uint32_t* c, const uint8_t* buf, const uint8_t* buf_end)
{
    static const char lengths[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 3, 3, 4, 0 };
    static const int masks[] = { 0x00, 0x7f, 0x1f, 0x0f, 0x07 };
    static const uint32_t mins[] = { 4194304, 0, 128, 2048, 65536 };
    static const int shiftc[] = { 0, 18, 12, 6, 0 };
    static const int shifte[] = { 0, 6, 4, 2, 0 };

    unsigned char s[4];
    s[0] = buf + 0 < buf_end ? buf[0] : 0;
    s[1] = buf + 1 < buf_end ? buf[1] : 0;
    s[2] = buf + 2 < buf_end ? buf[2] : 0;
    s[3] = buf + 3 < buf_end ? buf[3] : 0;

    int len = lengths[s[0] >> 3];

    /* Compute the pointer to the next character early so that the next
     * iteration can start working on the next character. Neither Clang
     * nor GCC figure out this reordering on their own.
     */
    unsigned char* next = s + len + !len;

    /* Assume a four-byte character and load four bytes. Unused bits are
     * shifted out.
     */
    *c = (uint32_t)(s[0] & masks[len]) << 18;
    *c |= (uint32_t)(s[1] & 0x3f) << 12;
    *c |= (uint32_t)(s[2] & 0x3f) << 6;
    *c |= (uint32_t)(s[3] & 0x3f) << 0;
    *c >>= shiftc[len];

    /* Accumulate the various error conditions. */
    int e = 0;
    e = (*c < mins[len]) << 6; // non-canonical encoding
    e |= ((*c >> 11) == 0x1b) << 7; // surrogate half?
    e |= (*c > 0x10FFFF) << 8; // out of range?
    e |= (s[1] & 0xc0) >> 2;
    e |= (s[2] & 0xc0) >> 4;
    e |= (s[3]) >> 6;
    e ^= 0x2a; // top two bits of each tail byte correct?
    e >>= shifte[len];

    return e ? (!!s[0] + !!s[1] + !!s[2] + !!s[3]) : (len + !len);
}

namespace flexui {

    FT_Library Font::s_FT_Library = nullptr;

    Font* Font::LoadFromMemory(const void* data, const size_t size)
    {
        if (FT_Init_FreeType(&s_FT_Library) != 0) { // TODO: this is not multithread friendly
            FUI_LOG_FATAL("%s", "Could not initialize FreeType library");
            return nullptr;
        } // TODO: close the library...

        FT_Error error;
        FT_Face face;

        if ((error = FT_New_Memory_Face(Font::s_FT_Library, (FT_Byte*)data, (FT_Long)size, 0, &face)) == FT_Err_Ok) {
            // select encoding
            if ((error = FT_Select_Charmap(face, FT_ENCODING_UNICODE)) == FT_Err_Ok) {
                static unsigned int s_counter = 0;
                return new Font(face, s_counter++);
            } else {
                FUI_LOG_WARN("Could not select the Unicode charmap, error=%d", error);
            }
        } else {
            FUI_LOG_WARN("Could not load font, error=%d", error);
        }

        return nullptr;
    }

    Font::Font(FT_Face face, unsigned int id)
        : m_Face(face)
        , m_ID(id)
    {
    }

    Font::~Font()
    {
        FT_Done_Face(m_Face);
    }

    bool Font::getGlyphDescription(GlyphCodepoint codepoint, const TextStyle& style, GlyphDescription& description)
    {
        GlyphHash idx = (style.bold * ((GlyphHash)1 << 31)) | // 1 bit
            (style.italic * ((GlyphHash)1 << 30)) | // 1 bit
            ((GlyphHash)style.size << 21) | // 9 bits
            codepoint; // 21 bits (MAX_VALID_UNICODE)
        auto desc_it = m_DescriptionCache.find(idx);
        if (desc_it != m_DescriptionCache.end()) {
            // already computed
            description = (*desc_it).second;
            return description.valid;
        }

        FT_Int32 load_flags = FT_LOAD_DEFAULT;
        if (FT_HAS_COLOR(m_Face)) {
            // some bitmap glyphs may not return
            // metrics if they are not rendered (idk why)
            // example: TwemojiMozilla.ttf
            // TODO: this is expensive, fixme
            load_flags |= FT_LOAD_RENDER | FT_LOAD_COLOR;
        }

        if (!loadGlyph(codepoint, style.size, load_flags)) {
            description.valid = false;
            m_DescriptionCache.insert(std::make_pair(idx, description));
            return false;
        }

        bool is_control = codepoint == ' ' || codepoint == '\t' || codepoint == '\r' || codepoint == '\n';

        if (!is_control && m_Face->glyph->metrics.width == 0 && m_Face->glyph->bitmap.width == 0) {
            FUI_LOG_WARN("Loaded glyph U+%04X doesn't have size", codepoint);
            description.valid = false;
            m_DescriptionCache.insert(std::make_pair(idx, description));
            return false;
        }

        description.valid = true;
        description.has_colors = FT_HAS_COLOR(m_Face) && m_Face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_BGRA;
        description.width = (m_Face->glyph->metrics.width >> 6);
        description.height = (m_Face->glyph->metrics.height >> 6);
        description.advance = (m_Face->glyph->metrics.horiAdvance >> 6);
        description.bearingX = (m_Face->glyph->metrics.horiBearingX >> 6);
        description.bearingY = (m_Face->glyph->metrics.horiBearingY >> 6);

        if (description.width == 0) {
            // if metrics doesn't contain the size
            // of the glyph, use the bitmap dimensions
            description.width = m_Face->glyph->bitmap.width;
            description.height = m_Face->glyph->bitmap.rows;
        }

        m_DescriptionCache.insert(std::make_pair(idx, description));

        return true;
    }

    bool Font::rasterizeGlyph(GlyphCodepoint codepoint, const TextStyle& style, unsigned char*& data, unsigned int& width, unsigned int& height)
    {
        FT_Int32 load_flags = FT_LOAD_RENDER;
        if (FT_HAS_COLOR(m_Face)) {
            // render colored glyphs like emojis
            load_flags |= FT_LOAD_COLOR;
        }

        if (!loadGlyph(codepoint, style.size, load_flags)) {
            return false;
        }

        if (m_Face->glyph->bitmap.buffer == nullptr) {
            FUI_LOG_WARN("Loaded glyph has a null bimap buffer codepoint=U+%04X", codepoint);
            return false;
        }

        width = m_Face->glyph->bitmap.width;
        height = m_Face->glyph->bitmap.rows;
        FUI_DEBUG_ASSERT(width * height > 0);
        data = (unsigned char*)malloc(width * height * 4);
        if (!data)
            return false; // malloc failed

        int pixels = width * height;
        auto source = m_Face->glyph->bitmap.buffer;

        switch (m_Face->glyph->bitmap.pixel_mode) {
        case FT_Pixel_Mode::FT_PIXEL_MODE_GRAY: {
            for (int i = 0; i < pixels; i++) {
                data[i * 4 + 0] = source[i];
                data[i * 4 + 1] = source[i];
                data[i * 4 + 2] = source[i];
                data[i * 4 + 3] = 255;
            }
            return true;
        }
        case FT_Pixel_Mode::FT_PIXEL_MODE_BGRA: {
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

    void Font::generateTextLayout(const std::string& utf8_text, const TextLayoutSettings& layoutSettings, TextLayout& layout)
    {
        // TODO: alignment
        // TODO: use Harfbuzz as an optional dependency
        //       to generate the shaping
        // TODO: bidi?
        // TODO: run a proper justification algorithm (Knuth/Plass maybe)

        layout.width = 0;
        layout.height = 0;
        layout.style = layoutSettings.style;
        if (!layoutSettings.skip_glyphs) {
            // reserve some initial space for the glyphs
            // some may be unused if a unicode codepoint spans
            // over multiple chars
            layout.glyphs.reserve(utf8_text.size());
        }

        const uint8_t* text_start = (uint8_t*)utf8_text.c_str();
        const uint8_t* text_end = (uint8_t*)utf8_text.c_str() + utf8_text.size();

        GlyphCodepoint code, prev_code;
        GlyphDescription description;
        TextLayout::GlyphInstance instance;
        int i = 0;
        int last_whitespace_i = 0;
        bool last_whitespace_was_wrap = false;
        int penX = 0, penY = layoutSettings.style.size;

        while (i < utf8_text.size()) {
            int code_length = utf8_decode(&code, text_start + i, text_end); // length in bytes

            FUI_DEBUG_ASSERT(code > 0 && code_length > 0);

            if (getGlyphDescription(code, layoutSettings.style, description)) {
                bool is_jump = code == '\n' || code == '\r';
                bool is_whitespace = code == ' ' || code == '\t';
                bool should_wrap = false;

                if (layoutSettings.maxWidth > 0) {
                    if (is_whitespace) {
                        // keep track of whitespaces
                        last_whitespace_i = i;
                        last_whitespace_was_wrap = false;
                    } else {
                        if (penX + description.advance > layoutSettings.maxWidth) { // should wrap?
                            if (layoutSettings.wordWrap) { // wrap the whole last word
                                if (!last_whitespace_was_wrap) { // avoid infinite loop
                                    i = last_whitespace_i;
                                    last_whitespace_was_wrap = true;
                                    should_wrap = true;

                                    // remove all glyphs after the last space
                                    while (layout.glyphs.size() > 0 && layout.glyphs.back().index >= last_whitespace_i)
                                        layout.glyphs.pop_back();
                                }
                            } else {
                                // wrap chars
                                should_wrap = true;
                            }
                        }
                    }
                }

                if (is_jump || should_wrap) {
                    penX = 0;
                    penY += layoutSettings.style.size;
                    if (should_wrap) {
                        prev_code = 0;
                        continue; // do not increment i
                    }
                } else {
                    instance = {};
                    instance.codepoint = code;
                    instance.index = i;
                    instance.x = penX + description.bearingX;
                    instance.y = penY - description.bearingY;
                    instance.w = description.width;
                    instance.h = description.height;
                    instance.has_colors = description.has_colors;

                    layout.width = std::max(layout.width, instance.x + instance.w);
                    layout.height = std::max(layout.height, instance.y + instance.h);

                    if (!layoutSettings.skip_glyphs)
                        layout.glyphs.emplace_back(instance);

                    penX += description.advance;

                    // apply kerning
                    if (i)
                        penX -= getKerning(prev_code, code, layoutSettings.style.size);
                }
            }

            prev_code = code;
            i += code_length;
        }
    }

    bool Font::setCurrentSize(FontSize pixel_height) const
    {
        FT_Error error;

        if (FT_IS_SCALABLE(m_Face)) {
            // try to avoid calls to FT_Set_Pixel_Sizes
            // since it is an expensive function
            if (pixel_height != m_Face->size->metrics.x_ppem) {
                error = FT_Set_Pixel_Sizes(m_Face, 0, pixel_height);
            } else {
                error = FT_Err_Ok;
            }
        } else {
            FUI_DEBUG_ASSERT(m_Face->num_fixed_sizes > 0);
            int best_i = 0;

            // font has fixed sizes
            // find the closest height to target (and greater)
            for (int i = 1; i < m_Face->num_fixed_sizes; i++) {
                if (m_Face->available_sizes[i].y_ppem < (FT_Pos)pixel_height) {
                    // it is lower, but consider it if it is
                    // better than the current best
                    if (m_Face->available_sizes[i].y_ppem > m_Face->available_sizes[best_i].y_ppem)
                        best_i = i;
                } else if (m_Face->available_sizes[i].y_ppem < m_Face->available_sizes[best_i].y_ppem) {
                    // it is greater but closer
                    best_i = i;
                }
            }

            // select this strike
            error = FT_Select_Size(m_Face, best_i);
        }

        if (error != FT_Err_Ok) {
            FUI_LOG_WARN("Could not set pixel size size=%d, error=%d", pixel_height, error);
            return false;
        }

        return true;
    }

    bool Font::loadGlyph(GlyphCodepoint codepoint, FontSize pixel_height, int load_flags)
    {
        // lookup unicode -> glyph index
        uint32_t glyph_index = FT_Get_Char_Index(m_Face, codepoint);
        if (glyph_index == 0) {
            FUI_LOG_WARN("Could not find glyph index for U+%04X", codepoint);
            return false;
        }

        // set desired size
        if (!setCurrentSize(pixel_height)) {
            return false;
        }

        // load glyph
        FT_Error error;
        if ((error = FT_Load_Glyph(m_Face, glyph_index, load_flags)) != FT_Err_Ok) {
            FUI_LOG_WARN("Could not load glyph U+%04X, glyph_index=%d, error=%d", codepoint, glyph_index, error);
            return false;
        }

        return true;
    }

    float Font::getKerning(GlyphCodepoint left, GlyphCodepoint right, FontSize pixel_height)
    {
        if (left > 0 && right > 0 && FT_HAS_KERNING(m_Face) && setCurrentSize(pixel_height)) {
            FT_UInt left_index = FT_Get_Char_Index(m_Face, left);
            FT_UInt right_index = FT_Get_Char_Index(m_Face, right);

            FT_Vector kerning;
            FT_Get_Kerning(m_Face, left_index, right_index, FT_KERNING_DEFAULT, &kerning);

            if (!FT_IS_SCALABLE(m_Face))
                return kerning.x; // already in pixels

            return (float)kerning.x / (float)(1 << 6);
        }
        return 0.0f;
    }

    void Font::getAvailableGlyphs(std::vector<GlyphCodepoint>& glyphs) const
    {
        glyphs.reserve(m_Face->num_glyphs);

        FT_UInt codepoint;
        FT_ULong charcode = FT_Get_First_Char(m_Face, &codepoint);

        while (codepoint != 0) {
            glyphs.push_back(charcode);
            charcode = FT_Get_Next_Char(m_Face, charcode, &codepoint);
        }
    }

    unsigned int Font::getID() const
    {
        return m_ID;
    }
}
