#pragma once

#include "Element.hpp"

namespace flexui {

	class LayoutTreeUpdater;
	class StyleTreeUpdater;
	class TreePainter;

	class TextureProvider;
	class ResourceProvider;

	class Surface {
	public:
		Surface();
		~Surface();

		void process();

		void setSize(const UIVec2& size);
		// TODO: smart pointers?
		void setTextureProvider(TextureProvider* provider);
		void setResourceProvider(ResourceProvider* provider);

		Element* findElementsAt(Element* root, const UIVec2& point, std::vector<Element*>* found = nullptr);

		Element* getRoot() const;
		UIVec2 getSize() const;
		Painter* getPainter() const;
		TextureProvider* getTextureProvider() const;
		ResourceProvider* getResourceProvider() const;

	private:
		Element* m_Root;

		TextureProvider* m_TextureProvider;
		ResourceProvider* m_ResourceProvider;

		LayoutTreeUpdater* m_LayoutUpdater;
		StyleTreeUpdater* m_StyleTreeUpdater;
		TreePainter* m_TreePainter;

		UIVec2 m_Size;
	};

}