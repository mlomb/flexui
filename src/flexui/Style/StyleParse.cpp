#include "flexui/Style/StyleParse.hpp"

#include "flexui/Style/NamedColors.hpp"
#include "flexui/Style/StyleSheet.hpp"
#include "flexui/Style/StyleRule.hpp"

#include "flexui/Selectors/SelectorParser.hpp"

#include <cmath>
#include <cstring>  // memcpy

/// Eventually we want to move away from std::string.
/// We'll have slices of the original buffer
/// moving around instead of copying all the time (substrs)

namespace flexui {


	// strip comments and consecutive spaces
	std::string sanitizeSource(const std::string& input, ParseResult& parseResult) {
		std::string result;

		bool before_whitespace = false;
		bool inside_comment = false;
		for (int i = 0; i < input.size(); i++) {
			unsigned char chr = input[i];
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

	bool parseRule(const std::string& source, StyleRule& rule, ParseResult& parseResult) {
		/*
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
				// TODO: allow for ; and } inside quotes
				consumeWhiteSpace(source, pos);
				auto value_end = source.find_first_of(";}", pos);
				if (value_end == std::string::npos)
					value_end = source.size() - 1; // to the end

				// go back to the first non-space
				size_t last_non_space = value_end - 1;
				while (last_non_space > pos && std::isspace(source[last_non_space]))
					last_non_space--;

				std::string value = source.substr(pos, last_non_space - pos + 1);
				pos = value_end + 1;

				// FUI_LOG_DEBUG("  (property)" << name << ":" << value);

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
		*/
		return false;
	}

	StyleSheet* ParseStyleSheet(const std::string& raw_source, ParseResult& parseResult)
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

			std::vector<Selector> selectors;
			if (ParseSelectors(selectors_str, selectors, parseResult)) {
				/*StyleRule rule = { };
				if (parseRule(properties_str, rule, parseResult)) {
					if (rule.properties.size() > 0) { // only add if is has at least one property
						int rule_ref = sheet->addRule(rule);

						// connect selectors
						for (StyleSelector& selector : selectors) {
							selector.computeSpecificity();
							sheet->addSelector(selector, rule_ref);
						}
					}
				}*/
			}

			pos = block_end + 1;
		}

		return sheet;
	}

}
