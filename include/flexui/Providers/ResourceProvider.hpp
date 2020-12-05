#pragma once

#include <string>
#include <map>
#include <memory>

namespace flexui {

	class Font;

	class ResourceProvider {
	public:
		ResourceProvider() { };
		virtual ~ResourceProvider() { };
		
		virtual bool loadFont(const std::string& familyName, unsigned char*& data, size_t& size) { return false; };

		// TODO: loadImage, loadStylesheet

		std::shared_ptr<Font> getFont(const std::string& familyName);

	private:
		std::map<std::string, std::shared_ptr<Font>> m_Fonts;
	};

}