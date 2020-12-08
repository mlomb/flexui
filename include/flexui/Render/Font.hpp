#pragma once

#include <string>
#include <unordered_map>
#include <vector>

/* Forward defs to avoid including FreeType */
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;
struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;

namespace flexui {
    /// Unicode codepoint
    typedef uint32_t GlyphCodepoint;

    /// Font size specified in pixels
    typedef unsigned int FontSize;

    /// Text alignment
    enum class TextAlign { LEFT, CENTER, RIGHT, JUSTIFY };

    /// Text style
    struct TextStyle {
        FontSize size = 12;
        bool bold = false, italic = false;
    };

    struct TextLayoutSettings {
        TextStyle style = {};
        TextAlign align = TextAlign::LEFT;
        bool wordWrap = true;
        // 0 means no max
        int maxWidth = 0;
        /// enable this flag if you don't need to
        /// generate every glyph position
        /// (useful if you only need the bounds)
        bool skip_glyphs = false;
    };

    struct TextLayout {
        /// A single glyph positioned in the layout
        struct GlyphInstance {
            GlyphCodepoint codepoint;
            int x, y, w, h;
            // original char index in string
            // (first byte of the codepoint)
            int index;
            bool has_colors;
        };

        int width, height;
        TextStyle style;
        std::vector<GlyphInstance> glyphs;
    };

    /// Single glyph description
    /// [For metrics details](https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html)
    struct GlyphDescription {
        bool valid;
        bool has_colors;
        int width, height;
        int advance;
        int bearingX, bearingY;
    };

    /// Represents a single typeface
    class Font {
    public:
        /// Load a font file from memory
        static Font* LoadFromMemory(const void* data, const size_t size);

        ~Font();

        ///  Returns all available codepoint in the font
        void getAvailableGlyphs(std::vector<GlyphCodepoint>& glyphs) const;

        /// Retrieve the metrics of a Glyph
        bool getGlyphDescription(GlyphCodepoint codepoint, const TextStyle& style, GlyphDescription& description);

        /// Render a Glyph to RGBA format
        bool rasterizeGlyph(GlyphCodepoint codepoint, const TextStyle& style, unsigned char*& data, unsigned int& width, unsigned int& height);

        /// Generate the layout of a given text
        void generateTextLayout(const std::string& utf8_text, const TextLayoutSettings& layoutSettings, TextLayout& layout);

        /// Return the instance counter for this font
        /// Useful for caching
        unsigned int getID() const;

    private:
        static FT_Library s_FT_Library;

        Font(FT_Face face, unsigned int id);

        /// font counter
        unsigned int m_ID;

        /// handle to the freetype face
        FT_Face m_Face;

        /// set the current freetype pixel sizes
        bool setCurrentSize(FontSize pixel_height) const;

        /// load a glyph to the face slot
        bool loadGlyph(GlyphCodepoint codepoint, FontSize pixel_height, int load_flags);

        /// kerning between two codepoints
        float getKerning(GlyphCodepoint left, GlyphCodepoint right, FontSize pixel_height);

        typedef uint32_t GlyphHash; // codepoint + size + styles
        std::unordered_map<GlyphHash, GlyphDescription> m_DescriptionCache;
    };

}