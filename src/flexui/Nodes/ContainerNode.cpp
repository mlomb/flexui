#include "flexui/Nodes/ContainerNode.hpp"

#include "flexui/Log.hpp"

namespace flexui {

	ContainerNode::ContainerNode() :
		m_FirstChild(nullptr),
		m_LastChild(nullptr)
	{
	}

	void ContainerNode::appendChild(Node* child_node)
	{
		FUI_ASSERT(child_node->getParentNode() == nullptr); // for now

		ContainerNode* this_node = static_cast<ContainerNode*>(this);
		child_node->_setParent(this);
		if (m_LastChild) {
			child_node->_setPreviousSibling(m_LastChild);
			m_LastChild->_setNextSibling(child_node);
		}
		else {
			_setFirstChild(child_node);
		}
		_setLastChild(child_node);
	}

}
