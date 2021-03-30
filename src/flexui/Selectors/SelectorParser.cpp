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

    bool ParseSingleSelector(const std::string& input_selector, Selector& selector, ParseResult& parseResult)
    {
		selector.clear();

		size_t pos = 0;
		SelectorRelationship next_rel = SelectorRelationship::NONE;

		while (pos < input_selector.size()) {
			consumeWhiteSpace(input_selector, pos);
			if (pos >= input_selector.size())
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
				case '.': type = SelectorIdentifierType::CLASS; break;
				case '*': type = SelectorIdentifierType::TAG; is_wildcard = true; break;
				case ':':
					// check for unsupported pseudo-elements
					if (pos < input_selector.size() && input_selector[pos] == ':') {
						parseResult.warnings.emplace_back("Pseudo-elements are not supported");
						return false;
					}

					std::string pseudo = parseIdentifier(input_selector, pos);

					SelectorPseudoState state;
					switch (HashStr(pseudo.c_str())) {
					case HashStr("hover"):    state = SelectorPseudoState::HOVER; break;
					case HashStr("disabled"): state = SelectorPseudoState::DISABLED; break;
					case HashStr("checked"):  state = SelectorPseudoState::CHECKED; break;
					case HashStr("active"):   state = SelectorPseudoState::ACTIVE; break;
					case HashStr("focus"):    state = SelectorPseudoState::FOCUS; break;
					default:
						parseResult.warnings.emplace_back("Unsupported pseudo state '" + pseudo + "'");
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

					if (input_selector[pos] == ' ') // check for space
						next_rel = SelectorRelationship::DESCENDANT;
					else
						next_rel = SelectorRelationship::NONE;

					continue; // skip the rest
				}

				SelectorPart part = { };
				part.identifier.type = type;
				part.identifier.value = is_wildcard ? "*" : parseIdentifier(input_selector, pos);
				part.prev_rel = next_rel;
				part.pseudos = SelectorPseudoState::NONE;

				// FUI_LOG_DEBUG("(selector type " << std::to_string((int)part.identifier.type) << ")" << part.identifier.text);

				selector.emplace_back(part);

				if (input_selector[pos] == ' ') // check for space
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
				parseResult.warnings.emplace_back("Attribute selectors are not supported");
				return false;
			}
			else {
				parseResult.errors.emplace_back("Unexpected character '" + std::string(1, chr) + "'");
				return false;
			}
		}

		return selector.size();
    }

    bool ParseSelectors(const std::string& input_selectors, std::vector<Selector>& selectors, ParseResult& parseResult)
    {
		// we have to track if we add a new selector
		// we can't use selectors.size() because the caller
		// may have passed a non empty vector
		bool any = false;

		size_t pos = 0;
		while (pos < input_selectors.size()) {
			size_t selector_end = input_selectors.find(",", pos);
			if (selector_end == std::string::npos)
				selector_end = input_selectors.size();

			std::string single_selector = input_selectors.substr(pos, selector_end - pos);

			Selector selector;
			if (ParseSingleSelector(single_selector, selector, parseResult)) {
				selectors.emplace_back(selector);
				any = true;
			}

			pos = selector_end + 1;
		}

		return any;
    }

}
