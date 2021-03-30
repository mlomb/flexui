#pragma once

#include <vector>
#include <string>
#include <stdint.h>

#include <yoga/YGEnums.h>

#include "flexui/Render/Font.hpp"
#include "flexui/Selectors/Selector.hpp"
#include "flexui/Misc/Color.hpp"

// weird macros
#undef RELATIVE
#undef ABSOLUTE
#undef OVERFLOW

namespace flexui {

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

	enum class Cursor {
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
		AUTO,
		PERCENT,
		PX, // not actually screen pixels http://inamidst.com/stuff/notes/csspx
		IN,
		CM,
		MM,
		EM,
		VW,
		VH
	};

	struct StyleLength {
		StyleLengthUnit unit;
		float number;
	};

}
