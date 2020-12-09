#pragma once

#include <string>
#include <vector>

namespace flexui {

	class Element;

	struct XMLParseResult {
		std::vector<std::string> errors;
		std::vector<std::string> warnings;
	};

	/// Parse XML source into a Element object
    Element* ParseXML(const std::string& source, XMLParseResult& parseResult);
}
