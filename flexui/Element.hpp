#pragma once

#include <vector>

// forward def to avoid include
class YGNode;

namespace flexui {
	
	class Element {
	public:
		Element();
		virtual ~Element();

		void addElement(Element* child, int index = -1);
		void removeElement(Element* child);


	private:
		Element* m_Parent;
		std::vector<Element*> m_Childrens;
		int m_Depth;

		YGNode* m_YogaNode;
	};

}
