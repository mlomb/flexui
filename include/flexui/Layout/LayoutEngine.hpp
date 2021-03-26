#pragma once

namespace flexui {

	class Document;

	class LayoutEngine {
	public:
		explicit LayoutEngine(Document* document);
		~LayoutEngine();

		// Performs the Layout pass
		void performLayout(float availableWidth, float availableHeight);

	private:
		Document* m_Document;
	};

}
