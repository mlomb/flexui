#include "flexui/Nodes/Style.hpp"

#include "flexui/Style/StyleSheet.hpp"

namespace flexui {

	Style::Style(const std::string& css) :
		m_CSSSource(css),
		m_StyleSheet(NULL)
	{
		m_StyleSheet = std::shared_ptr<StyleSheet>(ParseStyleSheet(css));
	}

	Style::~Style() {
	}

	std::string Style::getDebugInfo() const
	{
		return Node::getDebugInfo() + " StyleSheet [" + (m_StyleSheet ? std::to_string(m_StyleSheet->getRules().size()) + " rules" : "NULL") + "]";
	}

}
