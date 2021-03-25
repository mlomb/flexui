#pragma once

#include <string>
#include <vector>

namespace flexui {

	class Node;

	struct XMLParseResult {
		std::vector<std::string> errors;
		std::vector<std::string> warnings;
	};

	/// Parse XML source into a Node object
    Node* ParseXML(const std::string& source, XMLParseResult& parseResult);
}
