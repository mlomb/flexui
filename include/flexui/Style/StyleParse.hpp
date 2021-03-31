#pragma once

#include <vector>
#include <string>

#include "StyleDefinitions.hpp"
#include "flexui/Misc/Parser.hpp"

namespace flexui {

	class StyleSheet;

	/// Parse CSS source into a StyleSheet object
	/// The parser doesn't support any kind of escaping
	StyleSheet* ParseStyleSheet(const std::string& source, ParseResult& parseResult);
}
