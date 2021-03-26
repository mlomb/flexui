#include "flexui/Render/RenderEngine.hpp"

#include "flexui/Nodes/Document.hpp"
#include "flexui/Render/Painter.hpp"

namespace flexui {

	RenderEngine::RenderEngine(Document* document)
		: m_Document(document)
	{
	}

	RenderEngine::~RenderEngine()
	{
	}

	void RenderEngine::performRender(Painter* painter)
	{
		painter->reset();
		renderRecursive(painter, m_Document);
	}

	void RenderEngine::renderRecursive(Painter* painter, ContainerNode* node)
	{
		for (Node* child = node->getFirstChild(); child; child = child->getNextSibling()) {
			child->drawContent(painter);

			if (child->getNodeType() == NodeType::ELEMENT)
				renderRecursive(painter, static_cast<ContainerNode*>(child));
		}
	}

}
