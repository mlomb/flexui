#pragma once

#include <unordered_map>
#include <vector>

#include "StyleDefinitions.hpp"

namespace flexui {

	class StyleSheet {
	public:
		StyleSheet();
		~StyleSheet();

		int addRule(const StyleRule& rule);
		void addSelector(const StyleSelector& selector, int rule_ref);

	private:
		std::vector<StyleSelector*> m_Selectors;
		std::vector<StyleRule*> m_Rules;

		friend class StyleSelectorMatcher;

		typedef std::unordered_map<StyleHash, std::vector<StyleSelector*>> IdentifierLookupTable;

		IdentifierLookupTable m_OrderedIDs, m_OrderedTags, m_OrderedClasses;
	};

}
