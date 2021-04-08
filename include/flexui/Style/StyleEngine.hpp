#pragma once

#include <vector>

#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Style/StyleSheet.hpp"
#include "flexui/Nodes/TreeScope.hpp"

namespace flexui {

	class ContainerNode;
	class Document;
	class Element;
	class StyleSheet;
	
	class StyleEngine : public TreeScope {
	public:
		explicit StyleEngine(Document* document);
		~StyleEngine();

		void performStyles();

		void addStyleSheet(const std::shared_ptr<StyleSheet>& stylesheet);
		void removeStyleSheet(const std::weak_ptr<StyleSheet>& stylesheet);

		const std::vector<std::shared_ptr<StyleSheet>>& getStyleSheets() const;
		Cursor getCurrentCusor() const;

		void _attachedToTree(Node* node) override;
		void _detachedFromTree(Node* node) override;

	private:
		Document* m_Document;

		void calcStylesRecursive(ContainerNode* parent);
		void calcStyles(Element* element);

		std::vector<std::shared_ptr<StyleSheet>> m_StyleSheets;
		//std::vector<SelectorMatch> m_MatchedSelectors;

		typedef std::unordered_map<uint32_t, std::vector<std::shared_ptr<StyleRule>>> IdentifierLookupTable;

		IdentifierLookupTable m_OrderedIDs, m_OrderedTags, m_OrderedClasses;
	};

}
