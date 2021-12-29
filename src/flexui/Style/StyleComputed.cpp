#include "flexui/Style/StyleComputed.hpp"

#include <cstring>  // memcpy

#include "flexui/Style/StyleProperty.hpp"
#include "flexui/Style/StyleSheet.hpp"

namespace flexui {

	void StyleComputed::applyProperty(const StyleProperty& property)
	{
		using ID = StylePropertyID;

		switch (property.getID()) {
		#define SAVE_PROP(id, name, key) \
		case id: \
			name.populated = true; \
			name.inherited = false; \
			name.value = property.getValue().key; \
			break;

		SAVE_PROP(ID::WIDTH,      width, length);
		SAVE_PROP(ID::HEIGHT,     height, length);
		SAVE_PROP(ID::MIN_WIDTH,  minWidth, length);
		SAVE_PROP(ID::MIN_HEIGHT, minHeight, length);
		SAVE_PROP(ID::MAX_WIDTH,  maxWidth, length);
		SAVE_PROP(ID::MAX_HEIGHT, maxHeight, length);

		SAVE_PROP(ID::MARGIN_LEFT,   marginLeft, length);
		SAVE_PROP(ID::MARGIN_TOP,    marginTop, length);
		SAVE_PROP(ID::MARGIN_RIGHT,  marginRight, length);
		SAVE_PROP(ID::MARGIN_BOTTOM, marginBottom, length);

		SAVE_PROP(ID::PADDING_LEFT,   paddingLeft, length);
		SAVE_PROP(ID::PADDING_TOP,    paddingTop, length);
		SAVE_PROP(ID::PADDING_RIGHT,  paddingRight, length);
		SAVE_PROP(ID::PADDING_BOTTOM, paddingBottom, length);

		SAVE_PROP(ID::BORDER_COLOR, borderColor, color);
		SAVE_PROP(ID::BORDER_TOP_LEFT_RADIUS,     borderTopLeftRadius, length);
		SAVE_PROP(ID::BORDER_TOP_RIGHT_RADIUS,    borderTopRightRadius, length);
		SAVE_PROP(ID::BORDER_BOTTOM_LEFT_RADIUS,  borderBottomLeftRadius, length);
		SAVE_PROP(ID::BORDER_BOTTOM_RIGHT_RADIUS, borderBottomRightRadius, length);
		SAVE_PROP(ID::BORDER_LEFT_WIDTH,   borderLeftWidth, length);
		SAVE_PROP(ID::BORDER_TOP_WIDTH,    borderTopWidth, length);
		SAVE_PROP(ID::BORDER_RIGHT_WIDTH,  borderRightWidth, length);
		SAVE_PROP(ID::BORDER_BOTTOM_WIDTH, borderBottomWidth, length);

		SAVE_PROP(ID::FLEX_GROW,      flexGrow, number);
		SAVE_PROP(ID::FLEX_SHRINK,    flexShrink, number);
		SAVE_PROP(ID::FLEX_BASIS,     flexBasis, length);
		SAVE_PROP(ID::FLEX_DIRECTION, flexDirection, direction);
		SAVE_PROP(ID::FLEX_WRAP,      flexWrap, wrap);

		SAVE_PROP(ID::ALIGN_SELF,      alignSelf, align);
		SAVE_PROP(ID::ALIGN_ITEMS,     alignItems, align);
		SAVE_PROP(ID::ALIGN_CONTENT,   alignContent, align);
		SAVE_PROP(ID::JUSTIFY_CONTENT, justifyContent, justify);

		SAVE_PROP(ID::POSITION, position, position);
		SAVE_PROP(ID::LEFT,   left, length);
		SAVE_PROP(ID::TOP,    top, length);
		SAVE_PROP(ID::RIGHT,  right, length);
		SAVE_PROP(ID::BOTTOM, bottom, length);

		SAVE_PROP(ID::COLOR, color, color);
		SAVE_PROP(ID::BACKGROUND_COLOR, backgroundColor, color);

		SAVE_PROP(ID::OVERFLOW, overflow, overflow);
		SAVE_PROP(ID::DISPLAY, display, display);

		//SAVE_PROP(ID::FONT_FAMILY, fontFamily, string);
		SAVE_PROP(ID::FONT_SIZE, fontSize, length);
		SAVE_PROP(ID::WHITE_SPACE, whiteSpace, whiteSpace);

		SAVE_PROP(ID::CURSOR, cursor, cursor);
		}
	}

	void StyleComputed::applyRule(const StyleRule& rule)
	{
		for (const StyleProperty& prop : rule.properties)
			applyProperty(prop);
	}

	void StyleComputed::inherit(const StyleComputed& source)
	{
		#define INHERIT(prop_name) \
		if (!this->prop_name.populated || this->prop_name.inherited) { \
			this->prop_name.inherited = true; \
			this->prop_name.populated = true; \
			this->prop_name.value = source.prop_name.value; \
		}

		// only some properties are inherited

		INHERIT(color);
		INHERIT(fontSize);
		INHERIT(whiteSpace);
		INHERIT(cursor);
	}

	StyleComputed GetDefaultStyleValues()
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
		defaults.prop_name.value.unit = StyleLengthUnit::AUTO;

		SET_AUTO(width);
		SET_AUTO(height);
		SET_AUTO(minWidth);
		SET_AUTO(minHeight);
		SET_AUTO(maxWidth);
		SET_AUTO(maxHeight);

		SET_LENGTH(marginLeft, 0, StyleLengthUnit::PX);
		SET_LENGTH(marginTop, 0, StyleLengthUnit::PX);
		SET_LENGTH(marginRight, 0, StyleLengthUnit::PX);
		SET_LENGTH(marginBottom, 0, StyleLengthUnit::PX);

		SET_LENGTH(paddingLeft, 0, StyleLengthUnit::PX);
		SET_LENGTH(paddingTop, 0, StyleLengthUnit::PX);
		SET_LENGTH(paddingRight, 0, StyleLengthUnit::PX);
		SET_LENGTH(paddingBottom, 0, StyleLengthUnit::PX);

		SET_COLOR(borderColor, 0x00000000); // transparent
		SET_LENGTH(borderTopLeftRadius, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderTopRightRadius, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderBottomLeftRadius, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderBottomRightRadius, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderLeftWidth, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderTopWidth, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderRightWidth, 0, StyleLengthUnit::PX);
		SET_LENGTH(borderBottomWidth, 0, StyleLengthUnit::PX);

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
		SET_LENGTH(left, 0, StyleLengthUnit::PX);
		SET_LENGTH(top, 0, StyleLengthUnit::PX);
		SET_LENGTH(right, 0, StyleLengthUnit::PX);
		SET_LENGTH(bottom, 0, StyleLengthUnit::PX);

		SET_COLOR(color, 0xFFFFFFFF); // white
		SET_COLOR(backgroundColor, 0x00000000); // transparent
		
		SET_ENUM(overflow, Overflow::VISIBLE);
		SET_ENUM(display, Display::FLEX);

		defaults.fontFamily.inherited = true;
		defaults.fontFamily.populated = true;
        defaults.fontFamily.value = "default";
		SET_LENGTH(fontSize, 14, StyleLengthUnit::PX);
		SET_ENUM(whiteSpace, WhiteSpace::NORMAL);

		SET_ENUM(cursor, Cursor::AUTO);

		#undef SET_NUMBER
		#undef SET_LENGTH
		#undef SET_COLOR
		#undef SET_ENUM
		#undef SET_AUTO

		return defaults;
	}
	
}
