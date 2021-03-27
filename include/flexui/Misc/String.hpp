#pragma once

#include <string>

namespace flexui {

	typedef std::string String;

	constexpr const uint32_t HashStr(const char* str, const int i = 0)
	{
		// TODO: maybe find a better hash
		return str[i] ? (HashStr(str, i + 1) * 33) ^ str[i] : 5381;
	}

	// Hashed strings are immutable
	struct HashedString {
		HashedString(const String& string = "") : value(string), hash(HashStr(string.c_str())) { };

		operator const String&() const { return value; }

		const String value;
		const uint32_t hash;
	};

}