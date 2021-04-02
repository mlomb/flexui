#include "flexui/Selectors/SelectorParser.hpp"

namespace flexui {

	/// Check if the character is a CSS selector marker
	inline bool isSelectorMarker(char c) {
		return c == '*' || c == '#' || c == '.' || c == ':';
	}

	/// Check if the character is a CSS nesting operator
	inline bool isNestingOperator(char c) {
		return c == '>' || c == '+' || c == '~';
	}

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

	/// Parses a single CSS identifier from pos
	inline HashedString parseIdentifier(const StringSection& input, size_t& pos) {
		size_t start_abs = pos;
		while (pos < input.length() && isIdent(input[pos]))
			pos++;
		return input.section(start_abs, pos).str();
	}

    bool ParseSingleSelector(const StringSection& input_selector, Selector& selector, ParseResult& pr)
    {
		selector.clear();

		size_t pos = 0;
		SelectorRelationship next_rel = SelectorRelationship::NONE;

		while (pos < input_selector.length()) {
			parser::ConsumeWhiteSpace(input_selector, pos);
			if (pos >= input_selector.length())
				break; // no more

			char chr = input_selector[pos];
			bool marker = isSelectorMarker(chr);

			if (isIdentStart(chr) || marker) {
				if (marker)
					pos++; // skip marker

				SelectorIdentifierType type;
				bool is_wildcard = false;

				switch (chr) {
				case '#': type = SelectorIdentifierType::ID; break;
				default:  type = SelectorIdentifierType::TAG; break;
				case '*': type = SelectorIdentifierType::TAG; is_wildcard = true; break;
				case '.': type = SelectorIdentifierType::CLASS; break;
				case ':':
					// check for unsupported pseudo-elements (::)
					if (pos < input_selector.length() && input_selector[pos] == ':') {
						pr.warnings.emplace_back("Pseudo-elements are not supported");
						return false;
					}

					HashedString pseudo = parseIdentifier(input_selector, pos);

					PseudoStates state;
					switch (pseudo.hash()) {
					case HashStr("hover"):    state = PseudoStates::HOVER; break;
					case HashStr("disabled"): state = PseudoStates::DISABLED; break;
					case HashStr("checked"):  state = PseudoStates::CHECKED; break;
					case HashStr("active"):   state = PseudoStates::ACTIVE; break;
					case HashStr("focus"):    state = PseudoStates::FOCUS; break;
					default:
						pr.warnings.emplace_back("Unsupported pseudo state '" + pseudo.str() + "'");
						return false;
					}

					// elem:pseudo
					if (next_rel == SelectorRelationship::NONE && selector.size() > 0) {
						selector.back().pseudos |= state;
					}
					// :pseudo
					// elem > :pseudo
					else {
						// create wildcard tag
						SelectorPart part = { };
						part.identifier.type = SelectorIdentifierType::TAG;
						part.identifier.value = "*";
						part.prev_rel = SelectorRelationship::NONE;
						part.pseudos = state;
						selector.emplace_back(part);
					}

					if (pos < input_selector.length() && input_selector[pos] == ' ') // check for space
						next_rel = SelectorRelationship::DESCENDANT;
					else
						next_rel = SelectorRelationship::NONE;

					continue; // skip the rest
				}

				SelectorPart part = { };
				part.identifier.type = type;
				part.identifier.value = is_wildcard ? "*" : parseIdentifier(input_selector, pos);
				part.prev_rel = next_rel;
				part.pseudos = PseudoStates::NONE;

				// FUI_LOG_DEBUG("(selector type " << std::to_string((int)part.identifier.type) << ")" << part.identifier.text);

				selector.emplace_back(part);

				if (pos < input_selector.length() && input_selector[pos] == ' ') // check for space
					next_rel = SelectorRelationship::DESCENDANT;
				else
					next_rel = SelectorRelationship::NONE;
			}
			else if (isNestingOperator(chr)) {
				pos++; // skip operator

				switch (chr) {
				case '>': next_rel = SelectorRelationship::CHILD; break;
				case '+': next_rel = SelectorRelationship::ADJACENT_SIBLING; break;
				case '~': next_rel = SelectorRelationship::GENERAL_SIBLING; break;
				}

				// FUI_LOG_DEBUG("(nesting op)" << chr);
			}
			else if (chr == '[') {
				pr.warnings.emplace_back("Attribute selectors are not supported");
				return false;
			}
			else {
				pr.errors.emplace_back("Unexpected character '" + std::string(1, chr) + "'");
				return false;
			}
		}

		return selector.size();
    }

    bool ParseSelectors(const StringSection& input_selectors, std::vector<Selector>& selectors, ParseResult& pr)
    {
		// we have to track if we add a new selector
		// we can't use selectors.size() because the caller
		// may have passed a non empty vector
		bool any = false;

		size_t pos = 0;
		while (pos < input_selectors.length()) {
			auto single_selector = input_selectors.section_until(',', pos);

			Selector selector;
			if (ParseSingleSelector(single_selector, selector, pr)) {
				selectors.emplace_back(selector);
				any = true;
			}

			pos++; // skip ,
		}

		return any;
    }

}
