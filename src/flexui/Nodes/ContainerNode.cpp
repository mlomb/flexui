#include "flexui/Nodes/ContainerNode.hpp"

#include "flexui/Log.hpp"
#include "flexui/Layout/LayoutObject.hpp"
#include "flexui/Layout/Yoga.hpp"

namespace flexui {

	ContainerNode::ContainerNode() :
		m_FirstChild(nullptr),
		m_LastChild(nullptr)
	{
	}

	ContainerNode::~ContainerNode()
	{
	}

	void ContainerNode::appendChild(Node* child_node)
	{
		// remove child from parent first
		if (child_node->getParentNode()) {
			FUI_ASSERT(child_node->isContainerNode());
			static_cast<ContainerNode*>(child_node->getParentNode())->removeChild(child_node);
		}
		FUI_ASSERT(child_node->getParentNode() == nullptr);

		child_node->m_Parent = this;
		if (m_LastChild) {
			child_node->m_PreviousSibling = m_LastChild;
			m_LastChild->m_NextSibling = child_node;
		}
		else {
			m_FirstChild = child_node;
		}
		m_LastChild = child_node;

		// keep tree in sync (TODO: find a better way to do this)
		if (child_node->getLayoutObject()) {
			if (YGNode* yogaNode = getLayoutObject()->getYogaNode()) {
				YGNodeInsertChild(yogaNode, child_node->getLayoutObject()->getYogaNode(), YGNodeGetChildCount(yogaNode));
			}
		}

		child_node->m_Document = m_Document;
		if(child_node->isContainerNode())
			static_cast<ContainerNode*>(child_node)->propagateDocument();
	}

	void ContainerNode::removeChild(Node* child_node)
	{
		// TODO: !
	}

	void ContainerNode::propagateDocument()
	{
		for (Node* child = getFirstChild(); child; child = child->getNextSibling()) {
			if (child->m_Document != m_Document) {
				child->m_Document = m_Document;
				if (child->isContainerNode())
					static_cast<ContainerNode*>(child)->propagateDocument();
			}
		}
	}

}
