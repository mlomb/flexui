#pragma once

namespace flexui {

	typedef unsigned int TextureID;

	class TextureProvider {
	public:
		TextureProvider() { };
		virtual ~TextureProvider() { };

		virtual TextureID create(unsigned int width, unsigned int height) = 0;
		virtual TextureID resize(TextureID texture, unsigned int oldWidth, unsigned int oldHeight, unsigned int newWidth, unsigned int newHeight) = 0;
		virtual void store(TextureID texture, unsigned int x, unsigned int y, unsigned int width, unsigned int height, const unsigned char* image) = 0;
		virtual void dispose(TextureID texture) = 0;

	};

}
