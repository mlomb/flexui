#pragma once

#include <cstdint>
#include <unordered_map>

#include <flexui/Math.hpp>
#include <flexui/Providers/TextureProvider.hpp>

namespace flexui {

	class DynamicAtlasAllocator;

	class DynamicAtlas {
	public:
		typedef uint64_t Index;

		DynamicAtlas(TextureProvider* textureProvider);
		~DynamicAtlas();

		bool tryGet(const Index index, Rect& rect) const;
		TextureID getTexture() const;
		unsigned int getSize() const;

		bool add(const Index index, const unsigned int width, const unsigned int height, const unsigned char* data, const size_t size, Rect& rect);

	private:
		TextureProvider* m_TextureProvider;
		DynamicAtlasAllocator* m_Allocator;

		std::unordered_map<Index, Rect> m_Entries;

		TextureID m_Texture;
	};

}