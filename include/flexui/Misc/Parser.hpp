#pragma once

#include <vector>
#include <string>

namespace flexui {

	struct ParseResult {
		std::vector<std::string> errors;
		std::vector<std::string> warnings;
	};

	// Helper functions for parsers
	namespace parser {

		/// Advances pos until there is no more whitespace
		inline void ConsumeWhiteSpace(const std::string& input, size_t& pos) {
			while (pos < input.size() && std::isspace(input[pos]))
				pos++;
		}

	}
}
