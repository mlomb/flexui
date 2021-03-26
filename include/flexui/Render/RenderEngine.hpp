#pragma once

namespace flexui {

	class ContainerNode;
	class Document;
	class Painter;
	
	class RenderEngine {
	public:
		explicit RenderEngine(Document* document);
		~RenderEngine();

		void performRender(Painter* painter);

	private:
		Document* m_Document;

		void renderRecursive(Painter* painter, ContainerNode* node);
	};

}
