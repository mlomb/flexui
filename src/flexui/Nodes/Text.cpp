#include "flexui/Nodes/Text.hpp"

#include "flexui/Nodes/Element.hpp"
#include "flexui/Nodes/Document.hpp"
#include "flexui/Layout/TextLayoutObject.hpp"
#include "flexui/Render/Font.hpp"
#include "flexui/Render/Painter.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Providers/ResourceProvider.hpp"
#include "flexui/Log.hpp"

namespace flexui {

    Text::Text(const std::string& text)
        : m_Text(text)
    {
        m_Layout = new TextLayoutObject(this);
    }

    Text::~Text() {
    }

    const StyleComputed* Text::getParentComputedStyle() const
    {
        // We use the parent's computed style for styling since Text nodes
        // don't have StyleComputed for memory and performance reasons
        const Node* parent_node = getParentNode();
        FUI_ASSERT(parent_node->getNodeType() == NodeType::ELEMENT);
        const Element* parent_element = static_cast<const Element*>(parent_node);
        return parent_element->getComputedStyle();
    }

    void Text::drawContent(Painter& painter)
    {
        if (m_Text.size() == 0)
            return;

        auto computed_style = getParentComputedStyle();

        if (!getDocument()->getResourceProvider())
            return;

        std::shared_ptr<Font> font = getDocument()->getResourceProvider()->getFont(computed_style->fontFamily.value);

        if (font == nullptr)
            return;

        auto box = m_Layout->getLayoutRect();

        TextLayoutSettings layoutSettings;
        layoutSettings.style.size = computed_style->fontSize.value.number; // TODO: units
        layoutSettings.wrap = TextWrap::WORD;
        layoutSettings.maxWidth = box.width;

        TextLayout layout;
        font->generateTextLayout(m_Text, layoutSettings, layout);
        painter.drawText(font.get(), layout, box.position, computed_style->color.value);
    }

}