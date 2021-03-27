#pragma once

#include <vector>

namespace flexui {

	class Element;
	class StyleSheet;
	struct StyleSelector;

	struct SelectorMatch {
		StyleSheet* sheet;
		StyleSelector* selector;
		int stylesheetIndex;

		bool operator<(const SelectorMatch& b) const;
	};

	class StyleSelectorMatcher {
	public:

		/// May insert (very few) duplicates
		static void FindMatches(Element* element, std::vector<StyleSheet*>& sheets, std::vector<SelectorMatch>& matches);

	private:
		StyleSelectorMatcher(); // don't instance
	};

}
