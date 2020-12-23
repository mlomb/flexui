#include "flexui/Elements/Text.hpp"

#include "flexui/Providers/ResourceProvider.hpp"
#include "flexui/Render/Font.hpp"
#include "flexui/Render/Painter.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Surface.hpp"

#include <cassert>
#include <cmath>
#include <codecvt>
#include <locale>

#ifndef NAN
#define NAN (nan(NULL))
#else
// NAN from corecrt_math.h
#endif

namespace flexui {
    Text::Text()
        : Element()
    {
        enableMeasurement();
        setAsTextType();
        setTag("Text");
        m_FontNameTEST = "default";
    }

    Text::~Text() { }

    void Text::setText(const std::string& text)
    {
        m_Text = text;
    }

    void Text::setTextToAllGlyphsTEST()
    {
        std::shared_ptr<Font> font = getSurface()->getResourceProvider()->getFont(m_FontNameTEST);
        std::vector<GlyphCodepoint> glyphs;
        font.get()->getAvailableGlyphs(glyphs);
        std::string s;
        int i = 0;
        for (GlyphCodepoint& c : glyphs) {
            std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
            std::string u8str = converter.to_bytes(c);
            for (int j = 0; j < u8str.size(); j++) {
                if (u8str[j] == 0) {
                    break;
                }
                s.push_back(u8str[j]);
            }
            i++;
        }
        m_Text = s + " \nHello World AVWVWA AV Te";
    }

    void Text::paintContent(Painter* painter)
    {
        // draw background
        Element::paintContent(painter);

        if (m_Text.size() == 0)
            return;

        std::shared_ptr<Font> font = getSurface()->getResourceProvider()->getFont(m_FontNameTEST);

        if (font == nullptr)
            return;

        auto box = getContentRect();

        TextLayoutSettings layoutSettings;
        layoutSettings.style.size = m_ComputedStyle->fontSize.value.number; // TODO: units
        layoutSettings.wrap = TextWrap::WORD;
        layoutSettings.maxWidth = box.width;

        TextLayout layout;
        font->generateTextLayout(m_Text, layoutSettings, layout);
        painter->drawText(font.get(), layout, box.position, m_ComputedStyle->color.value);
    }

    Vec2 Text::measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode)
    {
        // See https://github.com/facebook/yoga/pull/576/files

        float measuredWidth = NAN;
        float measuredHeight = NAN;

        std::shared_ptr<Font> font = getSurface()->getResourceProvider()->getFont(m_FontNameTEST);

        if (font == nullptr || m_Text.size() == 0)
            return Vec2(measuredWidth, measuredHeight);

        TextLayout layout;

        TextLayoutSettings layoutSettings;
        layoutSettings.skip_glyphs = true; // we don't need each glyph
        layoutSettings.style.size = m_ComputedStyle->fontSize.value.number; // TODO: units

        if (widthMode == MeasureMode::EXACTLY) {
            measuredWidth = width;
        } else {
            layoutSettings.wrap = TextWrap::NONE;

            font->generateTextLayout(m_Text, layoutSettings, layout);
            measuredWidth = std::ceil(layout.width);

            if (widthMode == MeasureMode::AT_MOST)
                measuredWidth = std::min(measuredWidth, width);
        }

        if (heightMode == MeasureMode::EXACTLY) {
            measuredHeight = height;
        } else {
            layoutSettings.wrap = TextWrap::WORD;
            layoutSettings.maxWidth = measuredWidth;

            font->generateTextLayout(m_Text, layoutSettings, layout);
            measuredHeight = std::ceil(layout.height);

            if (heightMode == MeasureMode::AT_MOST)
                measuredHeight = std::min(measuredHeight, height);
        }

        return Vec2(measuredWidth, measuredHeight);
    }
}
