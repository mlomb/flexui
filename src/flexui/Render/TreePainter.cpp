#include "flexui/Render/TreePainter.hpp"

#include "flexui/Render/Painter.hpp"
#include "flexui/Nodes/Element.hpp"
#include "flexui/Surface.hpp"

namespace flexui {

	TreePainter::TreePainter(Surface* surface)
		: TreeProcessor(surface)
	{
		m_Painter = new Painter(surface->getTextureProvider());
	}

	TreePainter::~TreePainter()
	{
		delete m_Painter;
	}

	void TreePainter::process()
	{
		m_Painter->reset();
		rec(m_Surface->getRoot());
	}

	Painter* TreePainter::getPainter() const
	{
		return m_Painter;
	}

	// this will be removed
	void TreePainter::rec(Element* el)
	{
		el->paintContent(m_Painter);

		//for (Element* e : el->getChildrens())
		//	rec(e);
	}

}
