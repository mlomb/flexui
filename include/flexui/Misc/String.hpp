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
		HashedString(const String& string = "") : _value(string), _hash(HashStr(string.c_str())) { };
		HashedString(const char* string) : _value(string), _hash(HashStr(string)) { };

		operator const String&() const { return _value; }

		const String& str() const { return _value; }
		const uint32_t hash() const { return _hash; }
		
		bool operator ==(const HashedString& oth) const { return _hash == oth._hash; }

	private:
		String _value;
		uint32_t _hash;
	};

}