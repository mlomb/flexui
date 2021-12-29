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
		computeStyleRecursive(m_Document);
	}

	void StyleEngine::addStyleSheet(const std::shared_ptr<StyleSheet>& stylesheet)
	{
		m_StyleSheets.push_back(stylesheet);
		// add rules from index
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
			// remove rules from index
			for (const std::shared_ptr<StyleRule>& rule : stylesheet->getRules()) {
				for (const Selector& selector : rule->selectors) {
					if (selector.size() > 0) {
						const auto& last_part = selector.back();
						m_RulesIndex.removeIdentifier(last_part.identifier.type, last_part.identifier.value.hash(), rule);
					}
				}
			}
			m_StyleSheets.erase(it);
		}
	}

	void StyleEngine::_attachedToTree(Node* node)
	{
		if (node->getNodeType() == NodeType::STYLE) {
			Style* s = static_cast<Style*>(node);

			std::shared_ptr<StyleSheet> ss = s->getStyleSheet();
			if (ss)
				addStyleSheet(ss);
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
		}
	}

	void StyleEngine::computeStyleRecursive(ContainerNode* node)
	{
		for (Node* child = node->getFirstChild(); child; child = child->getNextSibling()) {
			if (child->getNodeType() == NodeType::ELEMENT)
				computeStyle(static_cast<Element*>(child));
			if (child->isContainerNode())
				computeStyleRecursive(static_cast<ContainerNode*>(child));
		}
	}

	void StyleEngine::computeStyle(Element* element)
	{
		m_MatchedSelectors.clear();
		m_RulesIndex.findMatches(element, m_MatchedSelectors);

		// sorted by precedence (broken)
		//std::sort(m_MatchedSelectors.begin(), m_MatchedSelectors.end());

		/*
		m_MatchedSelectors.clear();
		StyleSelectorMatcher::FindMatches(element, m_StyleSheets, m_MatchedSelectors);

		// TODO: calculate the hash of the matched selectors
		//       and reuse the StyleComputed*
		//       (apply inline later, use same pointer if no inline rules)
		/*StyleHash selectorsHash = 0;
		for (const SelectorMatch& match : m_MatchedSelectors) {
			match.
		}*/

		StyleComputed* computedStyle = new StyleComputed();
		*computedStyle = GetDefaultStyleValues(); // copy defaults
		for (const auto& match : m_MatchedSelectors) {
			for (const auto& sel : match->selectors) {
				if (MatchesSelector(element, sel)) {
					computedStyle->applyRule(*match);
					break;
				}
			}
		}

		// TODO: inline styles
		// computedStyle->applyRule(element->m_InlineRules);

		if (element->m_ComputedStyle)
			delete element->m_ComputedStyle;
		element->m_ComputedStyle = computedStyle;

		static_cast<ElementLayoutObject*>(element->getLayoutObject())->syncStyles();
	}

	Cursor StyleEngine::getCurrentCusor() const
	{
		Element* underCursor = m_Document->getEventsController().getElementOverMouse();
		return underCursor ? underCursor->m_ComputedStyle->cursor.value : Cursor::AUTO;
	}

}
