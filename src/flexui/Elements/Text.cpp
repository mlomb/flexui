#include "flexui/Elements/Text.hpp"

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

		void* font = nullptr;
		if (font == nullptr || m_Text.size() == 0)
			return;

		auto box = getContentRect();

		/*
		Graphics::TextSettings textSettings;
		textSettings.size = m_ComputedStyle->fontSize.value.number; // TODO: units
		textSettings.wordWrap = true;
		textSettings.wordWrapWidth = box.width;

		painter->drawText(getTextLayout(textSettings, false), box.position, m_ComputedStyle->color.value);
		*/
	}

	UIVec2 Text::measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode)
	{
		// See https://github.com/facebook/yoga/pull/576/files

		float measuredWidth = NAN; // NAN from corecrt_math.h
		float measuredHeight = NAN;

		void* font = nullptr;
		if (font == nullptr || m_Text.size() == 0)
			return UIVec2(measuredWidth, measuredHeight);

		/*
		if (widthMode == MeasureMode::EXACTLY) {
			measuredWidth = width;
		}
		else {
			Graphics::TextSettings textSettings;
			textSettings.size = m_ComputedStyle->fontSize.value.number; // TODO: units
			textSettings.wordWrap = false;

			measuredWidth = std::ceil(getTextLayout(textSettings, true).boundingSize.x);

			if (widthMode == MeasureMode::AT_MOST)
				measuredWidth = std::min(measuredWidth, width);
		}

		if (heightMode == MeasureMode::EXACTLY) {
			measuredHeight = height;
		}
		else {
			Graphics::TextSettings textSettings;
			textSettings.size = m_ComputedStyle->fontSize.value.number; // TODO: units
			textSettings.wordWrap = true;
			textSettings.wordWrapWidth = measuredWidth;

			measuredHeight = std::ceil(getTextLayout(textSettings, true).boundingSize.y);

			if (heightMode == MeasureMode::AT_MOST)
				measuredHeight = std::min(measuredHeight, height);
		}

		*/
		return UIVec2(measuredWidth, measuredHeight);
	}

	/*
	const Graphics::TextLayout& Text::getTextLayout(const Graphics::TextSettings& settings, bool skip_glyphs)
	{
		OE_ASSERT(m_Font);

		for (const Graphics::TextLayout& tl : m_TextLayoutCache) {
			if (!skip_glyphs && tl.no_glyphs)
				continue; // we need glyph data
			if (settings.wordWrap == tl.settings.wordWrap &&
				(!settings.wordWrap || settings.wordWrapWidth == tl.settings.wordWrapWidth))
				return tl;
		}
		m_TextLayoutCache.emplace_back(m_Font->generateTextLayout(m_Text, settings));
		return m_TextLayoutCache.back();
	}
	*/
}
