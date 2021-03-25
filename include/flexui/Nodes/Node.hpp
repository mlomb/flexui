#pragma once

#include <vector>
#include <memory>

// forward def to avoid include
struct YGNode;

namespace flexui {

	enum class NodeType {
		DOCUMENT,
		COMMENT,
		ELEMENT,
		TEXT,
	};

	// A Node is the base class for all nodes in the UI tree
	class Node {
	public:
		friend class TreeScope;

		Node* getParentNode() const { return m_Parent; }

		virtual void appendChild(Node* child_node);

		virtual ~Node();
		virtual NodeType getNodeType() const = 0;

		// Internal calls
		void _setParent(Node* parent) { m_Parent = parent; }
		void _setPreviousSibling(Node* node) { m_Previous = node; }
		void _setNextSibling(Node* node) { m_Next = node; }

	protected:
		Node();

		YGNode *m_YogaNode;

	private:
		Node *m_Parent;
		Node *m_Previous, *m_Next;
	};

}
