#pragma once

#include <string>

#include "flexui/Selectors/Selector.hpp"
#include "flexui/Misc/Parser.hpp"

namespace flexui {

	/// Parse a single selector
	/// The input string must have no comments
	/// Returns true if the selector was sucessfully parsed
	bool ParseSingleSelector(const std::string& input_selector, Selector& selector, ParseResult& parseResult);

	/// Parse multiple selectors (delimited by ',')
	/// Note that this function will not clear the selectors vector
	/// The input string must have no comments
	/// Returns true if at least one selector was parsed
	bool ParseSelectors(const std::string& input_selectors, std::vector<Selector>& selectors, ParseResult& parseResult);

}
