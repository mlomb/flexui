#include "flexui/Layout/ElementLayoutObject.hpp"

#include "flexui/Nodes/Element.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Layout/Yoga.hpp"

namespace flexui {

	ElementLayoutObject::ElementLayoutObject(Element* element)
		: m_Element(element)
	{
	}

	ElementLayoutObject::~ElementLayoutObject()
	{
	}

	void ElementLayoutObject::syncStyles() const
	{
		const StyleComputed* computedStyle = m_Element->getComputedStyle();
		const YGNodeRef yogaNode = m_YogaNode;

		// all values *should* be populated by now

		#define VA_ARGS(...) , ##__VA_ARGS__
		#define CAT(a, b) a##b
		#define SET_LENGTH(_base_fn, _value, ...) \
			if(_value.unit == StyleLengthUnit::AUTO) { \
				_base_fn(yogaNode VA_ARGS(__VA_ARGS__), YGUndefined); \
			} else if(_value.unit == StyleLengthUnit::PERCENT) { \
				CAT(_base_fn, Percent)(yogaNode VA_ARGS(__VA_ARGS__), _value.number); \
			} else { /* PIXELS */ \
				_base_fn(yogaNode VA_ARGS(__VA_ARGS__), _value.number); \
			}
		#define SET_AUTO_LENGTH(_base_fn, _value, ...) \
			if(_value.unit == StyleLengthUnit::AUTO) { \
				CAT(_base_fn, Auto)(yogaNode VA_ARGS(__VA_ARGS__)); \
			} else SET_LENGTH(_base_fn, _value, __VA_ARGS__);

		YGNodeStyleSetFlex(yogaNode, YGUndefined);

		SET_AUTO_LENGTH(YGNodeStyleSetWidth, computedStyle->width.value);
		SET_AUTO_LENGTH(YGNodeStyleSetHeight, computedStyle->height.value);
		SET_LENGTH(YGNodeStyleSetMinWidth, computedStyle->minWidth.value);
		SET_LENGTH(YGNodeStyleSetMinHeight, computedStyle->minHeight.value);
		SET_LENGTH(YGNodeStyleSetMaxWidth, computedStyle->maxWidth.value);
		SET_LENGTH(YGNodeStyleSetMaxHeight, computedStyle->maxHeight.value);

		SET_AUTO_LENGTH(YGNodeStyleSetMargin, computedStyle->marginLeft.value, YGEdge::YGEdgeLeft);
		SET_AUTO_LENGTH(YGNodeStyleSetMargin, computedStyle->marginTop.value, YGEdge::YGEdgeTop);
		SET_AUTO_LENGTH(YGNodeStyleSetMargin, computedStyle->marginRight.value, YGEdge::YGEdgeRight);
		SET_AUTO_LENGTH(YGNodeStyleSetMargin, computedStyle->marginBottom.value, YGEdge::YGEdgeBottom);
		
		SET_LENGTH(YGNodeStyleSetPadding, computedStyle->paddingLeft.value, YGEdge::YGEdgeLeft);
		SET_LENGTH(YGNodeStyleSetPadding, computedStyle->paddingTop.value, YGEdge::YGEdgeTop);
		SET_LENGTH(YGNodeStyleSetPadding, computedStyle->paddingRight.value, YGEdge::YGEdgeRight);
		SET_LENGTH(YGNodeStyleSetPadding, computedStyle->paddingBottom.value, YGEdge::YGEdgeBottom);
		
		// TODO: % unit?
		YGNodeStyleSetBorder(yogaNode, YGEdge::YGEdgeLeft, computedStyle->borderLeftWidth.value.number);
		YGNodeStyleSetBorder(yogaNode, YGEdge::YGEdgeTop, computedStyle->borderTopWidth.value.number);
		YGNodeStyleSetBorder(yogaNode, YGEdge::YGEdgeRight, computedStyle->borderRightWidth.value.number);
		YGNodeStyleSetBorder(yogaNode, YGEdge::YGEdgeBottom, computedStyle->borderBottomWidth.value.number);
		
		YGNodeStyleSetFlexGrow(yogaNode, computedStyle->flexGrow.value);
		YGNodeStyleSetFlexShrink(yogaNode, computedStyle->flexShrink.value);
		SET_AUTO_LENGTH(YGNodeStyleSetFlexBasis, computedStyle->flexBasis.value);
		
		YGNodeStyleSetFlexDirection(yogaNode, (YGFlexDirection)computedStyle->flexDirection.value);
		YGNodeStyleSetFlexWrap(yogaNode, (YGWrap)computedStyle->flexWrap.value);
		YGNodeStyleSetAlignSelf(yogaNode, (YGAlign)computedStyle->alignSelf.value);
		YGNodeStyleSetAlignContent(yogaNode, (YGAlign)computedStyle->alignContent.value);
		YGNodeStyleSetAlignItems(yogaNode, (YGAlign)computedStyle->alignItems.value);
		YGNodeStyleSetJustifyContent(yogaNode, (YGJustify)computedStyle->justifyContent.value);
		
		YGNodeStyleSetPositionType(yogaNode, (YGPositionType)computedStyle->position.value);
		SET_LENGTH(YGNodeStyleSetPosition, computedStyle->left.value, YGEdge::YGEdgeLeft);
		SET_LENGTH(YGNodeStyleSetPosition, computedStyle->top.value, YGEdge::YGEdgeTop);
		SET_LENGTH(YGNodeStyleSetPosition, computedStyle->right.value, YGEdge::YGEdgeRight);
		SET_LENGTH(YGNodeStyleSetPosition, computedStyle->bottom.value, YGEdge::YGEdgeBottom);
		
		YGNodeStyleSetOverflow(yogaNode, (YGOverflow)computedStyle->overflow.value);
		YGNodeStyleSetDisplay(yogaNode, (YGDisplay)computedStyle->display.value);
	}

}
