#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	// A ContainerNode allows nodes to hold other nodes
	class ContainerNode : public Node {
	public:

		void appendChild(Node* child_node);
		void removeChild(Node* child_node);

		Node* getFirstChild() const { return m_FirstChild; }
		Node* getLastChild() const { return m_LastChild; }

	protected:
		ContainerNode();
		~ContainerNode() override;

	private:
		Node* m_FirstChild;
		Node* m_LastChild;

		static void AttachToTree(Node* child);
		static void DeatchFromTree(Node* child);

	};

}
