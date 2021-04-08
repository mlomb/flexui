
#include "flexui/Style/StyleEngine.hpp"

#include "flexui/Style/StyleComputed.hpp"
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
		// TODO: add rules to index
	}

	void StyleEngine::removeStyleSheet(const std::weak_ptr<StyleSheet>& stylesheet)
	{
		auto it = std::find(m_StyleSheets.begin(), m_StyleSheets.end(), stylesheet);
		if (it != m_StyleSheets.end()) {
			// TODO: remove rules from index
			m_StyleSheets.erase(it);
		}
	}

	const std::vector<std::shared_ptr<StyleSheet>>& StyleEngine::getStyleSheets() const
	{
		return m_StyleSheets;
	}

	void StyleEngine::_attachedToTree(Node* node)
	{
		if (node->getNodeType() == NodeType::ELEMENT) {
			Element* e = static_cast<Element*>(node);
			//m_OrderedIDs.insert(e->getID().hash(), e->);
		}
		printf("NODE: %s\n", node->getDebugInfo().c_str());
	}

	void StyleEngine::_detachedFromTree(Node* node)
	{

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
}
