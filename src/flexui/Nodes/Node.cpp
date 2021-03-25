#include "flexui/Nodes/Node.hpp"

#include "flexui/Log.hpp"
#include "flexui/Nodes/ContainerNode.hpp"

namespace flexui {

	Node::Node() :
		m_Parent(nullptr),
		m_Previous(nullptr),
		m_Next(nullptr),
		m_YogaNode(nullptr)
	{
	}

	Node::~Node()
	{
		// TODO
	}

	void Node::appendChild(Node* child_node)
	{
		FUI_ASSERT_MSG(false, "This node type does not support appendChild");
	}
}
