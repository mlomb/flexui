#include "TreeProcessor.hpp"

namespace flexui {

	TreeProcessor::TreeProcessor(Surface* surface)
		: m_Surface(surface)
	{
	}

	TreeProcessor::~TreeProcessor()
	{
	}

	void TreeProcessor::onVersionChange(Element* element, VersionChangeType changes)
	{
	}

}

