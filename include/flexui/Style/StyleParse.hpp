#pragma once

#include <vector>
#include <string>

#include "StyleDefinitions.hpp"
#include "flexui/Misc/Parser.hpp"

namespace flexui {

	class StyleSheet;

	bool ParseStyleProperty(const std::string& name, const std::string& value, StyleRule& rule, ParseResult& parseResult);

	/// Parse CSS source into a StyleSheet object
	/// The parser doesn't support any kind of escaping
	StyleSheet* ParseStyleSheet(const std::string& source, ParseResult& parseResult);
}
