#pragma once

#include <string>
#include <vector>

#include <flexui/Element.hpp>

namespace flexui {

	struct UIVertex;

	/// Text UI element
	class Text : public Element {
	public:
		Text();
		virtual ~Text();

		void setText(const std::string& text);

		std::string m_FontNameTEST;
		void setTextToAllGlyphsTEST();

	private:
		std::string m_Text;

		std::vector<UIVertex> m_TextVerts;

		void paintContent(Painter* painter) override;
		Vec2 measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode) override;
	};
}