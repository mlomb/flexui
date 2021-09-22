#include "flexui/Nodes/Document.hpp"

#include "flexui/Layout/LayoutObject.hpp"
#include "flexui/Style/StyleEngine.hpp"
#include "flexui/Nodes/Element.hpp"
#include "flexui/Selectors/SelectorParser.hpp"
#include "flexui/Selectors/SelectorMatcher.hpp"

namespace flexui {

	Document::Document(ResourceProvider* resource_provider, TextureProvider* texture_provider) :
		m_ResourceProvider(resource_provider),
		m_TextureProvider(texture_provider),
		m_StyleEngine(this),
		m_LayoutEngine(this),
		m_RenderEngine(this),
		m_EventController(this)
	{
		m_Document = this;
		m_Layout = new LayoutObject();
	}

	Document::~Document()
	{
	}

	Element* Document::findElementsAt(Node* node, const Vec2& point, std::vector<Element*>* found)
	{
		if (node == nullptr || node->getLayoutObject() == nullptr)
			return nullptr;

		Element* element = node->getNodeType() == NodeType::ELEMENT ? static_cast<Element*>(node) : nullptr;

		const auto& bbox = node->getLayoutObject()->getLayoutRect();

		if (element && !element->isVisible())
			return nullptr; // Element is not visible

		if (bbox.contains(point)) {
			if (found && element)
				found->push_back(element);

			if (node->isContainerNode()) {
				ContainerNode* container_node = static_cast<ContainerNode*>(node);

				// iterate in reverse order (z-index)
				for (Node* child = container_node->getLastChild(); child; child = child->getPreviousSibling()) {
					Element* result = findElementsAt(child, point, found);
					if (result)
						return result;
				}
			}

			if(element)
				return element;
		}

		// no hit
		return nullptr;
	}

	Element* Document::querySelector(const String& selector)
	{
		// TODO: optimize
		auto v = querySelectorAll(selector);
		return v.size() > 0 ? v[0] : nullptr;
	}

	std::vector<Element*> Document::querySelectorAll(const String& selector)
	{
		Selector s;
		if (ParseSingleSelector(selector, s)) {
			SelectorIdentifier& identifier = s.back().identifier;
			const std::vector<Element*>& matching_last_identifier = m_ElementsIndex.findIdentifiers(identifier.type, identifier.value.hash());

			std::vector<Element*> matches;
			for (Element* e : matching_last_identifier) {
				if (MatchesSelector(e, s))
					matches.push_back(e);
			}
			return matches;
		}
		return std::vector<Element*>();
	}

	void Document::_attachedToTree(Node* node)
	{
		if (node->getNodeType() == NodeType::ELEMENT) {
			Element* e = static_cast<Element*>(node);

			// add existing identifiers to index
			_addElementID(e->getID(), e);
			_addElementTag(e->getTag(), e);
			for (const auto& klass : e->getClasses())
				_addElementClass(klass, e);
		}

		m_StyleEngine._attachedToTree(node);
		m_LayoutEngine._attachedToTree(node);
	}

	void Document::_detachedFromTree(Node* node)
	{
		if (node->getNodeType() == NodeType::ELEMENT) {
			Element* e = static_cast<Element*>(node);

			// remove identifiers from index
			_removeElementID(e->getID(), e);
			_removeElementTag(e->getTag(), e);
			for (auto& klass : e->getClasses()) {
				_removeElementClass(klass, e);
			}
		}

		m_StyleEngine._detachedFromTree(node);
		m_LayoutEngine._detachedFromTree(node);
	}

	void Document::_addElementID(const HashedString& id, Element* element) {   m_ElementsIndex.addIdentifier(SelectorIdentifierType::ID, id.hash(), element); }
	void Document::_addElementTag(const HashedString& tag, Element* element) { m_ElementsIndex.addIdentifier(SelectorIdentifierType::TAG, tag.hash(), element); }
	void Document::_addElementClass(const HashedString& klass, Element* element) { m_ElementsIndex.addIdentifier(SelectorIdentifierType::CLASS, klass.hash(), element); }

	void Document::_removeElementID(const HashedString& id, Element* element) { m_ElementsIndex.removeIdentifier(SelectorIdentifierType::ID, id.hash(), element); }
	void Document::_removeElementTag(const HashedString& tag, Element* element) { m_ElementsIndex.removeIdentifier(SelectorIdentifierType::TAG, tag.hash(), element); }
	void Document::_removeElementClass(const HashedString& klass, Element* element) { m_ElementsIndex.removeIdentifier(SelectorIdentifierType::CLASS, klass.hash(), element); }
}