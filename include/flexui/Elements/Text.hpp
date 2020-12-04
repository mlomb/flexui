#pragma once

#include <flexui/Element.hpp>

namespace flexui {
	/// Text UI element
	class Text : public Element {
	public:
		Text();
		virtual ~Text();

		void setText(const std::string& text);

	private:
		std::string m_Text;

		void paintContent(Painter* painter) override;
		UIVec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode) override;
	};
}