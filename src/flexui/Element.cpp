#include "Element.hpp"

#include "Layout/Yoga.hpp"

namespace flexui {

	Element::Element() :
		m_Parent(nullptr),
		m_Depth(0)
	{
		m_YogaNode = YGNodeNew();
		YGNodeSetContext(m_YogaNode, this);
	}

	Element::~Element()
	{
		assert(m_Parent == nullptr && "Element is still attached to the tree");
		
		YGNodeFreeRecursive(m_YogaNode);
	}
	
	void Element::addElement(Element* child, int index)
	{
	}

	void Element::removeElement(Element* child)
	{
	}
}
