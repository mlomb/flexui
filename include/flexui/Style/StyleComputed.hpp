#pragma once

#include "flexui/Style/StyleDefinitions.hpp"

namespace flexui {

	template<typename T>
	struct StyleValueComputed {
		bool populated = false; // value is ready
		bool inherited = false; // value is being inherited
		T value;
	};

	struct StyleComputed {
		StyleValueComputed<StyleLength> width;
		StyleValueComputed<StyleLength> height;
		StyleValueComputed<StyleLength> minWidth;
		StyleValueComputed<StyleLength> minHeight;
		StyleValueComputed<StyleLength> maxWidth;
		StyleValueComputed<StyleLength> maxHeight;

		StyleValueComputed<StyleLength> marginLeft;
		StyleValueComputed<StyleLength> marginTop;
		StyleValueComputed<StyleLength> marginRight;
		StyleValueComputed<StyleLength> marginBottom;

		StyleValueComputed<StyleLength> paddingLeft;
		StyleValueComputed<StyleLength> paddingTop;
		StyleValueComputed<StyleLength> paddingRight;
		StyleValueComputed<StyleLength> paddingBottom;

		StyleValueComputed<StyleColor> borderColor;
		StyleValueComputed<StyleLength> borderTopLeftRadius;
		StyleValueComputed<StyleLength> borderTopRightRadius;
		StyleValueComputed<StyleLength> borderBottomLeftRadius;
		StyleValueComputed<StyleLength> borderBottomRightRadius;
		StyleValueComputed<StyleLength> borderLeftWidth;
		StyleValueComputed<StyleLength> borderTopWidth;
		StyleValueComputed<StyleLength> borderRightWidth;
		StyleValueComputed<StyleLength> borderBottomWidth;

		StyleValueComputed<StyleNumber> flexGrow;
		StyleValueComputed<StyleNumber> flexShrink;
		StyleValueComputed<StyleLength> flexBasis;
		StyleValueComputed<FlexDirection> flexDirection;
		StyleValueComputed<FlexWrap> flexWrap;

		StyleValueComputed<Align> alignSelf;
		StyleValueComputed<Align> alignItems;
		StyleValueComputed<Align> alignContent;
		StyleValueComputed<Justify> justifyContent;

		StyleValueComputed<Position> position;
		StyleValueComputed<StyleLength> left;
		StyleValueComputed<StyleLength> top;
		StyleValueComputed<StyleLength> right;
		StyleValueComputed<StyleLength> bottom;

		StyleValueComputed<StyleColor> color;
		StyleValueComputed<StyleColor> backgroundColor;

		StyleValueComputed<Overflow> overflow;
		StyleValueComputed<Display> display;

		StyleValueComputed<StyleLength> fontSize;
		StyleValueComputed<WhiteSpace> whiteSpace;

		StyleValueComputed<StyleCursor> cursor;

		void applyProperty(const StyleProperty& property);
		void applyRule(const StyleRule& rule);
		void inherit(const StyleComputed& source);
	};

	constexpr StyleComputed GetDefaultStyleValues()
	{
		StyleComputed defaults = { };
		
		#define SET_NUMBER(prop_name, _value) \
		defaults.prop_name.populated = true; \
		defaults.prop_name.inherited = true; \
		defaults.prop_name.value = _value;

		#define SET_LENGTH(prop_name, _value, _unit) \
		defaults.prop_name.populated = true; \
		defaults.prop_name.inherited = true; \
		defaults.prop_name.value.number = _value; \
		defaults.prop_name.value.unit = _unit;

		#define SET_COLOR(prop_name, _value) \
		defaults.prop_name.populated = true; \
		defaults.prop_name.inherited = true; \
		defaults.prop_name.value = _value;

		#define SET_ENUM(prop_name, _enum_value) \
		defaults.prop_name.populated = true; \
		defaults.prop_name.inherited = true; \
		defaults.prop_name.value = _enum_value;

		#define SET_AUTO(prop_name) \
		defaults.prop_name.populated = true; \
		defaults.prop_name.inherited = true; \
		defaults.prop_name.value.set_auto = true;

		SET_AUTO(width);
		SET_AUTO(height);
		SET_AUTO(minWidth);
		SET_AUTO(minHeight);
		SET_AUTO(maxWidth);
		SET_AUTO(maxHeight);

		SET_LENGTH(marginLeft, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(marginTop, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(marginRight, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(marginBottom, 0, StyleLengthUnit::PIXELS);

		SET_LENGTH(paddingLeft, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(paddingTop, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(paddingRight, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(paddingBottom, 0, StyleLengthUnit::PIXELS);

		SET_COLOR(borderColor, 0x00000000); // transparent
		SET_LENGTH(borderTopLeftRadius, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderTopRightRadius, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderBottomLeftRadius, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderBottomRightRadius, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderLeftWidth, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderTopWidth, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderRightWidth, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(borderBottomWidth, 0, StyleLengthUnit::PIXELS);

		SET_NUMBER(flexGrow, 0);
		SET_NUMBER(flexShrink, 1);
		SET_AUTO(flexBasis);

		SET_ENUM(flexDirection, FlexDirection::ROW);
		SET_ENUM(flexWrap, FlexWrap::NOWRAP);
		SET_ENUM(alignSelf, Align::AUTO);
		SET_ENUM(alignItems, Align::STRETCH);
		SET_ENUM(alignContent, Align::FLEX_START);
		SET_ENUM(justifyContent, Justify::FLEX_START);

		SET_ENUM(position, (Position)0); // Position::RELATIVE
		SET_LENGTH(left, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(top, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(right, 0, StyleLengthUnit::PIXELS);
		SET_LENGTH(bottom, 0, StyleLengthUnit::PIXELS);

		SET_COLOR(color, 0xFFFFFFFF); // white
		SET_COLOR(backgroundColor, 0x00000000); // transparent
		
		SET_ENUM(overflow, Overflow::VISIBLE);
		SET_ENUM(display, Display::FLEX);
		
		SET_LENGTH(fontSize, 14, StyleLengthUnit::PIXELS);
		SET_ENUM(whiteSpace, WhiteSpace::NORMAL);

		SET_ENUM(cursor, StyleCursor::AUTO);

		#undef SET_NUMBER
		#undef SET_LENGTH
		#undef SET_COLOR
		#undef SET_ENUM
		#undef SET_AUTO

		return defaults;
	}
	
}
