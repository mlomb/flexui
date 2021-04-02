#include "flexui/Misc/String.hpp"

#include "flexui/Log.hpp"

namespace flexui {

	StringSection::StringSection(const char* input) :
		m_String(std::make_shared<std::string>(input)),
		m_Start(0),
		m_End(strlen(input))
	{
		FUI_ASSERT(input);
	}

	StringSection::StringSection(const std::string& input) :
		m_String(std::make_shared<std::string>(input)),
		m_Start(0),
		m_End(input.size())
	{
	}

	StringSection::StringSection(const StringSection& input, const size_t start, const size_t end) :
		m_String(input.m_String),
		m_Start(input.m_Start + start),
		m_End(input.m_Start + end)
	{
		FUI_ASSERT(start <= end);
		FUI_ASSERT(m_End <= m_String->length());
	}

	StringSection StringSection::section(const size_t start, const size_t end) const
	{
		FUI_ASSERT(start <= end);
		return StringSection(*this, start, end);
	}

	StringSection StringSection::section_until(const char c, size_t& pos) const
	{
		size_t start_abs = m_Start + pos;
		size_t end_abs = m_String->find(c, start_abs);
		if (end_abs == std::string::npos)
			end_abs = m_String->length();

		pos = end_abs - m_Start;
		return section(start_abs - m_Start, pos);
	}

	uint32_t StringSection::hash() const
	{
		return HashStrLen(m_String->c_str() + m_Start, length());
	}

	String StringSection::str() const
	{
		return m_String->substr(m_Start, length());
	}

	char StringSection::operator[](const size_t i) const
	{
		FUI_ASSERT(i < length());
		return m_String->at(m_Start + i);
	}

	size_t StringSection::length() const
	{
		return m_End - m_Start;
	}

	size_t StringSection::find(char c, size_t pos) const
	{
		return m_String->find(c, m_Start + pos);
	}

}