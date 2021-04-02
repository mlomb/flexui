#pragma once

#include <vector>
#include <string>

#include "flexui/Misc/String.hpp"

namespace flexui {

	struct ParseResult {
		std::vector<std::string> errors;
		std::vector<std::string> warnings;
	};

	// Helper functions for parsers
	namespace parser {

		/// Advances pos until there is no more whitespace
		inline void ConsumeWhiteSpace(const StringSection& input, size_t& pos) {
			while (pos < input.length() && std::isspace(input[pos]))
				pos++;
		}

	}
}
