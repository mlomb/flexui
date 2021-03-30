#pragma once

#include <vector>
#include <string>

#include "flexui/Selectors/Selector.hpp"
#include "flexui/Style/StyleProperty.hpp"

namespace flexui {

	// A single rule from a stylesheet
	class StyleRule {
	public:
		StyleRule();
		~StyleRule();


	private:
		std::vector<Selector> selectors;
		std::vector<StyleProperty> properties;
	};
	

	StyleRule ParseRule(const std::string& raw_input, ParseResult& parseResult);

}
