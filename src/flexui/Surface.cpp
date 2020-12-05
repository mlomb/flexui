#include "flexui/Surface.hpp"

#include "flexui/Layout/LayoutTreeUpdater.hpp"
#include "flexui/Style/StyleTreeUpdater.hpp"
#include "flexui/Render/TreePainter.hpp"

namespace flexui {

	Surface::Surface(ResourceProvider* rp, TextureProvider* tp) :
		m_Size({ 100, 100 }),
		m_TextureProvider(tp),
		m_ResourceProvider(rp)
	{
		m_LayoutUpdater = new LayoutTreeUpdater(this);
		m_StyleTreeUpdater = new StyleTreeUpdater(this);
		m_TreePainter = new TreePainter(this);

		m_Root = new Element();
		m_Root->m_Depth = 1;
		m_Root->m_Surface = this;
	}

	Surface::~Surface()
	{
		delete m_Root;
	}

	void Surface::process()
	{
		m_StyleTreeUpdater->process();
		m_LayoutUpdater->process();
		m_TreePainter->process();
	}

	Element* Surface::findElementsAt(Element* root, const Vec2& point, std::vector<Element*>* found)
	{
		/*
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

		*/
		// no hit
		return nullptr;
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

	Element* Surface::getRoot() const
	{
		return m_Root;
	}

	void Surface::setSize(const Vec2& size)
	{
		m_Size = size;
	}

}
