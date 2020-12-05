#pragma once

#include <string>
#include <vector>
#include <unordered_map>

/* Forward defs */
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;
struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;

namespace flexui {
	/// Unicode codepoint
	typedef unsigned int GlyphCodepoint;
	/// Font sizes specified in pixels
	typedef unsigned int FontSize;

	/**
		@brief Single glyph metrics
		[Details](https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html)
		@image html glyph-metrics.png "Glyph metrics"
	*/
	struct GlyphMetrics {
		int width, height;
		int H_advance, H_bearingX, H_bearingY;
		int V_advance, V_bearingX, V_bearingY;
		bool valid;
	};

	struct TextStyle {
		FontSize size;
		bool bold, italic;
	};

	struct TextLayoutSettings {
		bool wordWrap;
		int wordWrapWidth;
	};

	struct TextLayout {
		struct GlyphInstance {
			GlyphCodepoint codepoint;
			int x, y, w, h;
			int index; // original index in string
		};

		std::vector<GlyphInstance> glyphs;
		int width, height;
		TextStyle style;
	};

	class Font {
	public:
		~Font();

		/// Load a font file from memory
		static Font* LoadFromMemory(const unsigned char* data, const size_t size);

		/// Retrieve the metrics of a Glyph
		bool getGlyphMetrics(GlyphCodepoint codepoint, const TextStyle& style, GlyphMetrics& metrics);

		/// Render a Glyph to RGBA format
		bool renderGlyph(GlyphCodepoint codepoint, const TextStyle& style, unsigned char*& data, unsigned int& width, unsigned int& height);

		/// Generate the layout of a given text
		TextLayout generateTextLayout(const std::string& text, const TextStyle& textStyle, const TextLayoutSettings& layoutSettings, bool skip_glyphs = false);

	private:
		static FT_Library s_FT_Library;

		Font(FT_Face face);

		/// handle to the freetype face
		FT_Face m_Face;

		/// internal function to set the current freetype pixel sizes
		bool setSize(FontSize size);
		/// cache the selected size to prevent calling freetype multiple times
		FontSize m_CurrentSize;

		typedef uint64_t GlyphHash; // codepoint + size
		std::unordered_map<GlyphHash, GlyphMetrics> m_MetricsCache;
	};

}