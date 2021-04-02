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

		/// Check if the character is a valid decimal digit
		inline bool IsNumeric(char c) {
			return c >= '0' && c <= '9';
		}

		/// Check if the character is a valid hex digit
		inline bool IsHex(char c) {
			return
				(c >= 'a' && c <= 'f') ||
				(c >= 'A' && c <= 'F') ||
				IsNumeric(c);
		}

		/// Converts a hex digit to its decimal representation
		inline unsigned char HexToDec(unsigned char c) {
			if (c >= '0' && c <= '9') return c - '0';
			if (c >= 'a' && c <= 'f') return c - 'a' + 10;
			if (c >= 'A' && c <= 'F') return c - 'A' + 10;
			return 0;
		}

		/// Converts a two digit hex to its decimal representation
		inline unsigned char HexToDec(unsigned char l, unsigned char r) {
			return 16 * HexToDec(l) + HexToDec(r);
		}

	}
}
