#pragma once

#include <vector>
#include <memory>

#include "flexui/Math.hpp"

namespace flexui {

	class Document;
	class LayoutObject;
	class Painter;

	enum class NodeType {
		COMMENT,
		DOCUMENT,
		ELEMENT,
		TEXT
	};

	// A Node is the base class for all nodes in the UI tree
	class Node {
	public:

		virtual ~Node();
		virtual NodeType getNodeType() const = 0;
		virtual void drawContent(Painter& painter) { };

		Node* getParentNode() const { return m_Parent; }
		Node* getPreviousSibling() const { return m_PreviousSibling; }
		Node* getNextSibling() const { return m_NextSibling; }
		Document* getDocument() const { return m_Document; }
		LayoutObject* getLayoutObject() const { return m_Layout; }
		bool isContainerNode() const { return getNodeType() == NodeType::DOCUMENT || getNodeType() == NodeType::ELEMENT; }

	protected:
		Node();

		Document* m_Document;
		LayoutObject* m_Layout;

	private:
		friend class ContainerNode;
		Node *m_Parent;
		Node *m_PreviousSibling, *m_NextSibling;
	};

}
