#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	// Comment node
	class Comment : public Node {
	public:
		Comment(const std::string& text = "");
		~Comment() override;

		std::string getNodeName() const override { return "#comment"; }
		std::string getDebugInfo() const override;
		NodeType getNodeType() const override { return NodeType::COMMENT; };

	private:
		std::string m_Text;
	};

}
