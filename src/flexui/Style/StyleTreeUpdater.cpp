#include "flexui/Style/StyleTreeUpdater.hpp"

#include "flexui/Element.hpp"
#include "flexui/Surface.hpp"

#include "flexui/Style/StyleSheet.hpp"
#include "flexui/Style/StyleComputed.hpp"

#include "flexui/Layout/Yoga.hpp"

namespace flexui {

	StyleTreeUpdater::StyleTreeUpdater(Surface* surface)
		: TreeProcessor(surface)
	{
	}

	StyleTreeUpdater::~StyleTreeUpdater()
	{
	}

	void StyleTreeUpdater::process()
	{
		assert(m_SheetsStack.size() == 0);
		applyStyles(m_Surface->getRoot());
	}

	void StyleTreeUpdater::applyStyles(Element* element)
	{
		// TODO: check if we can skip this element

		// add this element's sheets to the sheets stack
		for (StyleSheet* ss : element->getStyleSheets())
			m_SheetsStack.push_back(ss);

		// find all selector matches
		m_MatchedSelectors.clear();
		StyleSelectorMatcher::FindMatches(element, m_SheetsStack, m_MatchedSelectors);

		// process matches
		processMatchedSelectors(element);

		// apply inherited (important: before traversing the subtree)
		if(element->m_Parent)
			element->m_ComputedStyle->inherit(*element->m_Parent->m_ComputedStyle);

		// sync with yoga
		syncWithLayout(element);

		// recurse
		for (Element* child : element->getChildrens())
			applyStyles(child);

		// remove sheets from stack, sheets only affect their subtree
		for (StyleSheet* ss : element->getStyleSheets())
			m_SheetsStack.pop_back();
	}

	void StyleTreeUpdater::processMatchedSelectors(Element* element)
	{
		std::sort(m_MatchedSelectors.begin(), m_MatchedSelectors.end()); // sorted by precedence

		// TODO: calculate the hash of the matched selectors
		//       and reuse the StyleComputed*
		//       (apply inline later, use same pointer if no inline rules)
		/*StyleHash selectorsHash = 0;
		for (const SelectorMatch& match : m_MatchedSelectors) {
			match.
		}*/

		StyleComputed* computedStyle = new StyleComputed();
		*computedStyle = GetDefaultStyleValues(); // copy defaults
		for (const SelectorMatch& match : m_MatchedSelectors) {
			assert(match.selector->rule);
			computedStyle->applyRule(*match.selector->rule);
		}

		computedStyle->applyRule(element->m_InlineRules);

		if (element->m_ComputedStyle)
			delete element->m_ComputedStyle;
		element->m_ComputedStyle = computedStyle;
	}

	void StyleTreeUpdater::syncWithLayout(Element* element)
	{
		const StyleComputed* computedStyle = element->m_ComputedStyle;
		const YGNodeRef yogaNode = element->m_YogaNode;

		// all values *should* be populated by now

		#define VA_ARGS(...) , ##__VA_ARGS__
		#define CAT(a, b) a##b
		#define SET_LENGTH(_base_fn, _value, ...) \
			if(_value.set_auto) { \
				_base_fn(yogaNode VA_ARGS(__VA_ARGS__), YGUndefined); \
			} else if(_value.unit == StyleLengthUnit::PERCENT) { \
				CAT(_base_fn, Percent)(yogaNode VA_ARGS(__VA_ARGS__), _value.number); \
			} else { /* PIXELS */ \
				_base_fn(yogaNode VA_ARGS(__VA_ARGS__), _value.number); \
			}
		#define SET_AUTO_LENGTH(_base_fn, _value, ...) \
			if(_value.set_auto) { \
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
