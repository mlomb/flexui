#pragma once

#include <vector>

#include "flexui/Style/StyleSelectorMatcher.hpp"

namespace flexui {

	class ContainerNode;
	class Document;
	class StyleSheet;
	
	class StyleEngine {
	public:
		explicit StyleEngine(Document* document);
		~StyleEngine();

		void performStyles();

		void addStyleSheet(StyleSheet* stylesheet);

		const std::vector<StyleSheet*>& getStyleSheets() const;

	private:
		Document* m_Document;

		void calcStylesRecursive(ContainerNode* parent);
		void calcStyles(Element* element);

		std::vector<StyleSheet*> m_StyleSheets;
		std::vector<SelectorMatch> m_MatchedSelectors;
	};

}
