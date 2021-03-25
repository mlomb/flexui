#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	// Text node
	class Text : public Node {
	public:
		Text(const std::string& text = "");
		~Text();

		NodeType getNodeType() const override { return NodeType::TEXT; };

	private:
		std::string m_Text;
	};

}
