#pragma once

#include "Element.hpp"

namespace flexui {

	class LayoutTreeUpdater;
	class StyleTreeUpdater;
	class TreePainter;

	class Surface {
	public:
		Surface();
		~Surface();

		void updateTree();

		void setSize(const UIVec2& size);
		Element* findElementsAt(Element* root, const UIVec2& point, std::vector<Element*>* found = nullptr);

		Element* getRoot() const;
		UIVec2 getSize() const;
		Painter* getPainter() const;

	private:
		Element* m_Root;

		LayoutTreeUpdater* m_LayoutUpdater;
		StyleTreeUpdater* m_StyleTreeUpdater;
		TreePainter* m_TreePainter;

		UIVec2 m_Size;
	};

}