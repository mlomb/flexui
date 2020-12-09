#include "flexui/Surface.hpp"

#include "flexui/Element.hpp"
#include "flexui/Layout/LayoutTreeUpdater.hpp"
#include "flexui/Style/StyleComputed.hpp"
#include "flexui/Style/StyleTreeUpdater.hpp"
#include "flexui/Render/TreePainter.hpp"
#include "flexui/Events/EventsController.hpp"

namespace flexui {

    Surface::Surface(ResourceProvider* rp, TextureProvider* tp)
        : m_Size({ 100, 100 })
        , m_TextureProvider(tp)
        , m_ResourceProvider(rp)
    {
        m_LayoutUpdater = new LayoutTreeUpdater(this);
        m_StyleTreeUpdater = new StyleTreeUpdater(this);
        m_TreePainter = new TreePainter(this);
        m_EventsController = new EventsController(this);

        m_Root = new Element();
        m_Root->m_Depth = 1;
        m_Root->m_Surface = this;
    }

	Surface::~Surface()
    {
		delete m_LayoutUpdater;
        delete m_StyleTreeUpdater;
        delete m_TreePainter;
        delete m_EventsController;
		delete m_Root;
	}

	void Surface::process()
	{
		m_EventsController->process();
		m_StyleTreeUpdater->process();
		m_LayoutUpdater->process();
		m_TreePainter->process();
	}

	Element* Surface::findElementsAt(Element* root, const Vec2& point, std::vector<Element*>* found)
	{
		if (!root->isVisible())
			return nullptr;

		const auto& bbox = root->getBoundingRect();

		if (bbox.contains(point)) {
			if (found)
				found->push_back(root);

			const auto& childs = root->getChildrens();

			// iterate in reverse order (z-index)
			for (auto it = childs.rbegin(); it != childs.rend(); it++) {
				Element* result = findElementsAt(*it, point, found);
				if (result)
					return result;
			}
			return root;
		}

		// no hit
		return nullptr;
	}

    StyleCursor Surface::getCurrentCusor() const
	{
        Element* underCursor = m_EventsController->getElementOverMouse();
        return underCursor ? underCursor->m_ComputedStyle->cursor.value : StyleCursor::AUTO;
    }

	Vec2 Surface::getSize() const
	{
		return m_Size;
	}

	Painter* Surface::getPainter() const
	{
		return m_TreePainter->getPainter();
	}

	TextureProvider* Surface::getTextureProvider() const
	{
		return m_TextureProvider;
	}

	ResourceProvider* Surface::getResourceProvider() const
	{
		return m_ResourceProvider;
    }

    EventsController* Surface::getEventsController() const
    {
        return m_EventsController;
    }

	Element* Surface::getRoot() const
	{
		return m_Root;
	}

	void Surface::setSize(const Vec2& size)
	{
		m_Size = size;
	}

}
