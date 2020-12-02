#include "Style/StyleComputed.hpp"

namespace flexui {

	void StyleComputed::applyProperty(const StyleProperty& property)
	{
		using ID = StylePropertyID;

		switch (property.id) {
		#define SAVE_PROP(id, name) \
		case id: \
			name.populated = true; \
			name.inherited = false; \
			memcpy(&name.value, (void*)&property.value, sizeof(name.value)); \
			break;

		SAVE_PROP(ID::WIDTH,      width);
		SAVE_PROP(ID::HEIGHT,     height);
		SAVE_PROP(ID::MIN_WIDTH,  minWidth);
		SAVE_PROP(ID::MIN_HEIGHT, minHeight);
		SAVE_PROP(ID::MAX_WIDTH,  maxWidth);
		SAVE_PROP(ID::MAX_HEIGHT, maxHeight);
		
		SAVE_PROP(ID::MARGIN_LEFT,   marginLeft);
		SAVE_PROP(ID::MARGIN_TOP,    marginTop);
		SAVE_PROP(ID::MARGIN_RIGHT,  marginRight);
		SAVE_PROP(ID::MARGIN_BOTTOM, marginBottom);

		SAVE_PROP(ID::PADDING_LEFT,   paddingLeft);
		SAVE_PROP(ID::PADDING_TOP,    paddingTop);
		SAVE_PROP(ID::PADDING_RIGHT,  paddingRight);
		SAVE_PROP(ID::PADDING_BOTTOM, paddingBottom);

		SAVE_PROP(ID::BORDER_COLOR, borderColor);
		SAVE_PROP(ID::BORDER_TOP_LEFT_RADIUS,     borderTopLeftRadius);
		SAVE_PROP(ID::BORDER_TOP_RIGHT_RADIUS,    borderTopRightRadius);
		SAVE_PROP(ID::BORDER_BOTTOM_LEFT_RADIUS,  borderBottomLeftRadius);
		SAVE_PROP(ID::BORDER_BOTTOM_RIGHT_RADIUS, borderBottomRightRadius);
		SAVE_PROP(ID::BORDER_LEFT_WIDTH,   borderLeftWidth);
		SAVE_PROP(ID::BORDER_TOP_WIDTH,    borderTopWidth);
		SAVE_PROP(ID::BORDER_RIGHT_WIDTH,  borderRightWidth);
		SAVE_PROP(ID::BORDER_BOTTOM_WIDTH, borderBottomWidth);

		SAVE_PROP(ID::FLEX_GROW,      flexGrow);
		SAVE_PROP(ID::FLEX_SHRINK,    flexShrink);
		SAVE_PROP(ID::FLEX_BASIS,     flexBasis);
		SAVE_PROP(ID::FLEX_DIRECTION, flexDirection);
		SAVE_PROP(ID::FLEX_WRAP,      flexWrap);

		SAVE_PROP(ID::ALIGN_SELF,      alignSelf);
		SAVE_PROP(ID::ALIGN_ITEMS,     alignItems);
		SAVE_PROP(ID::ALIGN_CONTENT,   alignContent);
		SAVE_PROP(ID::JUSTIFY_CONTENT, justifyContent);

		SAVE_PROP(ID::POSITION, position);
		SAVE_PROP(ID::LEFT,   left);
		SAVE_PROP(ID::TOP,    top);
		SAVE_PROP(ID::RIGHT,  right);
		SAVE_PROP(ID::BOTTOM, bottom);

		SAVE_PROP(ID::COLOR, color);
		SAVE_PROP(ID::BACKGROUND_COLOR, backgroundColor);

		SAVE_PROP(ID::OVERFLOW, overflow);
		SAVE_PROP(ID::DISPLAY, display);

		SAVE_PROP(ID::FONT_SIZE, fontSize);
		SAVE_PROP(ID::WHITE_SPACE, whiteSpace);

		SAVE_PROP(ID::CURSOR, cursor);
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

}
