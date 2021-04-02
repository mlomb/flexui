#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	struct StyleComputed;

	// Text node
	class Text : public Node {
	public:
		Text(const std::string& text = "");
		~Text();

		const std::string& getText() const { return m_Text; }
		const StyleComputed* getParentComputedStyle() const;

		std::string getNodeName() const override { return "#text"; }
		std::string getDebugInfo() const override;
		NodeType getNodeType() const override { return NodeType::TEXT; };

		void drawContent(Painter& painter) override;

	private:
		std::string m_Text;
	};

}
