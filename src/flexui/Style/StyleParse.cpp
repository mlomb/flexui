#include "flexui/Style/StyleParse.hpp"

#include "flexui/Style/NamedColors.hpp"
#include "flexui/Style/StyleSheet.hpp"

/// Eventually we want to move away from std::string.
/// We'll have slices of the original buffer
/// moving around instead of copying all the time (substrs)

namespace flexui {

	/// Check if the character is a valid start for a CSS identifier
	inline bool isIdentStart(char c) {
		return
			(c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			(c == '-') || (c == '_');
	}

	/// Check if the character is valid for a CSS identifier
	inline bool isIdent(char c) {
		return isIdentStart(c) || (c >= '0' && c <= '9');
	}

	/// Check if the character is a CSS nesting operator
	inline bool isNestingOperator(char c) {
		return c == '>' || c == '+' || c == '~';
	}

	/// Check if the character is a CSS selector marker
	inline bool isSelectorMarker(char c) {
		return c == '*' || c == '#' || c == '.' || c == ':';
	}

	/// Check if the character is a valid decimal number
	bool isNumeric(char c) {
		return c >= '0' && c <= '9';
	}

	/// Check if the character is a valid hex number
	bool isHex(char c) {
		return
			(c >= 'a' && c <= 'f') ||
			(c >= 'A' && c <= 'F') ||
			isNumeric(c);
	}

	/// Converts the hex character to its decimal representation
	unsigned char hexToDec(unsigned char c) {
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		return 0;
	}

	// Converts a two digit hex to its decimal representation
	unsigned char hexToDec(unsigned char l, unsigned char r) {
		return 16 * hexToDec(l) + hexToDec(r);
	}

	/// Converts decimal colors to StyleColor
	StyleColor buildColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
		// AABBGGRR
		return ((uint32_t)(a) << 24) | ((uint32_t)(b) << 16) | ((uint32_t)(g) << 8) | ((uint32_t)(r));
	}

	/// Advances pos until there is no more whitespace
	void consumeWhiteSpace(const std::string& input, size_t& pos) {
		while (pos < input.size() && std::isspace(input[pos]))
			pos++;
	}

	/// Parses a single CSS identifier from pos
	std::string parseIdentifier(const std::string& input, size_t& pos) {
		std::string result;
		while (pos < input.size() && isIdent(input[pos])) {
			result.push_back(input[pos]);
			pos++;
		}
		return result;
	}

	// Removes all spaces and transforms to lowercase
	std::string sanitize(const std::string& input) {
		std::string result;
		for (const char& c : input)
			if (!std::isspace(c))
				result.push_back(std::tolower(c));
		return result;
	}

	bool ParseSingleStyleSelector(const std::string& input, StyleSelector& selector, StyleParseResult& parseResult)
	{
		selector = { };
		selector.rule = nullptr;
		selector.specificity = 0;
		selector.order = 0;

		size_t pos = 0;
		StyleSelectorRelationship next_rel = StyleSelectorRelationship::NONE;

		while (pos < input.size()) {
			consumeWhiteSpace(input, pos);
			if (pos >= input.size())
				break; // no more

			char chr = input[pos];
			bool marker = isSelectorMarker(chr);

			if (isIdentStart(chr) || marker) {
				if (marker)
					pos++; // skip marker

				StyleIdentifierType type;
				bool is_wildcard = false;

				switch (chr) {
				case '#': type = StyleIdentifierType::ID; break;
				default:  type = StyleIdentifierType::TAG; break;
				case '.': type = StyleIdentifierType::CLASS; break;
				case '*': type = StyleIdentifierType::TAG; is_wildcard = true; break;
				case ':':
					std::string pseudo = parseIdentifier(input, pos);

					StylePseudoStates state;
					switch (HashStr(pseudo.c_str())) {
					case HashStr("hover"):    state = StylePseudoStates::HOVER; break;
					case HashStr("disabled"): state = StylePseudoStates::DISABLED; break;
					case HashStr("checked"):  state = StylePseudoStates::CHECKED; break;
					case HashStr("active"):   state = StylePseudoStates::ACTIVE; break;
					case HashStr("focus"):    state = StylePseudoStates::FOCUS; break;
					default:
						parseResult.warnings.emplace_back("Unsupported pseudo state '" + pseudo + "'");
						return false;
					}

					// elem:pseudo
					if (next_rel == StyleSelectorRelationship::NONE && selector.parts.size() > 0) {
						selector.parts.back().pseudo_states |= state;
					}
					// :pseudo
					// elem > :pseudo
					else {
						// create wildcard tag
						StyleSelectorPart part = { };
						part.identifier.type = StyleIdentifierType::TAG;
						part.identifier.text = "*";
						part.identifier.computeHash();
						part.prev_relationship = StyleSelectorRelationship::NONE;
						part.pseudo_states = state;
						selector.parts.emplace_back(part);
					}

					if (input[pos] == ' ') // check for space
						next_rel = StyleSelectorRelationship::DESCENDANT;
					else
						next_rel = StyleSelectorRelationship::NONE;

					continue; // skip the rest
				}

				StyleSelectorPart part = { };
				part.identifier.type = type;
				part.identifier.text = is_wildcard ? "*" : parseIdentifier(input, pos);
				part.identifier.computeHash();
				part.prev_relationship = next_rel;
				part.pseudo_states = StylePseudoStates::NONE;

				// OE_LOG_DEBUG("(selector type " << std::to_string((int)part.identifier.type) << ")" << part.identifier.text);

				selector.parts.emplace_back(part);

				if (input[pos] == ' ') // check for space
					next_rel = StyleSelectorRelationship::DESCENDANT;
				else
					next_rel = StyleSelectorRelationship::NONE;
			}
			else if (isNestingOperator(chr)) {
				pos++; // skip operator

				switch (chr) {
				case '>': next_rel = StyleSelectorRelationship::CHILD; break;
				case '+': next_rel = StyleSelectorRelationship::ADJACENT_SIBLING; break;
				case '~': next_rel = StyleSelectorRelationship::GENERAL_SIBLING; break;
				}

				// OE_LOG_DEBUG("(nesting op)" << chr);
			}
			else {
				parseResult.errors.emplace_back("Unexpected character '" + std::string(1, chr) + "'");
				return false;
			}
		}

		if (selector.parts.size() > 0) {
			selector.computeSpecificity();
			return true;
		}
		else {
			return false;
		}
	}

	bool ParseStyleSelectors(const std::string& input, std::vector<StyleSelector>& selectors, StyleParseResult& parseResult)
	{
		bool any = false;

		size_t pos = 0;
		while (pos < input.size()) {
			size_t selector_end = input.find(",", pos);
			if (selector_end == std::string::npos)
				selector_end = input.size();

			std::string single_selector = input.substr(pos, selector_end - pos);

			StyleSelector selector;
			if (ParseSingleStyleSelector(single_selector, selector, parseResult)) {
				selectors.emplace_back(selector);
				any = true;
			}

			pos = selector_end + 1;
		}

		return any;
	}

	bool parseNumber(const std::string& input, int& pos, StyleNumber& output, StyleParseResult& parseResult) {
		bool valid = false;
		bool negative = false;
		bool mantissa = false;
		int mantissa_place = 1;
		float value = 0;

		while (pos < input.size()) {
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

	bool parseLength(const std::string& input, StyleLength& output, StyleParseResult& parseResult) {
		int pos = 0;
		if (parseNumber(input, pos, output.number, parseResult)) {
			// number parsed
			if (pos < input.size()) {
				// more to read, must be a unit

				if (input[pos] == '%') {
					output.unit = StyleLengthUnit::PERCENT;
				}
				else if (pos + 1 < input.size() && input[pos] == 'p' && input[pos + 1] == 'x') {
					output.unit = StyleLengthUnit::PIXELS;
				}
				else {
					// invalid unit
					parseResult.warnings.emplace_back("Unexpected character '" + std::string(1, input[pos]) + "' parsing length");
					return false; // unexpected character
				}
			}
			else {
				// by default pixels
				output.unit = StyleLengthUnit::PIXELS;
			}

			output.set_auto = false;
			return true;
		}
		else if (input.size() >= 4) {
			// check if auto
			if (input[0] == 'a' && input[1] == 'u' && input[2] == 't' && input[3] == 'o') {
				output.set_auto = true;
				return true;
			}
		}

		return false;
	}

	bool parseColor(const std::string& input, StyleColor& output, StyleParseResult& parseResult) {
		if (input.size() < 2)
			return false;

		if (input[0] == '#') { // hex color
			// #RGB
			if (input.size() == 4) {
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
			else if (input.size() == 7) {
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
				parseResult.warnings.emplace_back("Invalid size for hex number (" + std::to_string(input.size()) + ")");
				return false; // invalid size for hex number
			}
		}
		else if (input.find("rgb") == 0 && input.size() > 5) { // rgb/a color
			bool has_alpha = input[3] == 'a';
			int num_components = has_alpha ? 4 : 3;
			int pos = has_alpha ? 5 : 4;
			float components[4];

			for (int i = 0; i < num_components; i++) {
				if (parseNumber(input, pos, components[i], parseResult)) {
					if (pos < input.size()) {
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
					parseResult.warnings.emplace_back("Couldn't parse component number " + std::to_string(i) + " of rgb/a color");
					return false;
				}
			}

			return false;
		}
		else {
			// try to match predefined colors
			// See https://www.w3.org/TR/css-color-3/#svg-color
			auto it = NAMED_COLORS.find(input);
			if (it != NAMED_COLORS.end()) {
				const auto& color = (*it).second;
				output = buildColor(
					(unsigned char)std::get<0>(color),
					(unsigned char)std::get<1>(color),
					(unsigned char)std::get<2>(color),
					(unsigned char)((float)std::get<3>(color) * 255.0f)
				);
				return true;
			}
		}

		parseResult.warnings.emplace_back("Expected color definition");

		return false;
	}

	bool ParseStyleProperty(const std::string& name, const std::string& raw_value, StyleRule& rule, StyleParseResult& parseResult)
	{
		using ID = StylePropertyID;

		StyleProperty prop = { };

		std::string value = sanitize(raw_value);

		int dummy = 0; // for parseNumber

		switch (HashStr(sanitize(name).c_str())) {
		#define PARSE_PROP(func, prop_name, prop_id, prop_key) \
		case HashStr(prop_name): \
			if (func(value, prop.value.prop_key, parseResult)) { \
				prop.id = prop_id; \
				rule.properties.emplace_back(prop); \
				return true; \
			} \
			break;

		#define LENGTH_PROPERTY(prop_name, prop_id) PARSE_PROP(parseLength, prop_name, prop_id, length);
		#define COLOR_PROPERTY(prop_name, prop_id) PARSE_PROP(parseColor, prop_name, prop_id, color);

		#define NUMBER_PROPERTY(prop_name, prop_id) \
		case HashStr(prop_name): \
			if (parseNumber(value, dummy, prop.value.number, parseResult)) { \
				prop.id = prop_id; \
				rule.properties.emplace_back(prop); \
				return true; \
			} \
			break;

		#define PARSE_ENUM_START(prop_name, prop_id) \
		case HashStr(prop_name): \
			prop.id = prop_id; \
			switch (HashStr(value.c_str())) {

		#define PARSE_ENUM_ENTRY(key, a, b) \
			case HashStr(a): prop.value.key = b; break; \

		#define PARSE_ENUM_END() \
			default: \
				return false; /* no match */ \
			} \
			rule.properties.emplace_back(prop); \
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

		LENGTH_PROPERTY("font-size", ID::FONT_SIZE);
		PARSE_ENUM_START("white-space", ID::WHITE_SPACE);
			PARSE_ENUM_ENTRY(whiteSpace, "normal", WhiteSpace::NORMAL);
			PARSE_ENUM_ENTRY(whiteSpace, "nowrap", WhiteSpace::NOWRAP);
		PARSE_ENUM_END();
		
		PARSE_ENUM_START("cursor", ID::CURSOR);
			PARSE_ENUM_ENTRY(cursor, "auto",        StyleCursor::AUTO);
			PARSE_ENUM_ENTRY(cursor, "default",     StyleCursor::DEFAULT);
			PARSE_ENUM_ENTRY(cursor, "none",        StyleCursor::NONE);
			PARSE_ENUM_ENTRY(cursor, "help",        StyleCursor::HELP);
			PARSE_ENUM_ENTRY(cursor, "pointer",     StyleCursor::POINTER);
			PARSE_ENUM_ENTRY(cursor, "progress",    StyleCursor::PROGRESS);
			PARSE_ENUM_ENTRY(cursor, "wait",        StyleCursor::WAIT);
			PARSE_ENUM_ENTRY(cursor, "crosshair",   StyleCursor::CROSSHAIR);
			PARSE_ENUM_ENTRY(cursor, "text",        StyleCursor::TEXT);
			PARSE_ENUM_ENTRY(cursor, "move",        StyleCursor::MOVE);
			PARSE_ENUM_ENTRY(cursor, "not-allowed", StyleCursor::NOT_ALLOWED);
			PARSE_ENUM_ENTRY(cursor, "all-scroll",  StyleCursor::ALL_SCROLL);
			PARSE_ENUM_ENTRY(cursor, "col-resize",  StyleCursor::COL_RESIZE);
			PARSE_ENUM_ENTRY(cursor, "row-resize",  StyleCursor::ROW_RESIZE);
		PARSE_ENUM_END();

		// shorthands
		#define FOUR_LENGTH_SHORTHAND(prop_name, a, b, c, d) \
		case HashStr(prop_name): \
			if (parseLength(value, prop.value.length, parseResult)) { \
				prop.id = a; \
				rule.properties.emplace_back(prop); \
				prop.id = b; \
				rule.properties.emplace_back(prop); \
				prop.id = c; \
				rule.properties.emplace_back(prop); \
				prop.id = d; \
				rule.properties.emplace_back(prop); \
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
			parseResult.warnings.emplace_back("Property '" + name + "' is not supported");
			break;
		}

		return false;
	}

	// strip comments and consecutive spaces
	std::string sanitizeSource(const std::string& input, StyleParseResult& parseResult) {
		std::string result;

		bool before_whitespace = false;
		bool inside_comment = false;
		for (int i = 0; i < input.size(); i++) {
			char chr = input[i];
			// check if next is closing comment
			if (chr == '*' && i + 1 < input.size() && input[i + 1] == '/') {
				i += 2;
				inside_comment = false;
			}
			else if (inside_comment) {
				// ignore character
			}
			else if (std::isspace(chr)) {
				if (!before_whitespace) {
					result.push_back(' ');
					before_whitespace = true;
				}
			}
			// check if next is opening comment
			else if (chr == '/' && i + 1 < input.size() && input[i + 1] == '*') {
				i += 2;
				inside_comment = true;
			}
			else {
				result.push_back(chr);
				before_whitespace = false;
			}
		}

		if (inside_comment) {
			parseResult.errors.emplace_back("Expected comment ending but found EOF");
		}

		return result;
	}

	bool parseRule(const std::string& source, StyleRule& rule, StyleParseResult& parseResult) {
		rule.properties.clear();

		size_t pos = 0;
		do {
			consumeWhiteSpace(source, pos);
			if (pos >= source.size())
				break; // done
			if (isIdentStart(source[pos])) {
				// name
				std::string name = parseIdentifier(source, pos);
				consumeWhiteSpace(source, pos);
				if (source[pos] == ':') {
					pos++; // skip :
				}
				else {
					parseResult.errors.emplace_back("Expected ':'");
					return false;
				}

				// value
				consumeWhiteSpace(source, pos);
				auto value_end = source.find_first_of(";}", pos);
				if (value_end == std::string::npos)
					value_end = source.size() - 1; // to the end

				std::string value = source.substr(pos, value_end - pos);
				pos = value_end + 1;

				// OE_LOG_DEBUG("  (property)" << name << ":" << value);

				// parse property
				ParseStyleProperty(name, value, rule, parseResult);
			}
			else if (source[pos] == '}') {
				pos++; // skip }
				break; // stop
			}
			else {
				parseResult.errors.emplace_back("Expected identifier but found '" + std::string(1, source[pos]) + "'");
				// try to skip to the next property
				pos = source.find_first_of(";}", pos);
				if (pos == std::string::npos)
					return false; // EOF
				if (source[pos] == ';')
					pos++; // skip ;
				else // source[pos] == '}'
					return rule.properties.size() > 0;
			}
		} while (pos < source.size());

		return rule.properties.size() > 0;
	}

	StyleSheet* ParseStyleSheet(const std::string& raw_source, StyleParseResult& parseResult)
	{
		parseResult.errors.clear();
		parseResult.warnings.clear();

		StyleSheet* sheet = new StyleSheet();

		std::string source = sanitizeSource(raw_source, parseResult);

		size_t pos = 0;
		while (pos < source.size()) {
			size_t block_start = source.find("{", pos);
			if (block_start == std::string::npos)
				break; // no more blocks
			size_t block_end = source.find("}", block_start);
			if (block_end == std::string::npos) {
				parseResult.errors.emplace_back("Expected block ending but found EOF");
				break;
			}

			std::string selectors_str = source.substr(pos, block_start - pos);
			std::string properties_str = source.substr(block_start + 1, block_end - block_start - 1);

			std::vector<StyleSelector> selectors;
			if (ParseStyleSelectors(selectors_str, selectors, parseResult)) {
				StyleRule rule = { };
				if (parseRule(properties_str, rule, parseResult)) {
					if (rule.properties.size() > 0) { // only add if not empty
						int rule_ref = sheet->addRule(rule);

						// connect selectors
						for (StyleSelector& selector : selectors) {
							selector.computeSpecificity();
							sheet->addSelector(selector, rule_ref);
						}
					}
				}
			}

			pos = block_end + 1;
		}

		return sheet;
	}

}
