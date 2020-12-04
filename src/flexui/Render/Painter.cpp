#include "flexui/Render/Painter.hpp"

#include <math.h>
#include <cassert>
#include <algorithm>

namespace flexui {

	const float UI_PI = 3.14159265358979323846f;
	const float UI_EPS = 0.0001f;
	const int UI_SUBDIVISIONS = 6;

	inline bool almostEqual(const float a, const float b) {
		return fabs(a - b) < UI_EPS;
	}

	Painter::Painter() :
		m_pVertex(NULL),
		m_pIndex(NULL),
		m_VertexCount(0),
		m_IndexCount(0)
	{
		m_BaseVertex = (UIVertex*)malloc(5000 * sizeof(UIVertex));
		m_BaseIndex = (UIIndex*)malloc(5000 * sizeof(UIIndex));
		reset();
	}

	Painter::~Painter()
	{
		delete m_BaseVertex;
		delete m_BaseIndex;
	}

	void Painter::drawRectangle(const UIRect& rect, const UIColor color)
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

		*m_pVertex = { rect.position,                             UIVec2 { 0, 0 }, color, 0 }; m_pVertex++; // 0
		*m_pVertex = { rect.position + UIVec2 { rect.width, 0  }, UIVec2 { 0, 0 }, color, 0 }; m_pVertex++; // 1
		*m_pVertex = { rect.position + UIVec2 { 0, rect.height }, UIVec2 { 0, 0 }, color, 0 }; m_pVertex++; // 2
		*m_pVertex = { rect.position + rect.size,                 UIVec2 { 0, 0 }, color, 0 }; m_pVertex++; // 3

		*m_pIndex = m_VertexCount + 0; m_pIndex++;
		*m_pIndex = m_VertexCount + 2; m_pIndex++;
		*m_pIndex = m_VertexCount + 1; m_pIndex++;
		*m_pIndex = m_VertexCount + 3; m_pIndex++;
		*m_pIndex = m_VertexCount + 1; m_pIndex++;
		*m_pIndex = m_VertexCount + 2; m_pIndex++;

		m_IndexCount += 6;
		m_VertexCount += 4;
	}

	void Painter::drawRoundedCorner(const UIVec2& center, const UIVec2& radii, const Corner corner, const UIColor color)
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
		*m_pVertex = { center, UIVec2(0, 0), color, 0 }; m_pVertex++; // C
		m_VertexCount += 1;

		for (int i = 0; i <= subdivisions; i++) {
			float angle = startAngle + i * stepAngle;
			UIVec2 offset = radii * UIVec2 { cosf(angle), -sinf(angle) };

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

	void Painter::drawRoundedCornerCarved(const UIVec2& center, const UIVec2& outerRadii, const UIVec2& innerRadii, const Corner corner, const UIColor color)
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

			UIVec2 offset = { cosf(angle), -sinf(angle) };
			UIVec2 out = outerRadii * offset;
			UIVec2 in = innerRadii * offset;

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

	void Painter::drawRoundedRectangle(const UIRect& rect, const UIColor color, const RoundedRectParams& params)
	{
		const UIVec2 halfSize = rect.size * 0.5f;
		const UIVec2& topLeftRadii = UIVec2::Min(params.cornerRadii[0], halfSize);
		const UIVec2& topRightRadii = UIVec2::Min(params.cornerRadii[1], halfSize);
		const UIVec2& bottomRightRadii = UIVec2::Min(params.cornerRadii[2], halfSize);
		const UIVec2& bottomLeftRadii = UIVec2::Min(params.cornerRadii[3], halfSize);

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
		const UIVec2 topRightCorner = rect.position + UIVec2(rect.size.x - topRightRadii.x, topRightRadii.y); // A
		const UIVec2 topLeftCorner = rect.position + topLeftRadii; // B
		const UIVec2 bottomLeftCorner = rect.position + UIVec2(bottomLeftRadii.x, rect.size.y - bottomLeftRadii.y); // C
		const UIVec2 bottomRightCorner = rect.position + rect.size - bottomRightRadii; // D

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
		drawRectangle(UIRect(rect.x + maxLeftRadii, rect.y, rect.width - maxLeftRadii - maxRightRadii, rect.height), color);

		// ---------- left side ----------

		// A (is the same for 1, 2, 3)
		drawRectangle(UIRect(rect.x, rect.y + topLeftRadii.y, minLeftRadii, rect.height - topLeftRadii.y - bottomLeftRadii.y), color);

		if (almostEqual(topLeftRadii.x, bottomLeftRadii.x)) { } // (1)
		else if(topLeftRadii.x > bottomLeftRadii.x) { // (2)
			// B
			drawRectangle(UIRect(rect.x + minLeftRadii, rect.y + topLeftRadii.y, maxLeftRadii - minLeftRadii, rect.height - topLeftRadii.y), color);
		}
		else { // (3)
			// B
			drawRectangle(UIRect(rect.x + minLeftRadii, rect.y, maxLeftRadii - minLeftRadii, rect.height - bottomLeftRadii.y), color);
		}

		// ---------- right side ----------

		// A (is the same for 1, 2, 3)
		drawRectangle(UIRect(rect.x + rect.width - minRightRadii, rect.y + topRightRadii.y, minRightRadii, rect.height - topRightRadii.y - bottomRightRadii.y), color);

		if (almostEqual(topRightRadii.x, bottomRightRadii.x)) {} // (1)
		else if (topRightRadii.x > bottomRightRadii.x) { // (2)
			// B
			drawRectangle(UIRect(rect.x + rect.width - maxRightRadii, rect.y + topRightRadii.y, maxRightRadii - minRightRadii, rect.height - topRightRadii.y), color);
		}
		else { // (3)
			// B
			drawRectangle(UIRect(rect.x + rect.width - maxRightRadii, rect.y, maxRightRadii - minRightRadii, rect.height - bottomRightRadii.y), color);
		}
	}

	void Painter::drawRoundedBorders(const UIRect& rect, const UIColor color, const RoundedBordersParams& params)
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
		
		const UIVec2 halfSize = rect.size * 0.5f;
		const UIVec2& topLeftRadii     = UIVec2::Min(params.rectParams.cornerRadii[0], halfSize);
		const UIVec2& topRightRadii    = UIVec2::Min(params.rectParams.cornerRadii[1], halfSize);
		const UIVec2& bottomRightRadii = UIVec2::Min(params.rectParams.cornerRadii[2], halfSize);
		const UIVec2& bottomLeftRadii  = UIVec2::Min(params.rectParams.cornerRadii[3], halfSize);

		const float leftWidth   = params.widths[0];
		const float topWidth    = params.widths[1];
		const float rightWidth  = params.widths[2];
		const float bottomWidth = params.widths[3];

		// A
		drawRectangle(UIRect(rect.x + topLeftRadii.x, rect.y, rect.width - topLeftRadii.x - topRightRadii.x, topWidth), color);
		// C
		drawRectangle(UIRect(rect.x + bottomLeftRadii.x, rect.y + rect.height - bottomWidth, rect.width - bottomLeftRadii.x - bottomRightRadii.x, bottomWidth), color);
		// B
		drawRectangle(UIRect(rect.x, rect.y + topLeftRadii.y, leftWidth, rect.height - topLeftRadii.y - bottomLeftRadii.y), color);
		// D
		drawRectangle(UIRect(rect.x + rect.width - rightWidth, rect.y + topRightRadii.y, rightWidth, rect.height - topRightRadii.y - bottomRightRadii.y), color);
		
		/*
			From https://drafts.csswg.org/css-backgrounds-3/#corner-shaping:

			The padding edge (inner border) radius is the outer border radius minus the corresponding border thickness.
			In the case where this results in a negative value, the inner radius is zero.

			outer edge = cornerRadii
			inner edge = cornerRadii - sidesBorderWidths

			if inner edge (x or y) < 0 then inner edge = 0,0
			(handled in drawRoundedCornerCarved, which fall back to drawRoundedCorner)
		*/

		const UIVec2 topRightCorner = rect.position + UIVec2(rect.size.x - topRightRadii.x, topRightRadii.y); // A
		const UIVec2 topLeftCorner = rect.position + topLeftRadii; // B
		const UIVec2 bottomLeftCorner = rect.position + UIVec2(bottomLeftRadii.x, rect.size.y - bottomLeftRadii.y); // C
		const UIVec2 bottomRightCorner = rect.position + rect.size - bottomRightRadii; // D

		const UIVec2 topLeftInnerRadii = topLeftRadii - UIVec2(leftWidth, topWidth);
		const UIVec2 topRightInnerRadii = topRightRadii - UIVec2(rightWidth, topWidth);
		const UIVec2 bottomLeftInnerRadii = bottomLeftRadii - UIVec2(leftWidth, bottomWidth);
		const UIVec2 bottomRightInnerRadii = bottomRightRadii - UIVec2(rightWidth, bottomWidth);

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

	/*
	void Painter::drawText(const Graphics::TextLayout& textLayout, const UIVec2& position, const UIColor color)
	{
		using namespace OrbitEngine::Graphics;

		if (textLayout.glyphs.size() == 0 || !textLayout.font)
			return;

		// the texture is POT
		const float texelSize = 1.0f / (float)m_Atlas->getSize();

		BitmapRGBA bitmap;
		const UIVec2 base_pos(position.x, position.y);
		Math::Recti rect;

		for (const TextLayout::GlyphInstance& gi : textLayout.glyphs) {
			DynamicAtlas::Index idx = textLayout.settings.size + gi.codepoint * 1000; // TODO: include font as hash

			if (!m_Atlas->tryGet(idx, rect)) {
				// new glyph
				GlyphMetrics metrics;
				if (!textLayout.font->getGlyph(gi.codepoint, textLayout.settings.size, GlyphRenderMode::GRAY, bitmap, metrics)) {
					OE_LOG_DEBUG("Can't get Glyph!");
					continue;
				}

				if (bitmap.valid()) {
					Texture* glyph_tex = bitmap.toTexture();
					if (!m_Atlas->add(idx, rect, glyph_tex)) {
						OE_ASSERT_MSG(false, "Can't add Glyph to atlas!");
						continue;
					}
					m_DestroyAfterCommitingAtlas.push_back(glyph_tex);
				}
				else {
					OE_ASSERT(false);
				}
			}

			const UIVec2 pos = base_pos + UIVec2(gi.rect.x, gi.rect.y);
			const UIVec2 size = UIVec2(gi.rect.width, gi.rect.height);

			float u0 = texelSize * rect.x;
			float v0 = texelSize * rect.y;
			float u1 = texelSize * (rect.x + rect.width);
			float v1 = texelSize * (rect.y + rect.height);

			v0 = 1.0f - v0;
			v1 = 1.0f - v1;

			*m_pVertex = { pos,                          color, UIVec2(u0, v1), 1 }; m_pVertex++;
			*m_pVertex = { pos + UIVec2(0, size.y), color, UIVec2(u0, v0), 1 }; m_pVertex++;
			*m_pVertex = { pos + size,                   color, UIVec2(u1, v0), 1 }; m_pVertex++;
			*m_pVertex = { pos + UIVec2(size.x, 0), color, UIVec2(u1, v1), 1 }; m_pVertex++;

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
	*/

	/*
	void Painter::end()
	{
		m_Atlas->commit();

		// destroy temporal textures
		for (Graphics::Texture* tex : m_DestroyAfterCommitingAtlas)
			delete tex;
		m_DestroyAfterCommitingAtlas.clear();

		// unmap
		// states
		// draw m_IndexCount
	}
	*/
}
