#pragma once

#include <stdint.h>

#include "flexui/Math.hpp"

namespace flexui {

	typedef uint16_t UIIndex;
	struct UIVertex {
		UIVec2 pos;
		UIVec2 uv;
		UIColor color; // AABBGGRR
		float flags;
	};

	enum class Corner {
		TOP_RIGHT = 0,
		TOP_LEFT = 1,
		BOTTOM_LEFT = 2,
		BOTTOM_RIGHT = 3
	};

	struct RoundedRectParams {
		UIVec2 cornerRadii[4];
	};
	struct RoundedBordersParams {
		RoundedRectParams rectParams;
		float widths[4];
	};

	// TODO: move from immediate to retained eventually
	class Painter {
	public:
		Painter();
		~Painter();

		void drawRectangle(const UIRect& rect, const UIColor color);
		void drawRoundedCorner(const UIVec2& center, const UIVec2& radii, const Corner corner, const UIColor color);
		void drawRoundedCornerCarved(const UIVec2& center, const UIVec2& outerRadii, const UIVec2& innerRadii, const Corner corner, const UIColor color);
		void drawRoundedRectangle(const UIRect& rect, const UIColor color, const RoundedRectParams& params);
		void drawRoundedBorders(const UIRect& rect, const UIColor color, const RoundedBordersParams& params);

		// void drawText(const Graphics::TextLayout& textLayout, const UIVec2& position, const UIColor color);

		// TODO: all of these must be removed
		void reset();
		UIVertex* getVertexPtr() const { return m_BaseVertex; };
		UIIndex* getIndexPtr() const { return m_BaseIndex; };
		unsigned int getVertexCount() const { return m_VertexCount; };
		unsigned int getIndexCount() const { return m_IndexCount; };

	private:
		UIVertex* m_pVertex;
		UIIndex* m_pIndex;
		unsigned int m_IndexCount;
		unsigned int m_VertexCount;

		UIVertex* m_BaseVertex;
		UIIndex* m_BaseIndex;

	private:
		friend class TreePainter;
	};

}
