#pragma once

#include <vector>
#include <string>
#include <memory>

#include "flexui/Math.hpp"
#include "flexui/Events/EventTarget.hpp"

namespace flexui {

	class ContainerNode;
	class Element;
	class Document;
	class LayoutObject;
	class Painter;

	enum class NodeType {
		COMMENT,
		STYLESHEET, // TODO: !
		DOCUMENT,
		ELEMENT,
		TEXT
	};

	// A Node is the base class for all nodes in the UI tree
	class Node : public EventTarget {
	public:

		virtual ~Node();
		virtual NodeType getNodeType() const = 0;
		virtual std::string getNodeName() const = 0;
		virtual std::string getDebugInfo() const { return getNodeName(); };
		virtual void drawContent(Painter& painter) { };

		ContainerNode* getParentNode() const { return m_Parent; }
		Node* getPreviousSibling() const { return m_PreviousSibling; }
		Node* getNextSibling() const { return m_NextSibling; }
		Document* getDocument() const { return m_Document; }
		LayoutObject* getLayoutObject() const { return m_Layout; }
		bool isContainerNode() const { return getNodeType() == NodeType::DOCUMENT || getNodeType() == NodeType::ELEMENT; }
		int getDepth() const { return m_Depth; }
		Element* getParentElement() const;

	protected:
		Node();

		LayoutObject* m_Layout;

	private:
		friend class Document;
		friend class ContainerNode;

		ContainerNode* m_Parent;
		Node *m_PreviousSibling, *m_NextSibling;

		int m_Depth;
		Document* m_Document;
	};

}
