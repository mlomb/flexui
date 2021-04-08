#pragma once

#include "flexui/Nodes/TreeScope.hpp"

namespace flexui {

	class Document;

	class LayoutEngine : public TreeScope {
	public:
		explicit LayoutEngine(Document* document);
		~LayoutEngine();

		// Performs the Layout pass
		void performLayout(float availableWidth, float availableHeight);

		void _attachedToTree(Node* node) override;
		void _detachedFromTree(Node* node) override;

	private:
		Document* m_Document;
	};

}
