#pragma once

#include "flexui/Nodes/ContainerNode.hpp"
#include "flexui/TreeScope.hpp"

namespace flexui {

	// A Document is the root for all other nodes
	class Document : public ContainerNode, public TreeScope {
	public:
		Document();
		~Document();

		NodeType getNodeType() const override { return NodeType::DOCUMENT; };
	};

}
