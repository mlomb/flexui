
#include "flexui/Style/StyleEngine.hpp"

#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Nodes/Style.hpp"
#include "flexui/Nodes/Element.hpp"
#include "flexui/Nodes/Document.hpp"
#include "flexui/Layout/ElementLayoutObject.hpp"

#include "flexui/Log.hpp"

namespace flexui {

	StyleEngine::StyleEngine(Document* document)
		: m_Document(document)
	{
	}

	StyleEngine::~StyleEngine()
	{
	}

	void StyleEngine::performStyles()
	{
		calcStylesRecursive(m_Document);
	}

	void StyleEngine::addStyleSheet(const std::shared_ptr<StyleSheet>& stylesheet)
	{
		m_StyleSheets.push_back(stylesheet);
		for (const std::shared_ptr<StyleRule>& rule : stylesheet->getRules()) {
			for (const Selector& selector : rule->selectors) {
				if (selector.size() > 0) {
					const auto& last_part = selector.back();
					m_RulesIndex.addIdentifier(last_part.identifier.type, last_part.identifier.value.hash(), rule);
				}
			}
		}
	}

	void StyleEngine::removeStyleSheet(const std::shared_ptr<StyleSheet>& stylesheet)
	{
		auto it = std::find(m_StyleSheets.begin(), m_StyleSheets.end(), stylesheet);
		if (it != m_StyleSheets.end()) {
			m_StyleSheets.erase(it);
			// TODO: remove rules from index
		}
	}

	const std::vector<std::shared_ptr<StyleSheet>>& StyleEngine::getStyleSheets() const
	{
		return m_StyleSheets;
	}

	void StyleEngine::_attachedToTree(Node* node)
	{
		switch (node->getNodeType()) {
		case NodeType::STYLE:
		{
			Style* s = static_cast<Style*>(node);
			std::shared_ptr<StyleSheet> ss = s->getStyleSheet();
			if (ss)
				addStyleSheet(ss);
			break;
		}
		case NodeType::ELEMENT:
		{
			Element* e = static_cast<Element*>(node);
			// add existing identifiers to index
			_addElementID(e->getID(), e);
			m_ElementsIndex.addIdentifier(SelectorIdentifierType::TAG, e->getTag().hash(), e);
			for (auto& klass : e->getClasses()) {
				_addElementClass(klass, e);
			}
			break;
		}
		}
	}

	void StyleEngine::_detachedFromTree(Node* node)
	{
		switch (node->getNodeType()) {
		case NodeType::STYLE:
		{
			Style* s = static_cast<Style*>(node);
			std::shared_ptr<StyleSheet> ss = s->getStyleSheet();
			if (ss)
				removeStyleSheet(ss);
			break;
		}
		case NodeType::ELEMENT:
		{
			Element* e = static_cast<Element*>(node);
			// remove identifiers from index
			_removeElementID(e->getID(), e);
			m_ElementsIndex.removeIdentifier(SelectorIdentifierType::TAG, e->getTag().hash(), e);
			for (auto& klass : e->getClasses()) {
				_removeElementClass(klass, e);
			}
			break;
		}
		}
	}

	void StyleEngine::calcStylesRecursive(ContainerNode* node)
	{
		for (Node* child = node->getFirstChild(); child; child = child->getNextSibling()) {
			if (child->getNodeType() == NodeType::ELEMENT) {
				calcStyles(static_cast<Element*>(child));
				calcStylesRecursive(static_cast<ContainerNode*>(child));
			}
		}
	}

	void StyleEngine::calcStyles(Element* element)
	{
		/*
		m_MatchedSelectors.clear();
		StyleSelectorMatcher::FindMatches(element, m_StyleSheets, m_MatchedSelectors);

		// sorted by precedence
		std::sort(m_MatchedSelectors.begin(), m_MatchedSelectors.end());

		// TODO: calculate the hash of the matched selectors
		//       and reuse the StyleComputed*
		//       (apply inline later, use same pointer if no inline rules)
		/*StyleHash selectorsHash = 0;
		for (const SelectorMatch& match : m_MatchedSelectors) {
			match.
		}* /

		StyleComputed* computedStyle = new StyleComputed();
		*computedStyle = GetDefaultStyleValues(); // copy defaults
		for (const SelectorMatch& match : m_MatchedSelectors) {
			FUI_ASSERT(match.selector->rule);
			computedStyle->applyRule(*match.selector->rule);
		}

		// TODO: inline styles
		// computedStyle->applyRule(element->m_InlineRules);

		if (element->m_ComputedStyle)
			delete element->m_ComputedStyle;
		element->m_ComputedStyle = computedStyle;

		static_cast<ElementLayoutObject*>(element->getLayoutObject())->syncStyles();
		*/
	}

	Cursor StyleEngine::getCurrentCusor() const
	{
		Element* underCursor = m_Document->getEventsController().getElementOverMouse();
		return underCursor ? underCursor->m_ComputedStyle->cursor.value : Cursor::AUTO;
	}

	void StyleEngine::_addElementID(const HashedString& id, Element* element)
	{
		m_ElementsIndex.addIdentifier(SelectorIdentifierType::ID, id.hash(), element);
	}

	void StyleEngine::_addElementClass(const HashedString& klass, Element* element)
	{
		m_ElementsIndex.addIdentifier(SelectorIdentifierType::CLASS, klass.hash(), element);
	}

	void StyleEngine::_removeElementID(const HashedString& id, Element* element)
	{
		m_ElementsIndex.removeIdentifier(SelectorIdentifierType::ID, id.hash(), element);
	}

	void StyleEngine::_removeElementClass(const HashedString& klass, Element* element)
	{
		m_ElementsIndex.removeIdentifier(SelectorIdentifierType::CLASS, klass.hash(), element);
	}
}
