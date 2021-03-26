#include "flexui/Layout/LayoutObject.hpp"

#include "flexui/Layout/Yoga.hpp"
#include "flexui/Nodes/Node.hpp"

namespace flexui {

	LayoutObject::LayoutObject()
	{
		m_YogaNode = YGNodeNew();
		YGNodeSetContext(m_YogaNode, this);
	}

	LayoutObject::~LayoutObject()
	{
		YGNodeFree(m_YogaNode);
	}

	void LayoutObject::calculateLayout()
	{
		Rect layoutRect = {
			YGNodeLayoutGetLeft(m_YogaNode), YGNodeLayoutGetTop(m_YogaNode),
			YGNodeLayoutGetWidth(m_YogaNode), YGNodeLayoutGetHeight(m_YogaNode)
		};

		YGNodeRef parentYogaNode = YGNodeGetOwner(m_YogaNode);
		LayoutObject* parentObject = parentYogaNode ? static_cast<LayoutObject*>(YGNodeGetContext(parentYogaNode)) : nullptr;

		Rect newBounds;

		newBounds.position = parentObject ? parentObject->m_LayoutRect.position : Vec2{ 0, 0 };
		newBounds.position += layoutRect.position;
		newBounds.size = layoutRect.size;

		if (m_LayoutRect != layoutRect) {
			m_LayoutRect = newBounds;

			if (m_LayoutRect.position != newBounds.position) {
				// TODO: trigger position change
			}

			// TODO: trigger size change
		}

		bool hasNewLayout = YGNodeGetHasNewLayout(m_YogaNode);
		if (hasNewLayout || 1) {
			for (YGNodeRef childYogaNode : m_YogaNode->getChildren()) {
				LayoutObject* childObject = static_cast<LayoutObject*>(YGNodeGetContext(childYogaNode));
				childObject->calculateLayout();
			}

			YGNodeSetHasNewLayout(m_YogaNode, false);

			// TODO: trigger repaint
		}
	}
}
