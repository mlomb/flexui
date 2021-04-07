#pragma once

#include <vector>

#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Style/StyleSheet.hpp" // StyleRule

namespace flexui {

	class ContainerNode;
	class Document;
	class Element;
	class StyleSheet;
	
	class StyleEngine {
	public:
		explicit StyleEngine(Document* document);
		~StyleEngine();

		void performStyles();

		void addStyleSheet(StyleSheet* stylesheet);

		const std::vector<StyleSheet*>& getStyleSheets() const;		
		Cursor getCurrentCusor() const;

	private:
		Document* m_Document;

		void calcStylesRecursive(ContainerNode* parent);
		void calcStyles(Element* element);

		std::vector<StyleSheet*> m_StyleSheets;
		//std::vector<SelectorMatch> m_MatchedSelectors;

		typedef std::unordered_map<uint32_t, std::vector<std::shared_ptr<StyleRule>>> IdentifierLookupTable;

		IdentifierLookupTable m_OrderedIDs, m_OrderedTags, m_OrderedClasses;
	};

}
