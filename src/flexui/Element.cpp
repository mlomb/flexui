#include "Element.hpp"

#include "Layout/Yoga.hpp"

namespace flexui {

	Element::Element() :
		m_Surface(nullptr),
		m_Parent(nullptr),
		m_Depth(0),
		m_ComputedStyle(nullptr),
		m_PseudoStates(StylePseudoStates::NONE)
	{
		setID("");
		setTag("Element");

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
		if (index < 0)
			index = (int)m_Childrens.size();

		// this function sets the correct owner in yoga land
		YGNodeInsertChild(m_YogaNode, child->m_YogaNode, index);

		if (child->m_Parent != nullptr) {
			if (child->m_Parent == this) {
				// TODO: handle reposition
				return;
			}
			else {
				child->m_Parent->removeElement(child);
			}
		}

		m_Childrens.insert(m_Childrens.begin() + index, child);
		child->m_Parent = this;
		child->updateHierarchy();
	}

	void Element::removeElement(Element* child)
	{
		assert(child->m_Parent == this); // TODO: maybe do a runtime check?

		child->m_Parent = nullptr;
		m_Childrens.erase(std::find(m_Childrens.begin(), m_Childrens.end(), child));
		child->updateHierarchy();
	}

	void Element::updateHierarchy()
	{
		assert(m_Parent);
		m_Depth = m_Parent->m_Depth + 1;
		m_Surface = m_Parent->m_Surface;
		for (Element* e : m_Childrens)
			e->updateHierarchy();
	}

	Element* Element::getParent() const
	{
		return m_Parent;
	}

	const std::vector<Element*>& Element::getChildrens() const
	{
		return m_Childrens;
	}

	const std::vector<StyleSheet*>& Element::getStyleSheets() const
	{
		return m_StyleSheets;
	}

	void Element::setID(const std::string& id)
	{
		m_ID = { StyleIdentifierType::ID, id };
		m_ID.computeHash();
	}

	void Element::setTag(const std::string& tag)
	{
		m_Tag = { StyleIdentifierType::TAG, tag };
		m_Tag.computeHash();
	}

	void Element::addStyleSheet(StyleSheet* stylesheet)
	{
		m_StyleSheets.emplace_back(stylesheet);
	}

}
