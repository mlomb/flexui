#pragma once

namespace flexui {

	class Document;

	// TreeScope mantain information about the underlying tree
	class TreeScope {
	public:
		TreeScope(Document* document);
		~TreeScope();

	private:
		//Document* m_Document;
	};

}
