#include "flexui/Layout/LayoutEngine.hpp"

#include "flexui/Nodes/Document.hpp"
#include "flexui/Layout/LayoutObject.hpp"
#include "flexui/Layout/Yoga.hpp"

namespace flexui {

	LayoutEngine::LayoutEngine(Document* document)
		: m_Document(document)
	{
	}

	LayoutEngine::~LayoutEngine()
	{
	}

	void LayoutEngine::performLayout(float availableWidth, float availableHeight)
	{
		YGNodeRef yogaNode = m_Document->getLayoutObject()->getYogaNode();

		YGNodeStyleSetWidth(yogaNode, availableWidth);
		YGNodeStyleSetHeight(yogaNode, availableHeight);

		if (YGNodeIsDirty(yogaNode)) {
			YGNodeCalculateLayout(yogaNode, availableWidth, availableHeight, YGDirectionLTR);
			m_Document->getLayoutObject()->calculateLayout();
		}
	}

	void LayoutEngine::_attachedToTree(Node* node)
	{

		// keep tree in sync (TODO: find a better way to do this)
		if (node->getLayoutObject()) {
			// TODO: !
			if (YGNode* yogaNode = node->getParentNode()->getLayoutObject()->getYogaNode()) {
				YGNodeInsertChild(yogaNode, node->getLayoutObject()->getYogaNode(), YGNodeGetChildCount(yogaNode));
			}
		}
	}

	void LayoutEngine::_detachedFromTree(Node* node)
	{

	}

}