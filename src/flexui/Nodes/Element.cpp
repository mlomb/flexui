#include "flexui/Nodes/Element.hpp"

#include "flexui/Layout/Yoga.hpp"
#include "flexui/Render/Painter.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Events/Events.hpp"

namespace flexui {

	Element::Element() :
		m_Surface(nullptr),
		m_ComputedStyle(nullptr),
		m_PseudoStates(StylePseudoStates::NONE)
	{
		setID("");
		setTag("Element");

		m_YogaNode = YGNodeNew();
		YGNodeSetContext(m_YogaNode, this);
	}

	Element::~Element()
	{
		// assert(m_Parent == nullptr && "Element is still attached to the tree");
		
		YGNodeFreeRecursive(m_YogaNode);
	}

	/*
	// this function sets the correct owner in yoga land
	YGNodeInsertChild(m_YogaNode, child->m_YogaNode, index);
	*/

	void Element::paintContent(Painter* painter)
	{
		if (!m_ComputedStyle)
			return;

		//painter->drawRectangle(m_BoundingBox, color);

		// TODO: % unit
		RoundedRectParams roundedParams;
		roundedParams.cornerRadii[0].x = roundedParams.cornerRadii[0].y = m_ComputedStyle->borderTopLeftRadius.value.number;
		roundedParams.cornerRadii[1].x = roundedParams.cornerRadii[1].y = m_ComputedStyle->borderTopRightRadius.value.number;
		roundedParams.cornerRadii[2].x = roundedParams.cornerRadii[2].y = m_ComputedStyle->borderBottomLeftRadius.value.number;
		roundedParams.cornerRadii[3].x = roundedParams.cornerRadii[3].y = m_ComputedStyle->borderBottomRightRadius.value.number;

		auto background_color = m_ComputedStyle->backgroundColor.value;
		auto border_color = m_ComputedStyle->borderColor.value;

		if ((background_color & 0xFF000000) > 0)
			painter->drawRoundedRectangle(m_BoundingRect, background_color, roundedParams);
		if ((border_color & 0xFF000000) > 0) {
			RoundedBordersParams borderParams;
			borderParams.rectParams = roundedParams;
			// TODO: units
			borderParams.widths[0] = m_ComputedStyle->borderLeftWidth.value.number;
			borderParams.widths[1] = m_ComputedStyle->borderTopWidth.value.number;
			borderParams.widths[2] = m_ComputedStyle->borderRightWidth.value.number;
			borderParams.widths[3] = m_ComputedStyle->borderBottomWidth.value.number;
			painter->drawRoundedBorders(m_BoundingRect, border_color, borderParams);
		}
	}

	Vec2 Element::measureContent(float width, MeasureMode widthMode, float height, MeasureMode heightMode)
	{
		assert(false && "measureContent is not overrided");
		return { 0, 0 };
    }

    void Element::executeDefault(EventBase* evt)
    {
        switch (evt->type) {
        case EventTypeID::MOUSE_ENTER:
            setPseudoStates(StylePseudoStates::HOVER);
            break;
        case EventTypeID::MOUSE_LEAVE:
            removePseudoStates(StylePseudoStates::HOVER);
            break;
        case EventTypeID::CAPTURE_IN:
            setPseudoStates(StylePseudoStates::ACTIVE);
            break;
        case EventTypeID::CAPTURE_OUT:
            removePseudoStates(StylePseudoStates::ACTIVE);
            break;
        }
    }

	MeasureMode YogaMeasureModeToMeasureMode(YGMeasureMode mode) {
		switch (mode)
		{
		default:
		case YGMeasureModeUndefined: return MeasureMode::UNDEFINED;
		case YGMeasureModeExactly: return MeasureMode::EXACTLY;
		case YGMeasureModeAtMost: return MeasureMode::AT_MOST;
		}
	}

	YGSize YogaMeasureCallback(YGNode* yogaNode, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode)
	{
		Element* element_ptr = static_cast<Element*>(yogaNode->getContext());
		auto size = element_ptr->measureContent(width, YogaMeasureModeToMeasureMode(widthMode), height, YogaMeasureModeToMeasureMode(heightMode));
		return { size.x, size.y };
	}

	void Element::setAsTextType()
    {
        YGNodeSetNodeType(m_YogaNode, YGNodeType::YGNodeTypeText);
	}

	void Element::enableMeasurement()
    {
        YGNodeSetMeasureFunc(m_YogaNode, YogaMeasureCallback);
	}

	void Element::setID(const std::string& id)
	{
		m_ID = { StyleIdentifierType::ID, id };
		m_ID.computeHash();
	}

	void Element::setTag(const std::string& tag)
	{
		m_Tag = { StyleIdentifierType::TAG, tag };
		m_Tag.computeHash();
	}

	void Element::addClass(const std::string& klass)
	{
		StyleIdentifier si = { StyleIdentifierType::CLASS, klass };
		si.computeHash();

		for (auto it = m_Classes.begin(); it != m_Classes.end(); it++) {
			if ((*it).text_hash == si.text_hash)
				return; // already present
		}

		m_Classes.emplace_back(si);
	}

	void Element::addStyleSheet(StyleSheet* stylesheet)
	{
		m_StyleSheets.emplace_back(stylesheet);
	}

    void Element::setPseudoStates(const StylePseudoStates states)
    {
        m_PseudoStates |= states;
    }

    void Element::removePseudoStates(const StylePseudoStates states)
    {
        m_PseudoStates &= ~states;
    }

	const std::vector<StyleSheet*>& Element::getStyleSheets() const
	{
		return m_StyleSheets;
	}

	Rect Element::getBoundingRect() const
	{
		return m_BoundingRect;
	}

	Rect Element::getContentRect() const
	{
		if (!m_ComputedStyle)
			return m_BoundingRect;

		float l = m_ComputedStyle->paddingLeft.value.number;
		float t = m_ComputedStyle->paddingTop.value.number;
		float r = m_ComputedStyle->paddingRight.value.number;
		float b = m_ComputedStyle->paddingBottom.value.number;
		return Rect(
			m_BoundingRect.x + l,
			m_BoundingRect.y + t,
			m_BoundingRect.width - l - r,
			m_BoundingRect.height - t - b
		);
    }

    bool Element::isVisible() const
    {
        return m_ComputedStyle && m_ComputedStyle->display.value == Display::FLEX;
    }

    Surface* Element::getSurface() const
    {
        return m_Surface;
    }

	std::string Element::getDebugLine() const
	{
		std::string dbg = getName();
		
		if (m_ID.text.size() > 0)
			dbg += "#" + m_ID.text;

		for(auto klass : m_Classes)
			dbg += " ." + klass.text;

		return dbg;
	}
}
