#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	// Responsible for hierarchy
	class ContainerNode : public Node {
	public:

		void appendChild(Node* child_node) override;

		// Internal calls
		void _setFirstChild(Node* node) { m_FirstChild = node; }
		void _setLastChild(Node* node) { m_LastChild = node; }

	protected:
		ContainerNode();


	private:
		Node* m_FirstChild;
		Node* m_LastChild;
	};

}
