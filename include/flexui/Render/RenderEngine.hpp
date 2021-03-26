#pragma once

namespace flexui {

	class ContainerNode;
	class Document;
	class Painter;
	
	class RenderEngine {
	public:
		explicit RenderEngine(Document* document);
		~RenderEngine();

		const Painter* getPainter() const { return m_Painter; }

		void performRender();

	private:
		Document* m_Document;
		Painter* m_Painter;

		void renderRecursive(ContainerNode* node);
	};

}
