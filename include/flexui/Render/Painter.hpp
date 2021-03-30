#pragma once

#include <stdint.h>

#include "flexui/Math.hpp"
#include "flexui/Render/Font.hpp"
#include "flexui/Misc/Color.hpp"

namespace flexui {

	class TextureProvider;
	class DynamicAtlas;

	typedef uint16_t UIIndex;
	struct UIVertex {
		Vec2 pos;
		Vec2 uv;
		Color color; // AABBGGRR
		float flags;
	};

	enum class Corner {
		TOP_RIGHT = 0,
		TOP_LEFT = 1,
		BOTTOM_LEFT = 2,
		BOTTOM_RIGHT = 3
	};

	struct RoundedRectParams {
		Vec2 cornerRadii[4];
	};
	struct RoundedBordersParams {
		RoundedRectParams rectParams;
		float widths[4];
	};

	// TODO: move from immediate to retained eventually
	class Painter {
	public:
		Painter(TextureProvider* textureProvider);
		~Painter();

		void drawRectangle(const Rect& rect, const Color color);
		void drawRoundedCorner(const Vec2& center, const Vec2& radii, const Corner corner, const Color color);
		void drawRoundedCornerCarved(const Vec2& center, const Vec2& outerRadii, const Vec2& innerRadii, const Corner corner, const Color color);
		void drawRoundedRectangle(const Rect& rect, const Color color, const RoundedRectParams& params);
		void drawRoundedBorders(const Rect& rect, const Color color, const RoundedBordersParams& params);
		void drawText(Font* font, const TextLayout& textLayout, const Vec2& position, const Color color);

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

		DynamicAtlas* m_Atlas;

	private:
		friend class TreePainter;
	};

}
