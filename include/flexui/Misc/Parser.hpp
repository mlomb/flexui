#pragma once

#include <vector>
#include <string>

namespace flexui {

	struct ParseResult {
		std::vector<std::string> errors;
		std::vector<std::string> warnings;
	};

}
