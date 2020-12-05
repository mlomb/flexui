#pragma once

#include <vector>
#include <string>

#include <yoga/YGEnums.h>
#include "flexui/Math.hpp"

// weird macros
#undef RELATIVE
#undef ABSOLUTE
#undef OVERFLOW

namespace flexui {

	enum class StylePropertyID {
		WIDTH,
		HEIGHT,
		MIN_WIDTH,
		MIN_HEIGHT,
		MAX_WIDTH,
		MAX_HEIGHT,

		MARGIN_LEFT,
		MARGIN_TOP,
		MARGIN_RIGHT,
		MARGIN_BOTTOM,

		PADDING_LEFT,
		PADDING_TOP,
		PADDING_RIGHT,
		PADDING_BOTTOM,

		BORDER_COLOR,
		BORDER_TOP_LEFT_RADIUS,
		BORDER_TOP_RIGHT_RADIUS,
		BORDER_BOTTOM_LEFT_RADIUS,
		BORDER_BOTTOM_RIGHT_RADIUS,
		BORDER_LEFT_WIDTH,
		BORDER_TOP_WIDTH,
		BORDER_RIGHT_WIDTH,
		BORDER_BOTTOM_WIDTH,

		FLEX_GROW,
		FLEX_SHRINK,
		FLEX_BASIS,
		FLEX_DIRECTION,
		FLEX_WRAP,

		ALIGN_SELF,
		ALIGN_ITEMS,
		ALIGN_CONTENT,
		JUSTIFY_CONTENT,

		POSITION,
		LEFT,
		TOP,
		RIGHT,
		BOTTOM,

		COLOR,
		BACKGROUND_COLOR,

		OVERFLOW,
		DISPLAY,

		FONT_FAMILY,
		FONT_SIZE,
		WHITE_SPACE,

		CURSOR,

		// always keep it last
		LAST_PROPERTY_INVALID
	};

	typedef uint32_t StyleHash;

	constexpr const StyleHash HashStr(const char* str, const int i = 0) {
		// TODO: maybe find a better hash
		return str[i] ? (HashStr(str, i + 1) * 33) ^ str[i] : 5381;
	}

	enum class Position {
		RELATIVE = YGPositionType::YGPositionTypeRelative,
		ABSOLUTE = YGPositionType::YGPositionTypeAbsolute
	};

	enum class FlexDirection {
		COLUMN = YGFlexDirection::YGFlexDirectionColumn,
		COLUMN_REVERSE = YGFlexDirection::YGFlexDirectionColumnReverse,
		ROW = YGFlexDirection::YGFlexDirectionRow,
		ROW_REVERSE = YGFlexDirection::YGFlexDirectionRowReverse
	};

	enum class FlexWrap {
		NOWRAP = YGWrap::YGWrapNoWrap,
		WRAP = YGWrap::YGWrapWrap,
		WRAP_REVERSE = YGWrap::YGWrapWrapReverse,
	};

	enum class Align {
		AUTO = YGAlign::YGAlignAuto,
		FLEX_START = YGAlign::YGAlignFlexStart,
		CENTER = YGAlign::YGAlignCenter,
		FLEX_END = YGAlign::YGAlignFlexEnd,
		STRETCH = YGAlign::YGAlignStretch,
		BASELINE = YGAlign::YGAlignBaseline,
		SPACE_BETWEEN = YGAlign::YGAlignSpaceBetween,
		SPACE_AROUND = YGAlign::YGAlignSpaceAround
	};

	enum class Justify {
		FLEX_START = YGJustify::YGJustifyFlexStart,
		CENTER = YGJustify::YGJustifyCenter,
		FLEX_END = YGJustify::YGJustifyFlexEnd,
		SPACE_BETWEEN = YGJustify::YGJustifySpaceBetween,
		SPACE_AROUND = YGJustify::YGJustifySpaceAround,
		SPACE_EVENLY = YGJustify::YGJustifySpaceEvenly
	};

	enum class Display {
		FLEX = YGDisplay::YGDisplayFlex,
		NONE = YGDisplay::YGDisplayNone
	};

	enum class Overflow {
		VISIBLE = YGOverflow::YGOverflowVisible,
		HIDDEN = YGOverflow::YGOverflowHidden,
		SCROLL = YGOverflow::YGOverflowScroll
	};

	enum class WhiteSpace {
		NORMAL = 0,
		NOWRAP,
	};

	enum class StyleCursor {
		AUTO,
		DEFAULT,
		NONE,
		HELP,
		POINTER,
		PROGRESS,
		WAIT,
		CROSSHAIR,
		TEXT,
		MOVE,
		NOT_ALLOWED,
		ALL_SCROLL,
		COL_RESIZE,
		ROW_RESIZE
	};

	enum class StyleLengthUnit {
		PIXELS,
		PERCENT
	};

	typedef float StyleNumber;

	struct StyleLength {
		bool set_auto = false;
		StyleNumber number;
		StyleLengthUnit unit;
	};

	typedef Color StyleColor;

	struct StyleValue {
		struct String { // TODO: make sure this doesn't leak! (probably it does)
			char* data;
			int8_t length;
		};
		union {
			String string;
			StyleNumber number;
			StyleLength length;
			StyleColor color;

			Position position;
			FlexDirection direction;
			FlexWrap wrap;
			Align align;
			Justify justify;
			WhiteSpace whiteSpace;
			Display display;
			Overflow overflow;
			StyleCursor cursor;
		};
	};

	struct StyleProperty {
		StylePropertyID id;
		StyleValue value;
	};

	struct StyleRule {
		std::vector<StyleProperty> properties;
	};

	enum class StyleIdentifierType {
		ID,
		TAG,
		CLASS
	};

	enum class StylePseudoStates : int {
		NONE     = 0,
		HOVER    = 1 << 0,
		DISABLED = 1 << 1,
		CHECKED  = 1 << 2,
		ACTIVE   = 1 << 3,
		FOCUS    = 1 << 4
	};

	enum class StyleSelectorRelationship {
		NONE, // same target (for example tag.class:pseudo)
		DESCENDANT, // default (any in subtree)
		CHILD, // >
		ADJACENT_SIBLING, // +
		GENERAL_SIBLING // ~
	};

	struct StyleIdentifier {
		StyleIdentifierType type;
		std::string text;
		StyleHash text_hash;

		void computeHash() { text_hash = HashStr(text.c_str()); }
		bool operator==(const StyleIdentifier& oth) const { return type == oth.type && text_hash == oth.text_hash; }
	};

	struct StyleSelectorPart {
		StyleIdentifier identifier;
		StyleSelectorRelationship prev_relationship;
		StylePseudoStates pseudo_states;
	};

	struct StyleSelector {
		std::vector<StyleSelectorPart> parts;
		StyleRule* rule;
		int specificity;
		int order;

		void computeSpecificity();
	};

	// operators for StylePseudoStates
	inline StylePseudoStates operator~(const StylePseudoStates& a) { return (StylePseudoStates)~(int)a; }
	inline StylePseudoStates operator&(const StylePseudoStates& a, const StylePseudoStates& b) { return (StylePseudoStates)((int)a & (int)b); }
	inline StylePseudoStates& operator|=(StylePseudoStates& a, const StylePseudoStates b) { return (StylePseudoStates&)((int&)a |= (int)b); }
	inline StylePseudoStates& operator&=(StylePseudoStates& a, const StylePseudoStates b) { return (StylePseudoStates&)((int&)a &= (int)b); }
}
