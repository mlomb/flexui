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
		Surface(ResourceProvider*, TextureProvider*);
		~Surface();

		void process();

		void setSize(const Vec2& size);

		Element* findElementsAt(Element* root, const Vec2& point, std::vector<Element*>* found = nullptr);

		Element* getRoot() const;
		Vec2 getSize() const;
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

		Vec2 m_Size;
	};

}