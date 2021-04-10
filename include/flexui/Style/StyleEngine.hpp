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

		const std::vector<std::shared_ptr<StyleSheet>>& getStyleSheets() const;
		Cursor getCurrentCusor() const;

		void _attachedToTree(Node* node) override;
		void _detachedFromTree(Node* node) override;

		void _addElementID(const HashedString& id, Element* element);
		void _addElementClass(const HashedString& klass, Element* element);
		void _removeElementID(const HashedString& id, Element* element);
		void _removeElementClass(const HashedString& klass, Element* element);

	private:
		Document* m_Document;

		void calcStylesRecursive(ContainerNode* parent);
		void calcStyles(Element* element);

		std::vector<std::shared_ptr<StyleSheet>> m_StyleSheets;

		IdentifierIndex<std::shared_ptr<StyleRule>> m_RulesIndex;
		IdentifierIndex<Element*> m_ElementsIndex;
	};

}
