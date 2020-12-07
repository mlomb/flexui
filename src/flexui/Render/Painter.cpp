#include "flexui/Render/Painter.hpp"

#include <math.h>
#include <cassert>
#include <algorithm>

#include <flexui/Render/Font.hpp>
#include <flexui/Render/DynamicAtlas.hpp>

namespace flexui {

	const float UI_PI = 3.14159265358979323846f;
	const float UI_EPS = 0.0001f;
	const int UI_SUBDIVISIONS = 6;

	inline bool almostEqual(const float a, const float b) {
		return fabs(a - b) < UI_EPS;
	}

	Painter::Painter(TextureProvider* textureProvider) :
		m_pVertex(NULL),
		m_pIndex(NULL),
		m_VertexCount(0),
		m_IndexCount(0)
	{
		m_Atlas = new DynamicAtlas(textureProvider);
		m_BaseVertex = (UIVertex*)malloc(50000 * sizeof(UIVertex));
		m_BaseIndex = (UIIndex*)malloc(50000 * sizeof(UIIndex));
		reset();
	}

	Painter::~Painter()
	{
		delete m_BaseVertex;
		delete m_BaseIndex;
	}

	void Painter::drawRectangle(const Rect& rect, const Color color)
	{
		if (rect.width < UI_EPS || rect.height < UI_EPS)
			return; // skip invalid rects
		if ((color & 0xFF000000) == 0)
			return; // skip transparent

		// 0     1
		// *-----*
		// | A  /|
		// |  /  |
		// |/  B |
		// *-----*
		// 2     3

		*m_pVertex = { rect.position,                           Vec2 { 0, 0 }, color, 0 }; m_pVertex++; // 0
		*m_pVertex = { rect.position + Vec2 { rect.width, 0  }, Vec2 { 0, 0 }, color, 0 }; m_pVertex++; // 1
		*m_pVertex = { rect.position + Vec2 { 0, rect.height }, Vec2 { 0, 0 }, color, 0 }; m_pVertex++; // 2
		*m_pVertex = { rect.position + rect.size,               Vec2 { 0, 0 }, color, 0 }; m_pVertex++; // 3

		*m_pIndex = m_VertexCount + 0; m_pIndex++;
		*m_pIndex = m_VertexCount + 2; m_pIndex++;
		*m_pIndex = m_VertexCount + 1; m_pIndex++;
		*m_pIndex = m_VertexCount + 3; m_pIndex++;
		*m_pIndex = m_VertexCount + 1; m_pIndex++;
		*m_pIndex = m_VertexCount + 2; m_pIndex++;

		m_IndexCount += 6;
		m_VertexCount += 4;
	}

	void Painter::drawRoundedCorner(const Vec2& center, const Vec2& radii, const Corner corner, const Color color)
	{
		if (radii.x < UI_EPS || radii.y < UI_EPS) {
			// No radius, nothing to do
			return;
		}

		const int subdivisions = UI_SUBDIVISIONS;
		const float stepAngle = 0.5f * UI_PI / (float)subdivisions; // quarter / subdivisions
		const float startAngle = 0.5f * UI_PI * static_cast<int>(corner);

		// Top left reference
		//          A
		//        * |
		//     *    |
		//   *      | radii.y
		//  *       |
		// |--------C
		//   radii.x
		// 
		// C = center
		// A = first in chain
		// * = a offset

		int centerIndex = m_VertexCount;
		*m_pVertex = { center, Vec2(0, 0), color, 0 }; m_pVertex++; // C
		m_VertexCount += 1;

		for (int i = 0; i <= subdivisions; i++) {
			float angle = startAngle + i * stepAngle;
			Vec2 offset = radii * Vec2 { cosf(angle), -sinf(angle) };

			*m_pVertex = { center + offset, { 0, 0 }, color, 0 }; m_pVertex++;
			m_VertexCount++;

			// first iteration is to create A
			if (i == 0)
				continue;

			*m_pIndex = m_VertexCount - 2; m_pIndex++;
			*m_pIndex = m_VertexCount - 1; m_pIndex++;
			*m_pIndex = centerIndex; m_pIndex++;
			m_IndexCount += 3;
		}
	}

	void Painter::drawRoundedCornerCarved(const Vec2& center, const Vec2& outerRadii, const Vec2& innerRadii, const Corner corner, const Color color)
	{
		if (innerRadii.x < UI_EPS || innerRadii.y < UI_EPS) {
			drawRoundedCorner(center, outerRadii, corner, color);
			return;
		}

		// See https://drafts.csswg.org/css-backgrounds-3/#corner-shaping

		// Top left reference
		//                         * outer.x
		//                    *    |
		//                *        | 
		//            *            |
		//         *               * inner.x
		//       *               *
		//     *               * 
		//   *               *
		//  * ------------- *      C (center)
		// outer.y       inner.y

		const int subdivisions = UI_SUBDIVISIONS;
		const float stepAngle = 0.5f * UI_PI / (float)subdivisions; // quarter / subdivisions
		const float startAngle = 0.5f * UI_PI * static_cast<int>(corner);

		for (int i = 0; i <= subdivisions; i++) {
			float angle = startAngle + i * stepAngle;

			Vec2 offset = { cosf(angle), -sinf(angle) };
			Vec2 out = outerRadii * offset;
			Vec2 in = innerRadii * offset;

			*m_pVertex = { center + out, { 0, 0 }, color, 0 }; m_pVertex++;
			*m_pVertex = { center + in, { 0, 0 }, color, 0 }; m_pVertex++;
			m_VertexCount += 2;

			// first iteration is to generate the initial two vertexs
			if (i == 0)
				continue;

			*m_pIndex = m_VertexCount - 2; m_pIndex++;
			*m_pIndex = m_VertexCount - 3; m_pIndex++;
			*m_pIndex = m_VertexCount - 4; m_pIndex++;
			*m_pIndex = m_VertexCount - 3; m_pIndex++;
			*m_pIndex = m_VertexCount - 2; m_pIndex++;
			*m_pIndex = m_VertexCount - 1; m_pIndex++;
			m_IndexCount += 6;
		}
	}

	void Painter::drawRoundedRectangle(const Rect& rect, const Color color, const RoundedRectParams& params)
	{
		const Vec2 halfSize = rect.size * 0.5f;
		const Vec2& topLeftRadii = Vec2::Min(params.cornerRadii[0], halfSize);
		const Vec2& topRightRadii = Vec2::Min(params.cornerRadii[1], halfSize);
		const Vec2& bottomRightRadii = Vec2::Min(params.cornerRadii[2], halfSize);
		const Vec2& bottomLeftRadii = Vec2::Min(params.cornerRadii[3], halfSize);

		const float maxLeftRadii = std::max(topLeftRadii.x, bottomLeftRadii.x);
		const float minLeftRadii = std::min(topLeftRadii.x, bottomLeftRadii.x);
		const float maxRightRadii = std::max(topRightRadii.x, bottomRightRadii.x);
		const float minRightRadii = std::min(topRightRadii.x, bottomRightRadii.x);

		// corner centers
		//    __----------------__
		//   * \    |        |     *
		//  *    \  |        |     / *
		// *   TL \ |        |  / TR  *
		// |_______\|← B  A →|/_______|
		// |        |        |        |
		// |        |        |        |
		// |        |        |        |
		// |        |        |        |
		// |________|← C  D →|________|
		// *       /|        |\       *
		//  * BL /  |        |  \BR  *
		//    * /   |        |   \ *
		//     *____|________|__ *
		const Vec2 topRightCorner = rect.position + Vec2(rect.size.x - topRightRadii.x, topRightRadii.y); // A
		const Vec2 topLeftCorner = rect.position + topLeftRadii; // B
		const Vec2 bottomLeftCorner = rect.position + Vec2(bottomLeftRadii.x, rect.size.y - bottomLeftRadii.y); // C
		const Vec2 bottomRightCorner = rect.position + rect.size - bottomRightRadii; // D

		drawRoundedCorner(topRightCorner, topRightRadii, Corner::TOP_RIGHT, color);
		drawRoundedCorner(topLeftCorner, topLeftRadii, Corner::TOP_LEFT, color);
		drawRoundedCorner(bottomLeftCorner, bottomLeftRadii, Corner::BOTTOM_LEFT, color);
		drawRoundedCorner(bottomRightCorner, bottomRightRadii, Corner::BOTTOM_RIGHT, color);

		// (1) both corners have the same radii.x
		//    __--------------
		//   * \    |        |
		//  *    \  |        |
		// *   TL \ |        |
		// |_______\|        |
		// |        |        |
		// |        |        |
		// |   A    | CENTER |
		// |        |        |
		// |________|        |
		// *       /|        |
		//  * BL /  |        |
		//    * /   |        |
		//     *____|________|

		// (2) top radii.x > bottom radii.x
		//    __--------------
		//   * \    |        |
		//  *    \  |        |
		// *   TL \ |        |
		// |_______\|        |
		// |    |   |        |
		// |    |   |        |
		// | A  |   | CENTER |
		// |    |   |        |
		// |____| B |        |
		// * BL/|   |        |
		//  */  |   |        |
		//    *_|___|________|

		// (3) bottom radii.x > top radii.x
		//    __--------------
		//   * |    |        |
		//  *  |    |        |
		// * TL|    |        |
		// |___|    |        |
		// |   |  B |        |
		// |   |    |        |
		// | A |    | CENTER |
		// |   |    |        |
		// |___|____|        |
		// *       /|        |
		//  * BL /  |        |
		//    * /   |        |
		//     *____|________|

		// center
		drawRectangle(Rect(rect.x + maxLeftRadii, rect.y, rect.width - maxLeftRadii - maxRightRadii, rect.height), color);

		// ---------- left side ----------

		// A (is the same for 1, 2, 3)
		drawRectangle(Rect(rect.x, rect.y + topLeftRadii.y, minLeftRadii, rect.height - topLeftRadii.y - bottomLeftRadii.y), color);

		if (almostEqual(topLeftRadii.x, bottomLeftRadii.x)) { } // (1)
		else if(topLeftRadii.x > bottomLeftRadii.x) { // (2)
			// B
			drawRectangle(Rect(rect.x + minLeftRadii, rect.y + topLeftRadii.y, maxLeftRadii - minLeftRadii, rect.height - topLeftRadii.y), color);
		}
		else { // (3)
			// B
			drawRectangle(Rect(rect.x + minLeftRadii, rect.y, maxLeftRadii - minLeftRadii, rect.height - bottomLeftRadii.y), color);
		}

		// ---------- right side ----------

		// A (is the same for 1, 2, 3)
		drawRectangle(Rect(rect.x + rect.width - minRightRadii, rect.y + topRightRadii.y, minRightRadii, rect.height - topRightRadii.y - bottomRightRadii.y), color);

		if (almostEqual(topRightRadii.x, bottomRightRadii.x)) {} // (1)
		else if (topRightRadii.x > bottomRightRadii.x) { // (2)
			// B
			drawRectangle(Rect(rect.x + rect.width - maxRightRadii, rect.y + topRightRadii.y, maxRightRadii - minRightRadii, rect.height - topRightRadii.y), color);
		}
		else { // (3)
			// B
			drawRectangle(Rect(rect.x + rect.width - maxRightRadii, rect.y, maxRightRadii - minRightRadii, rect.height - bottomRightRadii.y), color);
		}
	}

	void Painter::drawRoundedBorders(const Rect& rect, const Color color, const RoundedBordersParams& params)
	{
		//    __--------------__   
		//   *  |            |  *  
		//  *   |      A     |   * 
		// *____|____________|____*
		// |    |            |    |
		// | B  |            |  D |
		// |____|____________|____|
		// *    |            |    *
		//  *   |      C     |   * 
		//   *__|____________|__* 
		
		const Vec2 halfSize = rect.size * 0.5f;
		const Vec2& topLeftRadii     = Vec2::Min(params.rectParams.cornerRadii[0], halfSize);
		const Vec2& topRightRadii    = Vec2::Min(params.rectParams.cornerRadii[1], halfSize);
		const Vec2& bottomRightRadii = Vec2::Min(params.rectParams.cornerRadii[2], halfSize);
		const Vec2& bottomLeftRadii  = Vec2::Min(params.rectParams.cornerRadii[3], halfSize);

		const float leftWidth   = params.widths[0];
		const float topWidth    = params.widths[1];
		const float rightWidth  = params.widths[2];
		const float bottomWidth = params.widths[3];

		// A
		drawRectangle(Rect(rect.x + topLeftRadii.x, rect.y, rect.width - topLeftRadii.x - topRightRadii.x, topWidth), color);
		// C
		drawRectangle(Rect(rect.x + bottomLeftRadii.x, rect.y + rect.height - bottomWidth, rect.width - bottomLeftRadii.x - bottomRightRadii.x, bottomWidth), color);
		// B
		drawRectangle(Rect(rect.x, rect.y + topLeftRadii.y, leftWidth, rect.height - topLeftRadii.y - bottomLeftRadii.y), color);
		// D
		drawRectangle(Rect(rect.x + rect.width - rightWidth, rect.y + topRightRadii.y, rightWidth, rect.height - topRightRadii.y - bottomRightRadii.y), color);
		
		/*
			From https://drafts.csswg.org/css-backgrounds-3/#corner-shaping:

			The padding edge (inner border) radius is the outer border radius minus the corresponding border thickness.
			In the case where this results in a negative value, the inner radius is zero.

			outer edge = cornerRadii
			inner edge = cornerRadii - sidesBorderWidths

			if inner edge (x or y) < 0 then inner edge = 0,0
			(handled in drawRoundedCornerCarved, which fall back to drawRoundedCorner)
		*/

		const Vec2 topRightCorner = rect.position + Vec2(rect.size.x - topRightRadii.x, topRightRadii.y); // A
		const Vec2 topLeftCorner = rect.position + topLeftRadii; // B
		const Vec2 bottomLeftCorner = rect.position + Vec2(bottomLeftRadii.x, rect.size.y - bottomLeftRadii.y); // C
		const Vec2 bottomRightCorner = rect.position + rect.size - bottomRightRadii; // D

		const Vec2 topLeftInnerRadii = topLeftRadii - Vec2(leftWidth, topWidth);
		const Vec2 topRightInnerRadii = topRightRadii - Vec2(rightWidth, topWidth);
		const Vec2 bottomLeftInnerRadii = bottomLeftRadii - Vec2(leftWidth, bottomWidth);
		const Vec2 bottomRightInnerRadii = bottomRightRadii - Vec2(rightWidth, bottomWidth);

		drawRoundedCornerCarved(topLeftCorner, topLeftRadii, topLeftInnerRadii, Corner::TOP_LEFT, color);
		drawRoundedCornerCarved(topRightCorner, topRightRadii, topRightInnerRadii, Corner::TOP_RIGHT, color);
		drawRoundedCornerCarved(bottomLeftCorner, bottomLeftRadii, bottomLeftInnerRadii, Corner::BOTTOM_LEFT, color);
		drawRoundedCornerCarved(bottomRightCorner, bottomRightRadii, bottomRightInnerRadii, Corner::BOTTOM_RIGHT, color);

		// TODO: make rectangles where radius < border width
		//       i.e. bottomLeftInnerRadii.y (bottomRightRadii.y - bottomWidth) < 0 
		// *******************
		// |  |            |
		// |  |   CENTER   |
		// |LW|    ________|
		// |  |XXX|        |
		// |  |XXX|        |
		// |__|___|   BW   |
		//  *     |        |
		//   *    |        |
		//    *___|________|
	}

	void Painter::reset()
	{
		m_pVertex = m_BaseVertex;
		m_pIndex = m_BaseIndex;
		m_VertexCount = 0;
		m_IndexCount = 0;
	}

	void Painter::drawText(Font* font, const TextLayout& textLayout, const Vec2& position, const Color color)
	{
		if (textLayout.glyphs.size() == 0)
			return;

		// the texture is POT
		const float texelSize = 1.0f / (float)m_Atlas->getSize();
		const unsigned int fontId = font->getID();

		const Vec2 base_pos(position.x, position.y);
		Rect rect;

		for (const TextLayout::GlyphInstance& gi : textLayout.glyphs) {
			DynamicAtlas::Index idx = fontId * 0x10FFFF + gi.codepoint; // TODO: correct hashes

			if (gi.w == 0)
				continue; // a space, a tab

			if (!m_Atlas->tryGet(idx, rect)) {
				// new glyph
				unsigned char* data;
				unsigned int w, h;
				if (font->rasterizeGlyph(gi.codepoint, textLayout.style, data, w, h)) {
					bool success = m_Atlas->add(idx, w, h, data, w * h * 4, rect);
					free(data); // TODO: use smart pointers
					if (!success) {
						// std::cerr << "Can't add Glyph to atlas!" << std::endl;
						continue;
					}
				}
				else {
					// std::cerr << "Can't render Glyph!" << std::endl;
					continue;
				}
			}

			const Vec2 pos = base_pos + Vec2(gi.x, gi.y);
			const Vec2 size = Vec2(gi.w, gi.h);

			const float u0 = texelSize * rect.x;
			const float v0 = texelSize * rect.y;
			const float u1 = texelSize * (rect.x + rect.width);
			const float v1 = texelSize * (rect.y + rect.height);

			const float flags = gi.has_colors ? 2 : 1;

			*m_pVertex = { pos,                   Vec2(u0, v0), color, flags }; m_pVertex++;
			*m_pVertex = { pos + Vec2(0, size.y), Vec2(u0, v1), color, flags }; m_pVertex++;
			*m_pVertex = { pos + size,            Vec2(u1, v1), color, flags }; m_pVertex++;
			*m_pVertex = { pos + Vec2(size.x, 0), Vec2(u1, v0), color, flags }; m_pVertex++;

			*m_pIndex = m_VertexCount + 0; m_pIndex++;
			*m_pIndex = m_VertexCount + 1; m_pIndex++;
			*m_pIndex = m_VertexCount + 2; m_pIndex++;
			*m_pIndex = m_VertexCount + 2; m_pIndex++;
			*m_pIndex = m_VertexCount + 3; m_pIndex++;
			*m_pIndex = m_VertexCount + 0; m_pIndex++;

			m_IndexCount += 6;
			m_VertexCount += 4;
		}
	}
}
