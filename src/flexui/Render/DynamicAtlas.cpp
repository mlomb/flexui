#include "flexui/Render/DynamicAtlas.hpp"

#include <cassert>

#include "flexui/Render/DynamicAtlasAllocator.hpp"

namespace flexui {

	DynamicAtlas::DynamicAtlas(TextureProvider* textureProvider) :
		m_TextureProvider(textureProvider),
		m_Allocator(new DynamicAtlasAllocator(64, 4096))
	{
		m_Texture = m_TextureProvider->create(64, 64);
	}

	DynamicAtlas::~DynamicAtlas()
	{
		m_TextureProvider->dispose(m_Texture);
	}

	bool DynamicAtlas::tryGet(const Index index, Rect& rect) const
	{
		auto it = m_Entries.find(index);
		if (it != m_Entries.end()) {
			rect = (*it).second;
			return true;
		}
		return false;
	}

	bool DynamicAtlas::add(const Index index, const unsigned int width, const unsigned int height, const unsigned char* data, const size_t size, Rect& rect)
	{
		assert(tryGet(index, rect) == false);

		rect.width = width;
		rect.height = height;

		unsigned int prevAtlasSize = m_Allocator->getSize();

		// allocate
		unsigned int lx, ly;
		if (m_Allocator->tryAllocate(width, height, lx, ly)) {
			rect.x = lx;
			rect.y = ly;

			m_Entries.insert(std::make_pair(index, rect));

			unsigned int newAtlasSize = m_Allocator->getSize();
			if (prevAtlasSize != newAtlasSize) {
				// atlas must grow
				m_Texture = m_TextureProvider->resize(m_Texture, prevAtlasSize, prevAtlasSize, newAtlasSize, newAtlasSize);
			}

			m_TextureProvider->store(m_Texture, rect.x, rect.y, rect.width, rect.height, data/*, size*/);

			return true;
		}

		return false;
	}

	TextureID DynamicAtlas::getTexture() const
	{
		return m_Texture;
	}

	unsigned int DynamicAtlas::getSize() const
	{
		return m_Allocator->getSize();
	}

}
