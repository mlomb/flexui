#include "flexui/Style/StyleProperty.hpp"

#include "flexui/Style/NamedColors.hpp"

namespace flexui {

	/// Check if the character is a valid decimal digit
	bool isNumeric(char c) {
		return c >= '0' && c <= '9';
	}

	/// Check if the character is a valid hex digit
	bool isHex(char c) {
		return
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F') ||
			isNumeric(c);
	}

	/// Check if the character is a valid CSS property
	inline bool isPropertyNameChar(const char c) {
		return (c == '-') || (c >= 'a' && c <= 'z');
	}

	/// Converts decimal colors to Color
	Color buildColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		// AABBGGRR
		return ((uint32_t)(a) << 24) | ((uint32_t)(b) << 16) | ((uint32_t)(g) << 8) | ((uint32_t)(r));
	}

	/// Converts a hex digit to its decimal representation
	unsigned char hexToDec(unsigned char c) {
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return 0;
	}

	/// Converts a two digit hex to its decimal representation
	unsigned char hexToDec(unsigned char l, unsigned char r) {
		return 16 * hexToDec(l) + hexToDec(r);
	}

	bool parseNumber(const StringSection& input, int& pos, float& output, ParseResult& parseResult) {
		bool valid = false;
		bool negative = false;
		bool mantissa = false;
		int mantissa_place = 1;
		float value = 0;

		while (pos < input.length()) {
			char chr = input[pos];
			if (chr == '-') {
				if (pos == 0) {
					negative = true;
				}
				else {
					parseResult.warnings.emplace_back("Unexpected '-'");
					return false; // unexpected -
				}
			}
			else if (chr == '.') {
				if (mantissa) {
					parseResult.warnings.emplace_back("Unexpected '.'");
					return false; // unexpected .
				}
				mantissa = true;
			}
			else if (isNumeric(chr)) {
				int num = chr - '0';
				if (!mantissa) {
					// whole part
					value = value * 10 + num;
				}
				else {
					// decimal part
					value = value + (float)num / powf(10, (float)mantissa_place++);
				}
				valid = true;
			}
			else {
				break; // unexpected character (probably a unit)
			}
			pos++;
		}

		output = negative ? -value : value;

		return valid;
	}

	bool parseLength(const StringSection& input, StyleLength& output, ParseResult& parseResult) {
		int pos = 0;
		if (parseNumber(input, pos, output.number, parseResult)) {
			// number parsed
			if (pos < input.length()) {
				// more to read, must be a unit
				
				// set as AUTO, if continues to be AUTO after parsing, the unit was invalid
				output.unit = StyleLengthUnit::AUTO;

				if (input[pos] == '%') output.unit = StyleLengthUnit::PERCENT;
				else if (pos + 1 < input.length()) { // enough for two char unit
					if(false) { }
					#define CHECK_LENGTH_UNIT(a, b, _unit) \
					else if (input[pos] == a && input[pos + 1] == b) output.unit = _unit;

					// absolute lengths
					CHECK_LENGTH_UNIT('p', 'x', StyleLengthUnit::PX)
					CHECK_LENGTH_UNIT('i', 'n', StyleLengthUnit::IN)
					CHECK_LENGTH_UNIT('c', 'm', StyleLengthUnit::CM)
					CHECK_LENGTH_UNIT('m', 'm', StyleLengthUnit::MM)
					// font-relative lengths
					CHECK_LENGTH_UNIT('e', 'm', StyleLengthUnit::EM)
					// viewport lengths
					CHECK_LENGTH_UNIT('v', 'w', StyleLengthUnit::VW)
					CHECK_LENGTH_UNIT('v', 'h', StyleLengthUnit::VH)

					#undef CHECK_LENGTH_UNIT
				}
				
				if(output.unit == StyleLengthUnit::AUTO){
					// invalid unit
					parseResult.warnings.emplace_back("Unexpected character '" + std::string(1, input[pos]) + "' parsing length");
					return false; // unexpected character
				}
			}
			else {
				// by default pixels
				output.unit = StyleLengthUnit::PX;
				// annoying if using something like: margin: 0;
				// parseResult.warnings.emplace_back("Defaulting to pixels due absence of length unit");
			}

			return true;
		}
		else if (input.length() >= 4) {
			// check if auto
			if (input[0] == 'a' && input[1] == 'u' && input[2] == 't' && input[3] == 'o') {
				output.unit = StyleLengthUnit::AUTO;
				return true;
			}
		}

		return false;
	}

	bool parseColor(const StringSection& input, Color& output, ParseResult& parseResult) {
		if (input.length() < 2)
			return false;

		if (input[0] == '#') { // hex color
			// #RGB
			if (input.length() == 4) {
				if (isHex(input[1]) &&
					isHex(input[2]) &&
					isHex(input[3])) {
					output = buildColor(
						hexToDec(input[1], input[1]),
						hexToDec(input[2], input[2]),
						hexToDec(input[3], input[3]),
						(unsigned char)0xFF
					);
					return true;
				}
				else {
					parseResult.warnings.emplace_back("Invalid #RGB hex value");
					return false; // invalid hex
				}
			}
			// #RRGGBB
			else if (input.length() == 7) {
				if (isHex(input[1]) &&
					isHex(input[2]) &&
					isHex(input[3]) &&
					isHex(input[4]) &&
					isHex(input[5]) &&
					isHex(input[6])) {
					output = buildColor(
						hexToDec(input[1], input[2]),
						hexToDec(input[3], input[4]),
						hexToDec(input[5], input[6]),
						(unsigned char)0xFF
					);
					return true;
				}
				else {
					parseResult.warnings.emplace_back("Invalid #RRGGBB hex value");
					return false; // invalid hex
				}
			}
			else {
				parseResult.warnings.emplace_back("Invalid size for hex number (" + std::to_string(input.length()) + ")");
				return false; // invalid size for hex number
			}
		}
		else if (input.length() > 5 && input[0] == 'r' && input[1] == 'g' && input[2] == 'b') { // rgb/a color
			bool has_alpha = input[3] == 'a';
			int num_components = has_alpha ? 4 : 3;
			int pos = has_alpha ? 5 : 4;
			float components[4];

			for (int i = 0; i < num_components; i++) {
				if (parseNumber(input, pos, components[i], parseResult)) {
					if (pos < input.length()) {
						if (input[pos] == '%') {
							pos++; // skip %
						}
						if (input[pos] == ',') {
							pos++; // skip ,

							// next component
							if (i + 1 >= num_components) {
								parseResult.warnings.emplace_back("Too many components for rgb/a");
								return false; // too many components
							}
						}
						else if (input[pos] == ')') {
							// check if matched all components
							if (i + 1 < num_components) {
								parseResult.warnings.emplace_back("Too few components for rgb/a");
								return false; // too few components
							}

							output = buildColor(
								(unsigned char)components[0],
								(unsigned char)components[1],
								(unsigned char)components[2],
								has_alpha ? (unsigned char)((float)components[3] * 255.0f) : 0xFF
							);
							return true;
						}
					}
					else {
						parseResult.warnings.emplace_back("Incomplete rgb/a color");
						return false;
					}
				}
				else {
					parseResult.warnings.emplace_back("Could not parse component number " + std::to_string(i) + " of rgb/a color");
					return false;
				}
			}

			return false;
		}
		else {
			// try to match predefined colors
			// See https://www.w3.org/TR/css-color-3/#svg-color
			
			// BUG: for some unknown reason, in Emscripten 2.0.10
			//      the map NAMED_COLORS is empty
			// printf("size: %lu\n", NAMED_COLORS.size());

			auto it = NAMED_COLORS.find(input.str());
			if (it != NAMED_COLORS.end()) {
				output = (*it).second;
				return true;
			}
			else {
				parseResult.warnings.emplace_back("Named color '" + input.str() + "' not found");
				return false;
			}
		}

		parseResult.warnings.emplace_back("Expected color definition");
		return false;
	}

	bool parseString(const StringSection& input, String*& output, ParseResult& parseResult)
	{
		if (input.length() < 2) // must have at least two quotes
			return false;

		bool _single = input[0] == '"' || input[input.length() - 1] == '"';
		bool _double = input[0] == '\'' || input[input.length() - 1] == '\'';

		if (!_single && !_double)
			return false;

		const size_t start = 1;
        const size_t end = input.length() - 1;

		output = new std::string(input.section(start, end).str());

		return true;
	}

	bool ParseStylePropertyLine(const StringSection& line, std::vector<StyleProperty>& properties, ParseResult& parseResult)
	{
		// Example of valid property lines:
		// 
		// margin: 5px
		// margin-left: 5px
		// color: #ff00ff
		// background-color: rgba(255, 0, 255, 0.5)
		// font-family: "default"
		// justify-content: flex-start

		size_t pos = 0;
		parser::ConsumeWhiteSpace(line, pos);

		// consume a property name
		size_t property_name_start = pos;
		while (pos < line.length() && isPropertyNameChar(line[pos]))
			pos++;
		size_t property_name_end = pos;

		if (property_name_start == property_name_end) {
			parseResult.errors.push_back("Missing property name");
			return false;
		}

		parser::ConsumeWhiteSpace(line, pos);

		// next char must be :
		if (pos >= line.length() || line[pos] != ':') {
			parseResult.errors.push_back("Missing :");
			return false;
		}

		pos++; // consume :
		parser::ConsumeWhiteSpace(line, pos);

		// consume whitespace backwards
		size_t tail = line.length() - 1;
		while (tail > pos && std::isspace(line[tail]))
			tail--;

		StringSection property_name = line.section(property_name_start, property_name_end);
		StringSection raw_value = line.section(pos, tail + 1);

		using ID = StylePropertyID;

		StyleValue value = { 0 };
		ID id = ID::LAST_PROPERTY_INVALID; // for enums
		int dummy = 0; // for parseNumber
		
		switch (property_name.hash()) {

		#define PARSE_PROP(func, prop_name, prop_id, prop_key) \
		case HashStr(prop_name): \
			if (func(raw_value, value.prop_key, parseResult)) { \
				properties.emplace_back(prop_id, value); \
				return true; \
			} \
			break;

		#define LENGTH_PROPERTY(prop_name, prop_id) PARSE_PROP(parseLength, prop_name, prop_id, length);
		#define COLOR_PROPERTY(prop_name, prop_id) PARSE_PROP(parseColor, prop_name, prop_id, color);
		#define STRING_PROPERTY(prop_name, prop_id) PARSE_PROP(parseString, prop_name, prop_id, string);

		#define NUMBER_PROPERTY(prop_name, prop_id) \
		case HashStr(prop_name): \
			if (parseNumber(raw_value, dummy, value.number, parseResult)) { \
				properties.emplace_back(prop_id, value); \
				return true; \
			} \
			break;

		#define PARSE_ENUM_START(prop_name, prop_id) \
		case HashStr(prop_name): \
			id = prop_id; \
			switch (raw_value.hash()) {

		#define PARSE_ENUM_ENTRY(key, a, b) \
			case HashStr(a): value.key = b; break; \

		#define PARSE_ENUM_END() \
			default: \
				return false; /* no match */ \
			} \
			properties.emplace_back(id, value); \
			return true;

		/// -----------------------
		/// -----------------------
		/// -----------------------

		LENGTH_PROPERTY("width",      ID::WIDTH);
		LENGTH_PROPERTY("height",     ID::HEIGHT);
		LENGTH_PROPERTY("min-width",  ID::MIN_WIDTH);
		LENGTH_PROPERTY("min-height", ID::MIN_HEIGHT);
		LENGTH_PROPERTY("max-width",  ID::MAX_WIDTH);
		LENGTH_PROPERTY("max-height", ID::MAX_HEIGHT);

		LENGTH_PROPERTY("margin-left",   ID::MARGIN_LEFT);
		LENGTH_PROPERTY("margin-top",    ID::MARGIN_TOP);
		LENGTH_PROPERTY("margin-right",  ID::MARGIN_RIGHT);
		LENGTH_PROPERTY("margin-bottom", ID::MARGIN_BOTTOM);

		LENGTH_PROPERTY("padding-left",   ID::PADDING_LEFT);
		LENGTH_PROPERTY("padding-top",    ID::PADDING_TOP);
		LENGTH_PROPERTY("padding-right",  ID::PADDING_RIGHT);
		LENGTH_PROPERTY("padding-bottom", ID::PADDING_BOTTOM);

		COLOR_PROPERTY("border-color", ID::BORDER_COLOR);
		LENGTH_PROPERTY("border-top-left-radius",     ID::BORDER_TOP_LEFT_RADIUS);
		LENGTH_PROPERTY("border-top-right-radius",    ID::BORDER_TOP_RIGHT_RADIUS);
		LENGTH_PROPERTY("border-bottom-left-radius",  ID::BORDER_BOTTOM_LEFT_RADIUS);
		LENGTH_PROPERTY("border-bottom-right-radius", ID::BORDER_BOTTOM_RIGHT_RADIUS);
		LENGTH_PROPERTY("border-left-width",   ID::BORDER_LEFT_WIDTH);
		LENGTH_PROPERTY("border-top-width",    ID::BORDER_TOP_WIDTH);
		LENGTH_PROPERTY("border-right-width",  ID::BORDER_RIGHT_WIDTH);
		LENGTH_PROPERTY("border-bottom-width", ID::BORDER_BOTTOM_WIDTH);

		NUMBER_PROPERTY("flex-grow",   ID::FLEX_GROW);
		NUMBER_PROPERTY("flex-shrink", ID::FLEX_SHRINK);
		LENGTH_PROPERTY("flex-basis",  ID::FLEX_BASIS);

		PARSE_ENUM_START("flex-direction", ID::FLEX_DIRECTION);
			PARSE_ENUM_ENTRY(direction, "row",            FlexDirection::ROW);
			PARSE_ENUM_ENTRY(direction, "column",         FlexDirection::COLUMN);
			PARSE_ENUM_ENTRY(direction, "row-reverse",    FlexDirection::ROW_REVERSE);
			PARSE_ENUM_ENTRY(direction, "column-reverse", FlexDirection::COLUMN_REVERSE);
		PARSE_ENUM_END();
		PARSE_ENUM_START("flex-wrap", ID::FLEX_WRAP);
			PARSE_ENUM_ENTRY(wrap, "nowrap",       FlexWrap::NOWRAP);
			PARSE_ENUM_ENTRY(wrap, "wrap",         FlexWrap::WRAP);
			PARSE_ENUM_ENTRY(wrap, "wrap-reverse", FlexWrap::WRAP_REVERSE);
		PARSE_ENUM_END();

		#define ALIGN_ENUM_ENTRIES() \
			PARSE_ENUM_ENTRY(align, "auto",          Align::AUTO); \
			PARSE_ENUM_ENTRY(align, "flex-start",    Align::FLEX_START); \
			PARSE_ENUM_ENTRY(align, "center",        Align::CENTER); \
			PARSE_ENUM_ENTRY(align, "flex-end",      Align::FLEX_END); \
			PARSE_ENUM_ENTRY(align, "stretch",       Align::STRETCH); \
			PARSE_ENUM_ENTRY(align, "baseline",      Align::BASELINE); \
			PARSE_ENUM_ENTRY(align, "space-between", Align::SPACE_BETWEEN); \
			PARSE_ENUM_ENTRY(align, "space-around",  Align::SPACE_AROUND);

		PARSE_ENUM_START("align-self", ID::ALIGN_SELF);
			ALIGN_ENUM_ENTRIES();
		PARSE_ENUM_END();
		PARSE_ENUM_START("align-items", ID::ALIGN_ITEMS);
			ALIGN_ENUM_ENTRIES();
		PARSE_ENUM_END();
		PARSE_ENUM_START("align-content", ID::ALIGN_CONTENT);
			ALIGN_ENUM_ENTRIES();
		PARSE_ENUM_END();
		PARSE_ENUM_START("justify-content", ID::JUSTIFY_CONTENT);
			PARSE_ENUM_ENTRY(justify, "flex-start",    Justify::FLEX_START);
			PARSE_ENUM_ENTRY(justify, "center",        Justify::CENTER);
			PARSE_ENUM_ENTRY(justify, "flex-end",      Justify::FLEX_END);
			PARSE_ENUM_ENTRY(justify, "space-between", Justify::SPACE_BETWEEN);
			PARSE_ENUM_ENTRY(justify, "space-around",  Justify::SPACE_AROUND);
			PARSE_ENUM_ENTRY(justify, "space-evenly",  Justify::SPACE_EVENLY);
		PARSE_ENUM_END();

		PARSE_ENUM_START("position", ID::POSITION);
			PARSE_ENUM_ENTRY(position, "relative", Position::RELATIVE);
			PARSE_ENUM_ENTRY(position, "absolute", Position::ABSOLUTE);
		PARSE_ENUM_END();
		LENGTH_PROPERTY("left",   ID::LEFT);
		LENGTH_PROPERTY("top",    ID::TOP);
		LENGTH_PROPERTY("right",  ID::RIGHT);
		LENGTH_PROPERTY("bottom", ID::BOTTOM);

		COLOR_PROPERTY("color", ID::COLOR);
		COLOR_PROPERTY("background-color", ID::BACKGROUND_COLOR);

		PARSE_ENUM_START("overflow", ID::OVERFLOW);
			PARSE_ENUM_ENTRY(overflow, "visible", Overflow::VISIBLE);
			PARSE_ENUM_ENTRY(overflow, "hidden",  Overflow::HIDDEN);
			PARSE_ENUM_ENTRY(overflow, "scroll",  Overflow::SCROLL);
		PARSE_ENUM_END();
		PARSE_ENUM_START("display", ID::DISPLAY);
			PARSE_ENUM_ENTRY(display, "flex", Display::FLEX);
			PARSE_ENUM_ENTRY(display, "none", Display::NONE);
		PARSE_ENUM_END();

		STRING_PROPERTY("font-family", ID::FONT_FAMILY);
		LENGTH_PROPERTY("font-size", ID::FONT_SIZE);
		PARSE_ENUM_START("white-space", ID::WHITE_SPACE);
			PARSE_ENUM_ENTRY(whiteSpace, "normal", WhiteSpace::NORMAL);
			PARSE_ENUM_ENTRY(whiteSpace, "nowrap", WhiteSpace::NOWRAP);
		PARSE_ENUM_END();

		PARSE_ENUM_START("cursor", ID::CURSOR);
			PARSE_ENUM_ENTRY(cursor, "auto",        Cursor::AUTO);
			PARSE_ENUM_ENTRY(cursor, "default",     Cursor::DEFAULT);
			PARSE_ENUM_ENTRY(cursor, "none",        Cursor::NONE);
			PARSE_ENUM_ENTRY(cursor, "help",        Cursor::HELP);
			PARSE_ENUM_ENTRY(cursor, "pointer",     Cursor::POINTER);
			PARSE_ENUM_ENTRY(cursor, "progress",    Cursor::PROGRESS);
			PARSE_ENUM_ENTRY(cursor, "wait",        Cursor::WAIT);
			PARSE_ENUM_ENTRY(cursor, "crosshair",   Cursor::CROSSHAIR);
			PARSE_ENUM_ENTRY(cursor, "text",        Cursor::TEXT);
			PARSE_ENUM_ENTRY(cursor, "move",        Cursor::MOVE);
			PARSE_ENUM_ENTRY(cursor, "not-allowed", Cursor::NOT_ALLOWED);
			PARSE_ENUM_ENTRY(cursor, "all-scroll",  Cursor::ALL_SCROLL);
			PARSE_ENUM_ENTRY(cursor, "col-resize",  Cursor::COL_RESIZE);
			PARSE_ENUM_ENTRY(cursor, "row-resize",  Cursor::ROW_RESIZE);
		PARSE_ENUM_END();

		// shorthands
		#define FOUR_LENGTH_SHORTHAND(prop_name, a, b, c, d) \
		case HashStr(prop_name): \
			if (parseLength(raw_value, value.length, parseResult)) { \
				properties.emplace_back(a, value); \
				properties.emplace_back(b, value); \
				properties.emplace_back(c, value); \
				properties.emplace_back(d, value); \
				return true; \
			} \
			break;

		FOUR_LENGTH_SHORTHAND(
			"border-radius",
			ID::BORDER_TOP_LEFT_RADIUS,
			ID::BORDER_TOP_RIGHT_RADIUS,
			ID::BORDER_BOTTOM_LEFT_RADIUS,
			ID::BORDER_BOTTOM_RIGHT_RADIUS
		)
		FOUR_LENGTH_SHORTHAND(
			"border-width",
			ID::BORDER_LEFT_WIDTH,
			ID::BORDER_TOP_WIDTH,
			ID::BORDER_RIGHT_WIDTH,
			ID::BORDER_BOTTOM_WIDTH
		)
		FOUR_LENGTH_SHORTHAND(
			"margin",
			ID::MARGIN_LEFT,
			ID::MARGIN_TOP,
			ID::MARGIN_RIGHT,
			ID::MARGIN_BOTTOM
		)
		FOUR_LENGTH_SHORTHAND(
			"padding",
			ID::PADDING_LEFT,
			ID::PADDING_TOP,
			ID::PADDING_RIGHT,
			ID::PADDING_BOTTOM
		)

		// TODO: flex shorthand

		default:
			parseResult.warnings.emplace_back("Property '" + property_name.str() + "' is not supported");
			break;
		}

		return true;
	}

	bool ParseStylePropertiesBlock(const StringSection& block, std::vector<StyleProperty>& properties, ParseResult& parseResult)
	{
		bool any = false;

		size_t pos = 0;
		while (pos < block.length()) {
			any |= ParseStylePropertyLine(block.section_until(';', pos), properties, parseResult);
			pos++; // skip ,
		}

		return any;
	}

	StyleProperty::StyleProperty(const StylePropertyID id, const StyleValue& value) :
		m_ID(id),
		m_Value(value)
	{
	}

	StyleProperty::StyleProperty() :
		m_ID(StylePropertyID::LAST_PROPERTY_INVALID),
		m_Value({ 0 })
	{
	}

	StyleProperty::~StyleProperty()
	{
		switch (m_ID)
		{
		case StylePropertyID::FONT_FAMILY:
			delete m_Value.string;
			break;
		default:
			break;
		}
	}

}