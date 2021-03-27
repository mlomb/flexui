#include "flexui/Nodes/Element.hpp"

#include "flexui/Render/Painter.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Events/Events.hpp"
#include "flexui/Layout/ElementLayoutObject.hpp"

namespace flexui {

	Element::Element(const std::string& tag) :
		m_ComputedStyle(nullptr),
		m_PseudoStates(SelectorPseudoState::NONE)
	{
		setID("");
		setTag(tag);

		m_Layout = new ElementLayoutObject(this);
	}

	Element::~Element()
	{

	}

	void Element::drawContent(Painter& painter)
	{
		if (!m_ComputedStyle)
			return;

		const Rect& boundingRect = m_Layout->getLayoutRect();

		// painter.drawRectangle(boundingRect, 0xFFFF00FF);

		// TODO: % unit
		RoundedRectParams roundedParams;
		roundedParams.cornerRadii[0].x = roundedParams.cornerRadii[0].y = m_ComputedStyle->borderTopLeftRadius.value.number;
		roundedParams.cornerRadii[1].x = roundedParams.cornerRadii[1].y = m_ComputedStyle->borderTopRightRadius.value.number;
		roundedParams.cornerRadii[2].x = roundedParams.cornerRadii[2].y = m_ComputedStyle->borderBottomLeftRadius.value.number;
		roundedParams.cornerRadii[3].x = roundedParams.cornerRadii[3].y = m_ComputedStyle->borderBottomRightRadius.value.number;

		auto background_color = m_ComputedStyle->backgroundColor.value;
		auto border_color = m_ComputedStyle->borderColor.value;

		if ((background_color & 0xFF000000) > 0)
			painter.drawRoundedRectangle(boundingRect, background_color, roundedParams);
		if ((border_color & 0xFF000000) > 0) {
			RoundedBordersParams borderParams;
			borderParams.rectParams = roundedParams;
			// TODO: units
			borderParams.widths[0] = m_ComputedStyle->borderLeftWidth.value.number;
			borderParams.widths[1] = m_ComputedStyle->borderTopWidth.value.number;
			borderParams.widths[2] = m_ComputedStyle->borderRightWidth.value.number;
			borderParams.widths[3] = m_ComputedStyle->borderBottomWidth.value.number;
			painter.drawRoundedBorders(boundingRect, border_color, borderParams);
		}
	}

	void Element::executeDefault(EventBase* evt)
    {
        switch (evt->type) {
        case EventTypeID::MOUSE_ENTER:
            setPseudoStates(SelectorPseudoState::HOVER);
            break;
        case EventTypeID::MOUSE_LEAVE:
            removePseudoStates(SelectorPseudoState::HOVER);
            break;
        case EventTypeID::CAPTURE_IN:
            setPseudoStates(SelectorPseudoState::ACTIVE);
            break;
        case EventTypeID::CAPTURE_OUT:
            removePseudoStates(SelectorPseudoState::ACTIVE);
            break;
        }
    }

	void Element::setID(const std::string& id)
	{
		m_ID = { SelectorIdentifierType::ID, id };
		m_ID.computeHash();
	}

	void Element::setTag(const std::string& tag)
	{
		m_Tag = { SelectorIdentifierType::TAG, tag };
		m_Tag.computeHash();
	}

	void Element::addClass(const std::string& klass)
	{
		SelectorIdentifier si = { SelectorIdentifierType::CLASS, klass };
		si.computeHash();

		for (auto it = m_Classes.begin(); it != m_Classes.end(); it++) {
			if ((*it).text_hash == si.text_hash)
				return; // already present
		}

		m_Classes.emplace_back(si);
	}

    void Element::setPseudoStates(const SelectorPseudoState states)
    {
        m_PseudoStates |= states;
    }

    void Element::removePseudoStates(const SelectorPseudoState states)
    {
        m_PseudoStates &= ~states;
    }

	Rect Element::getContentRect() const
	{
		auto layoutRect = m_Layout->getLayoutRect();

		if (!m_ComputedStyle)
			return layoutRect;

		float l = m_ComputedStyle->paddingLeft.value.number;
		float t = m_ComputedStyle->paddingTop.value.number;
		float r = m_ComputedStyle->paddingRight.value.number;
		float b = m_ComputedStyle->paddingBottom.value.number;
		return Rect(
			layoutRect.x + l,
			layoutRect.y + t,
			layoutRect.width - l - r,
			layoutRect.height - t - b
		);
    }

    bool Element::isVisible() const
    {
        return m_ComputedStyle && m_ComputedStyle->display.value == Display::FLEX;
    }

	std::string Element::getDebugInfo() const
	{
		std::string dbg = ContainerNode::getDebugInfo();

		if (m_ID.text.size() > 0)
			dbg += "#" + m_ID.text;

		for (auto klass : m_Classes)
			dbg += " ." + klass.text;

		return dbg;
	}
}
