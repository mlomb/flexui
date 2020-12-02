#include "Surface.hpp"

#include "Layout/LayoutTreeUpdater.hpp"
#include "Style/StyleTreeUpdater.hpp"

namespace flexui {

	Surface::Surface()
		: m_Size({ 100, 100 })
	{
		m_LayoutUpdater = new LayoutTreeUpdater(this);
		m_StyleTreeUpdater = new StyleTreeUpdater(this);

		m_Root = new Element();
		m_Root->m_Depth = 1;
		m_Root->m_Surface = this;
	}

	Surface::~Surface()
	{
		delete m_Root;
	}

	void Surface::updateTree()
	{
		m_StyleTreeUpdater->process();
		m_LayoutUpdater->process();
	}

	Element* Surface::getRoot() const
	{
		return m_Root;
	}

	void Surface::setSize(const UIVec2& size)
	{
		m_Size = size;
	}

	Element* Surface::findElementsAt(Element* root, const UIVec2& point, std::vector<Element*>* found)
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

	UIVec2 Surface::getSize() const
	{
		return m_Size;
	}

}
