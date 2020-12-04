#pragma once

namespace flexui {

	typedef void* TextureID;

	class TextureProvider {
	public:
		TextureProvider() { };
		virtual ~TextureProvider() { };

		virtual TextureID create(unsigned int width, unsigned int height) = 0;
		virtual void resize(TextureID texture, unsigned int width, unsigned int height) = 0;
		virtual void store(TextureID texture, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const void* image) = 0;
		virtual void dispose(TextureID texture) = 0;

	private:
	};

}