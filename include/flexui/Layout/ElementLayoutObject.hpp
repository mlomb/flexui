#pragma once

#include "flexui/Layout/LayoutObject.hpp"

namespace flexui {

	class Element;

	class ElementLayoutObject : public LayoutObject {
	public:
		ElementLayoutObject(Element* element);
		~ElementLayoutObject();

		void syncStyles() const;

	private:
		Element* m_Element;
	};

}
