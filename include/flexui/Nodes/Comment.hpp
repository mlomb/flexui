#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	// Comment node
	class Comment : public Node {
	public:
		Comment(const std::string& text = "");
		~Comment() override;

		NodeType getNodeType() const override { return NodeType::COMMENT; };

	private:
		std::string m_Text;
	};

}
