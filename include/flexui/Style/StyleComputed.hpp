#pragma once

#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Misc/Color.hpp"

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

		StyleValueComputed<Color> borderColor;
		StyleValueComputed<StyleLength> borderTopLeftRadius;
		StyleValueComputed<StyleLength> borderTopRightRadius;
		StyleValueComputed<StyleLength> borderBottomLeftRadius;
		StyleValueComputed<StyleLength> borderBottomRightRadius;
		StyleValueComputed<StyleLength> borderLeftWidth;
		StyleValueComputed<StyleLength> borderTopWidth;
		StyleValueComputed<StyleLength> borderRightWidth;
		StyleValueComputed<StyleLength> borderBottomWidth;

		StyleValueComputed<float> flexGrow;
		StyleValueComputed<float> flexShrink;
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

		StyleValueComputed<Color> color;
		StyleValueComputed<Color> backgroundColor;

		StyleValueComputed<Overflow> overflow;
		StyleValueComputed<Display> display;

		StyleValueComputed<std::string> fontFamily;
		StyleValueComputed<StyleLength> fontSize;
		StyleValueComputed<WhiteSpace> whiteSpace;

		StyleValueComputed<Cursor> cursor;

		void applyProperty(const StyleProperty& property);
		void applyRule(const StyleRule& rule);
		void inherit(const StyleComputed& source);
	};

	StyleComputed GetDefaultStyleValues();

}
