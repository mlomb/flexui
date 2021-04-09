#pragma once

#include <string>
#include <memory>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	class StyleSheet;

	// Style node
	class Style : public Node {
	public:
		Style(const std::string& css = "");
		~Style() override;

		std::shared_ptr<StyleSheet> getStyleSheet() const { return m_StyleSheet; }

		std::string getNodeName() const override { return "#style"; }
		std::string getDebugInfo() const override;
		NodeType getNodeType() const override { return NodeType::STYLE; };

	private:
		std::string m_CSSSource;
		std::shared_ptr<StyleSheet> m_StyleSheet;
	};

}
