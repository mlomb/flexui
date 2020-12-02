#pragma once

#include <vector>

#include "TreeProcessor.hpp"

#include "Style/StyleSelectorMatcher.hpp"

namespace flexui {

	class StyleSheet;
	struct StyleSelector;

	class StyleTreeUpdater : public TreeProcessor {
	public:
		StyleTreeUpdater(Surface* surface);
		virtual ~StyleTreeUpdater();

		void process() override;

	private:
		std::vector<StyleSheet*> m_SheetsStack;
		std::vector<SelectorMatch> m_MatchedSelectors;

		void applyStyles(Element* element);
		void processMatchedSelectors(Element* element);
		void syncWithLayout(Element* element);
	};

}
