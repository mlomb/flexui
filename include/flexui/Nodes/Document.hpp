#pragma once

#include "flexui/Nodes/ContainerNode.hpp"
#include "flexui/Style/StyleEngine.hpp"
#include "flexui/Layout/LayoutEngine.hpp"
#include "flexui/Render/RenderEngine.hpp"

#include "flexui/TreeScope.hpp"

namespace flexui {

	class ResourceProvider;

	// A Document is the root for all other nodes
	class Document : public ContainerNode, public TreeScope {
	public:
		Document(ResourceProvider* resource_provider);
		~Document();

		NodeType getNodeType() const override { return NodeType::DOCUMENT; };
		StyleEngine& getStyleEngine() { return m_StyleEngine; }
		LayoutEngine& getLayoutEngine() { return m_LayoutEngine; }
		RenderEngine& getRenderEngine() { return m_RenderEngine; }
		ResourceProvider* getResourceProvider() const { return m_ResourceProvider; }

	private:
		StyleEngine m_StyleEngine;
		LayoutEngine m_LayoutEngine;
		RenderEngine m_RenderEngine;

		ResourceProvider* m_ResourceProvider;
	};

}
