#pragma once

#include <cstdint>
#include <memory>
#include <string>

namespace flexui {

	typedef std::string String;

	constexpr const uint32_t HashStrLen(const char* str, const size_t length, const size_t i = 0)
	{
		// TODO: maybe find a better hash
		return i != length ? (HashStrLen(str, length, i + 1) * 33) ^ str[i] : 5381;
	}

	constexpr const uint32_t HashStr(const char* str, const size_t i = 0)
	{
		// TODO: maybe find a better hash
		return str[i] ? (HashStr(str, i + 1) * 33) ^ str[i] : 5381;
	}

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

	class StringSection {
	public:
		StringSection(const char* input);
		StringSection(const std::string& input);

		/// Generate a subsection of this string
		StringSection section(const size_t start, const size_t end) const;
		/// Generate a subsection until the next occurrence of char
		StringSection section_until(const char c, size_t& pos) const;
		uint32_t hash() const;

		std::string str() const; // warning: string allocation
		char operator[](const size_t i) const;
		size_t length() const;
		size_t find(const char c, const size_t pos = 0) const;

	private:
		StringSection(const StringSection& input, const size_t start, const size_t end);

		std::shared_ptr<std::string> m_String;
		size_t m_Start, m_End; // [ )
	};

}