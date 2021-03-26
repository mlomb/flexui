#pragma once

#include "flexui/Math.hpp"

// forward def to avoid include
struct YGNode;

namespace flexui {

	// A LayoutObject holds layout information of a Node
	// This class is owned by its corresponding Node
	class LayoutObject {
	public:
		LayoutObject();
		
		Rect getLayoutRect() const { return m_LayoutRect; };
		YGNode* getYogaNode() const { return m_YogaNode; }

	protected:
		friend class LayoutEngine;

		virtual ~LayoutObject();

		YGNode* m_YogaNode;

		void calculateLayout();

	private:
		Rect m_LayoutRect;
	};

}
