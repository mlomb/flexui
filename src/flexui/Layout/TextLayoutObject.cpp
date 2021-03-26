#include "flexui/Layout/TextLayoutObject.hpp"

#include <cassert>
#include <cmath>
#include <codecvt>
#include <locale>

#ifndef NAN
#define NAN (nan(NULL))
#else
// NAN from corecrt_math.h
#endif

#include "flexui/Render/Font.hpp"
#include "flexui/Layout/Yoga.hpp"
#include "flexui/Nodes/Document.hpp"
#include "flexui/Nodes/Text.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Providers/ResourceProvider.hpp"
#include "flexui/Log.hpp"

namespace flexui {

	YGSize YogaMeasureTextCallback(YGNode* yogaNode, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
	{
        // See https://github.com/facebook/yoga/pull/576/files

        float measuredWidth = NAN;
        float measuredHeight = NAN;

        const TextLayoutObject* text_object_ptr = static_cast<TextLayoutObject*>(yogaNode->getContext());
        const Text* text_node = text_object_ptr->getTextNode();
        const StyleComputed* computed_style = text_node->getParentComputedStyle();
        
        if(computed_style == nullptr || text_node->getText().size() == 0)
            return { measuredWidth, measuredHeight };

        FUI_ASSERT(text_node->getDocument());

        if (!text_node->getDocument()->getResourceProvider())
            return { measuredWidth, measuredHeight };

        std::shared_ptr<Font> font = text_node->getDocument()->getResourceProvider()->getFont(computed_style->fontFamily.value);

        if (font == nullptr)
            return { measuredWidth, measuredHeight };

        TextLayout layout;

        TextLayoutSettings layoutSettings;
        layoutSettings.skip_glyphs = true; // we don't need each glyph
        layoutSettings.style.size = computed_style->fontSize.value.number; // TODO: units

        if (widthMode == YGMeasureMode::YGMeasureModeExactly) {
            measuredWidth = width;
        } else {
            layoutSettings.wrap = TextWrap::NONE;

            font->generateTextLayout(text_node->getText(), layoutSettings, layout);
            measuredWidth = std::ceil(layout.width);

            if (widthMode == YGMeasureMode::YGMeasureModeAtMost)
                measuredWidth = std::min(measuredWidth, width);
        }

        if (heightMode == YGMeasureMode::YGMeasureModeExactly) {
            measuredHeight = height;
        } else {
            layoutSettings.wrap = TextWrap::WORD;
            layoutSettings.maxWidth = measuredWidth;

            font->generateTextLayout(text_node->getText(), layoutSettings, layout);
            measuredHeight = std::ceil(layout.height);

            if (heightMode == YGMeasureMode::YGMeasureModeAtMost)
                measuredHeight = std::min(measuredHeight, height);
        }

        return { measuredWidth, measuredHeight };
	}

    TextLayoutObject::TextLayoutObject(Text* text)
		: m_Text(text)
	{
		YGNodeSetNodeType(m_YogaNode, YGNodeType::YGNodeTypeText);
		YGNodeSetMeasureFunc(m_YogaNode, YogaMeasureTextCallback);
	}

	TextLayoutObject::~TextLayoutObject()
	{
	}
}
