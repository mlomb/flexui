#pragma once

#include <unordered_map>
#include <vector>

#include "flexui/Misc/Parser.hpp"
#include "flexui/Selectors/Selector.hpp"
#include "flexui/Style/StyleProperty.hpp"

namespace flexui {

	// A single style rule from a stylesheet
	struct StyleRule {
		std::vector<Selector> selectors;
		std::vector<StyleProperty> properties;
	};

	class StyleSheet {
	public:
		StyleSheet();
		~StyleSheet();

		void addRule(const std::shared_ptr<StyleRule>& rule);
		const std::vector<std::shared_ptr<StyleRule>>& getRules() const { return m_Rules; }

	private:
		std::vector<std::shared_ptr<StyleRule>> m_Rules;
	};

	StyleSheet* ParseStyleSheet(const std::string& css, ParseResult* pr = nullptr);

}
