#pragma once

#include <string>

namespace flexui {

	class ResourceProvider {
	public:
		ResourceProvider() { };
		virtual ~ResourceProvider() { };
		
		virtual bool loadFont(const std::string& familyName, void*& data, size_t& size) { return false; };

		// TODO: loadImage, loadStylesheet

	};

}