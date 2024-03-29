#pragma once

#include <vector>

#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Style/StyleSheet.hpp"
#include "flexui/Nodes/TreeScope.hpp"
#include "flexui/Selectors/IdentifierIndex.hpp"

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
		void removeStyleSheet(const std::shared_ptr<StyleSheet>& stylesheet);

		Cursor getCurrentCusor() const;

		// internal calls
		void _attachedToTree(Node* node) override;
		void _detachedFromTree(Node* node) override;

	private:

		void computeStyleRecursive(ContainerNode* parent);
		void computeStyle(Element* element);

		Document* m_Document;

		std::vector<std::shared_ptr<StyleSheet>> m_StyleSheets;
		IdentifierIndex<std::shared_ptr<StyleRule>> m_RulesIndex;
		std::vector<std::shared_ptr<StyleRule>> m_MatchedSelectors;
	};

}
