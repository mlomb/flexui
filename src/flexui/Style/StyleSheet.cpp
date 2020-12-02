#include "Style/StyleSheet.hpp"

#include <cassert>

namespace flexui {

	StyleSheet::StyleSheet()
	{
	}

	StyleSheet::~StyleSheet()
	{
		for (StyleRule* rule : m_Rules)
			delete rule;
		for (StyleSelector* selector : m_Selectors)
			delete selector;
	}

	int StyleSheet::addRule(const StyleRule& rule)
	{
		StyleRule* new_rule = new StyleRule();
		*new_rule = rule; // copy
		m_Rules.emplace_back(new_rule);
		return (int)m_Rules.size() - 1;
	}

	void StyleSheet::addSelector(const StyleSelector& selector, int rule_ref)
	{
		assert(selector.parts.size() > 0);

		StyleSelector* new_selector = new StyleSelector();
		*new_selector = selector; // copy
		new_selector->rule = m_Rules[rule_ref];
		new_selector->order = (int)m_Rules.size();
		m_Selectors.emplace_back(new_selector);

		const StyleIdentifier& lastIdentifier = new_selector->parts.back().identifier;

		switch (lastIdentifier.type) {
		case StyleIdentifierType::ID:    m_OrderedIDs[lastIdentifier.text_hash].emplace_back(new_selector); break;
		case StyleIdentifierType::TAG:   m_OrderedTags[lastIdentifier.text_hash].emplace_back(new_selector); break;
		case StyleIdentifierType::CLASS: m_OrderedClasses[lastIdentifier.text_hash].emplace_back(new_selector); break;
		default:
			assert(false);
			break;
		}
	}

}
