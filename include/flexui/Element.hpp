#pragma once

#include <vector>

// forward def to avoid include
struct YGNode;

#include "Structs.hpp"

namespace flexui {

	class Element {
	public:
		Element();
		virtual ~Element();

		void addElement(Element* child, int index = -1);
		void removeElement(Element* child);

	private:
		friend class Surface;
		friend class LayoutTreeUpdater;

		Element* m_Parent;
		std::vector<Element*> m_Childrens;
		int m_Depth;

		YGNode* m_YogaNode;
		UIRect m_LayoutRect;
		UIRect m_BoundingRect;

		Surface* m_Surface;
	};

}
