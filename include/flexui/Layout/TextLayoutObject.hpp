#pragma once

#include "flexui/Layout/LayoutObject.hpp"

namespace flexui {

	class Text;

	class TextLayoutObject : public LayoutObject {
	public:
		TextLayoutObject(Text* text);
		~TextLayoutObject();

		Text* getTextNode() const { return m_Text; }

	private:
		Text* m_Text;
	};

}
