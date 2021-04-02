#include "flexui/Style/StyleSheet.hpp"

#include "flexui/Selectors/SelectorParser.hpp"

namespace flexui {

	// strip comments and consecutive spaces
	std::string sanitizeSource(const std::string& input, ParseResult& pr) {
		std::string result;
		result.reserve(input.size());

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
			pr.errors.emplace_back("Expected comment ending but found EOF");
		}

		return result;
	}

	StyleSheet* ParseStyleSheet(const std::string& raw_css, ParseResult& pr)
	{
		StringSection css = sanitizeSource(raw_css, pr);

		StyleSheet* stylesheet = new StyleSheet();
		bool any = false;

		size_t pos = 0;
		while (pos < css.length()) {
			StringSection selectors = css.section_until('{', pos);
			pos++; // skip {

			StringSection block = css.section_until('}', pos);
			pos++; // skip }

			if (selectors.length() == 0) {
				pr.errors.push_back("Expected selector");
				break;
			}
			if (block.length() == 0) {
				pr.errors.push_back("Expected properties block");
				break;
			}

			auto rule = std::make_shared<StyleRule>();

			if (ParseSelectors(selectors, rule->selectors, pr) &&
				ParseStylePropertiesBlock(block, rule->properties, pr)) {
				stylesheet->addRule(rule);
				any = true;
			}
		}

		if (!any) {
			delete stylesheet;
			stylesheet = nullptr;
		}
		return stylesheet;
	}

	StyleSheet::StyleSheet()
	{
	}

	StyleSheet::~StyleSheet()
	{
	}

	void StyleSheet::addRule(const std::shared_ptr<StyleRule>& rule)
	{
		m_Rules.emplace_back(rule);
	}

}
