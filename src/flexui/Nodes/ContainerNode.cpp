#include "flexui/Nodes/ContainerNode.hpp"

#include "flexui/Log.hpp"
#include "flexui/Nodes/Document.hpp"

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
		if (child_node->m_Parent) {
			FUI_ASSERT(child_node->isContainerNode());
			static_cast<ContainerNode*>(child_node->m_Parent)->removeChild(child_node);
		}
		FUI_ASSERT(child_node->m_Parent == nullptr);

		child_node->m_Parent = this;
		if (m_LastChild) {
			child_node->m_PreviousSibling = m_LastChild;
			m_LastChild->m_NextSibling = child_node;
		}
		else {
			m_FirstChild = child_node;
		}
		m_LastChild = child_node;

		AttachToTree(child_node);
	}

	void ContainerNode::removeChild(Node* child_node)
	{
		FUI_ASSERT(child_node->m_Parent == this);

		child_node->m_Parent = nullptr;

		// connect pointers
		if (child_node->m_PreviousSibling) {
			child_node->m_PreviousSibling->m_NextSibling = child_node->m_NextSibling;
		}
		else {
			m_FirstChild = child_node->m_NextSibling;
		}
		if (child_node->m_NextSibling) {
			child_node->m_NextSibling->m_PreviousSibling = child_node->m_PreviousSibling;
		}
		else {
			m_LastChild = child_node->m_PreviousSibling;
		}

		DeatchFromTree(child_node);
	}

	void ContainerNode::AttachToTree(Node* child)
	{
		Node* parent = child->getParentNode(); FUI_ASSERT(parent);
		Document* doc = parent->m_Document;

		// the child shouldn't be attached to the tree
		FUI_ASSERT(child->m_Document == nullptr);

		child->m_Document = doc;
		child->m_Depth = child->getParentNode()->m_Depth + 1;

		if (doc)
			doc->_attachedToTree(child);

		if (child->isContainerNode()) {
			Node* sub_child = static_cast<ContainerNode*>(child)->getFirstChild();
			while (sub_child) {
				AttachToTree(sub_child);
				sub_child = sub_child->getNextSibling();
			}
		}
	}

	void ContainerNode::DeatchFromTree(Node* child)
	{
		// the child should be attached to the tree
		FUI_ASSERT(child->m_Document != nullptr);

		if (child->m_Document)
			child->m_Document->_detachedFromTree(child);

		child->m_Document = nullptr;
		child->m_Depth = child->getParentNode() ? child->getParentNode()->m_Depth + 1 : 0;

		if (child->isContainerNode()) {
			Node* sub_child = static_cast<ContainerNode*>(child)->getFirstChild();
			while (sub_child) {
				DeatchFromTree(sub_child);
				sub_child = sub_child->getNextSibling();
			}
		}
	}

}
