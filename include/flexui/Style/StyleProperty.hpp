#pragma once

#include <vector>
#include <string>

#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Misc/Parser.hpp"

namespace flexui {

	enum class StylePropertyID : uint8_t {
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

		TEXT_ALIGN, // TODO: !
		FONT_FAMILY,
		FONT_SIZE,
		WHITE_SPACE,

		CURSOR,

		// always keep it last
		LAST_PROPERTY_INVALID
	};

	struct StyleValue {
		union {
			String* string;

			float number;
			StyleLength length;
			Color color;

			Position position;
			FlexDirection direction;
			FlexWrap wrap;
			Align align;
			Justify justify;
			TextAlign textAlign;
			WhiteSpace whiteSpace;
			Display display;
			Overflow overflow;
			Cursor cursor;
		};
	};

	/// A single style property
	class StyleProperty {
	public:
		// The ownership of any valid pointer in the StyleValue parameter will
		// be transfered to this StyleProperty (for deletion)
		StyleProperty(const StylePropertyID id, const StyleValue& value);
		StyleProperty();
		~StyleProperty();

		// disable copying
		StyleProperty(const StyleProperty&) = delete;
		StyleProperty& operator = (const StyleProperty&) = delete;
		// enable move
		StyleProperty(StyleProperty&&) = default;
		StyleProperty& operator = (StyleProperty&&) = default;

		bool isValid() const { return m_ID != StylePropertyID::LAST_PROPERTY_INVALID;  }
		StylePropertyID getID() const { return m_ID; }
		const StyleValue& getValue() const { return m_Value; }

	private:
		StylePropertyID m_ID;
		StyleValue m_Value;
	};

	/// Parse a single style property line (i.e "margin: 5px !important;")
	/// The properties will be inserted into the `properties` vector
	/// (the vector will not be cleared)
	/// The properties are inserted into a vector because some style properties
	/// may expand into multiple (i.e margin: 5px will expand to 4 properties: left, right, etc)
	/// Returns true if at least one property is inserted
	bool ParseStylePropertyLine(const std::string&line, std::vector<StyleProperty>& properties, ParseResult& parseResult);

}
