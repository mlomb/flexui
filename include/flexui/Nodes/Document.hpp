#pragma once

#include "flexui/Nodes/ContainerNode.hpp"
#include "flexui/Style/StyleEngine.hpp"
#include "flexui/Layout/LayoutEngine.hpp"
#include "flexui/Render/RenderEngine.hpp"

namespace flexui {

	class ResourceProvider;
	class TextureProvider;

	// A Document is the root for all other nodes
	class Document : public ContainerNode {
	public:
		Document(ResourceProvider* resource_provider, TextureProvider* texture_provider);
		~Document() override;

		NodeType getNodeType() const override { return NodeType::DOCUMENT; };
		StyleEngine& getStyleEngine() { return m_StyleEngine; }
		LayoutEngine& getLayoutEngine() { return m_LayoutEngine; }
		RenderEngine& getRenderEngine() { return m_RenderEngine; }
		ResourceProvider* getResourceProvider() const { return m_ResourceProvider; }
		TextureProvider* getTextureProvider() const { return m_TextureProvider; }

	private:
		StyleEngine m_StyleEngine;
		LayoutEngine m_LayoutEngine;
		RenderEngine m_RenderEngine;

		ResourceProvider* m_ResourceProvider;
		TextureProvider* m_TextureProvider;
	};

}
