#include "flexui/Nodes/Document.hpp"

#include "flexui/Layout/LayoutObject.hpp"
#include "flexui/Style/StyleEngine.hpp"

namespace flexui {

	Document::Document(ResourceProvider* resource_provider, TextureProvider* texture_provider) :
		m_ResourceProvider(resource_provider),
		m_TextureProvider(texture_provider),
		m_StyleEngine(this),
		m_LayoutEngine(this),
		m_RenderEngine(this)
	{
		m_Document = this;
		m_Layout = new LayoutObject();
	}

	Document::~Document()
	{
	}

}