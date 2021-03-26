#include "flexui/Nodes/Node.hpp"

#include "flexui/Log.hpp"
#include "flexui/Nodes/ContainerNode.hpp"

namespace flexui {

	Node::Node() :
		m_Document(nullptr),
		m_Parent(nullptr),
		m_PreviousSibling(nullptr),
		m_NextSibling(nullptr),
		m_Layout(nullptr)
	{
	}

	Node::~Node()
	{
		FUI_ASSERT_MSG(m_Parent == nullptr, "Node is still attached to the tree");
		delete m_Layout;
	}
}
