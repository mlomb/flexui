#include "flexui/Render/RenderEngine.hpp"

#include "flexui/Nodes/Document.hpp"
#include "flexui/Render/Painter.hpp"

namespace flexui {

	RenderEngine::RenderEngine(Document* document) :
		m_Document(document),
		m_Painter(nullptr)
	{
	}

	RenderEngine::~RenderEngine()
	{
	}

	void RenderEngine::performRender()
	{
		if (!m_Painter) {
			m_Painter = new Painter(m_Document->getTextureProvider());
		}
		m_Painter->reset();
		renderRecursive(m_Document);
	}

	void RenderEngine::renderRecursive(ContainerNode* node)
	{
		for (Node* child = node->getFirstChild(); child; child = child->getNextSibling()) {
			child->drawContent(*m_Painter);

			if (child->getNodeType() == NodeType::ELEMENT)
				renderRecursive(static_cast<ContainerNode*>(child));
		}
	}

}
