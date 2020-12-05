#include "flexui/Elements/Text.hpp"

#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Providers/ResourceProvider.hpp"
#include "flexui/Render/Font.hpp"
#include "flexui/Surface.hpp"
#include "flexui/Render/Painter.hpp"

namespace flexui {
	Text::Text() : Element()
	{
		enableMeasurement();
		setAsTextType();
		setTag("Text");
	}

	Text::~Text()
	{
	}

	void Text::setText(const std::string& text)
	{
		m_Text = text;
	}

	void Text::paintContent(Painter* painter)
	{
		// draw background
		Element::paintContent(painter);

		if (m_Text.size() == 0)
			return;

		// TODO!
		// m_ComputedStyle->fontFamily.value;
		std::string fontFamily = "default";
		std::shared_ptr<Font> font = getSurface()->getResourceProvider()->getFont(fontFamily);

		if (font == nullptr)
			return;

		auto box = getContentRect();

		TextStyle textStyle;
		textStyle.size = m_ComputedStyle->fontSize.value.number; // TODO: units

		TextLayoutSettings layoutSettings;
		layoutSettings.wordWrap = true;
		layoutSettings.wordWrapWidth = box.width;

		painter->drawText(font.get(), font->generateTextLayout(m_Text, textStyle, layoutSettings, false), box.position, m_ComputedStyle->color.value);
	}

	Vec2 Text::measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode)
	{
		// See https://github.com/facebook/yoga/pull/576/files

		float measuredWidth = NAN; // NAN from corecrt_math.h
		float measuredHeight = NAN;

		std::string fontFamily = "Proggy Tiny";
		std::shared_ptr<Font> font = getSurface()->getResourceProvider()->getFont(fontFamily);

		if (font == nullptr || m_Text.size() == 0)
			return Vec2(measuredWidth, measuredHeight);

		TextStyle textStyle;
		textStyle.size = m_ComputedStyle->fontSize.value.number; // TODO: units

		if (widthMode == MeasureMode::EXACTLY) {
			measuredWidth = width;
		}
		else {
			TextLayoutSettings layoutSettings;
			layoutSettings.wordWrap = false;

			measuredWidth = std::ceil(font->generateTextLayout(m_Text, textStyle, layoutSettings, true).width);

			if (widthMode == MeasureMode::AT_MOST)
				measuredWidth = std::min(measuredWidth, width);
		}

		if (heightMode == MeasureMode::EXACTLY) {
			measuredHeight = height;
		}
		else {
			TextLayoutSettings layoutSettings;
			layoutSettings.wordWrap = false;
			layoutSettings.wordWrapWidth = measuredWidth;

			measuredWidth = std::ceil(font->generateTextLayout(m_Text, textStyle, layoutSettings, true).height);

			if (heightMode == MeasureMode::AT_MOST)
				measuredHeight = std::min(measuredHeight, height);
		}

		return Vec2(measuredWidth, measuredHeight);
	}
}
