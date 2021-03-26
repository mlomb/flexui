#pragma once

#include <string>

#include "flexui/Nodes/Node.hpp"

namespace flexui {

	class StyleComputed;

	// Text node
	class Text : public Node {
	public:
		Text(const std::string& text = "");
		~Text();

		const std::string& getText() const { return m_Text; }
		const StyleComputed* getParentComputedStyle() const;
		NodeType getNodeType() const override { return NodeType::TEXT; };

		void drawContent(Painter* painter) override;

	private:
		std::string m_Text;
	};

}
