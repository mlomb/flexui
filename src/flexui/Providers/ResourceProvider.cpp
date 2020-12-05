#include "flexui/Providers/ResourceProvider.hpp"

#include "flexui/Render/Font.hpp"
#include "flexui/DefaultResources.hpp"
#include <cassert>

namespace flexui {

    std::shared_ptr<Font> ResourceProvider::getFont(const std::string& familyName)
    {
        auto it = m_Fonts.find(familyName);
        if (it != m_Fonts.end())
            return (*it).second;

        unsigned char* data = nullptr;
        size_t size = 0;

        if (familyName == "default") {
            data = (unsigned char*)GetDefaultFontData();
            size = GetDefaultFontSize();
        }
        else {
            loadFont(familyName, data, size);
        }

        if (size > 0) {
            assert(data != nullptr);
            auto font = std::shared_ptr<Font>(Font::LoadFromMemory(data, size));
            m_Fonts.emplace(familyName, font);
            return font;
        }
        else {
            m_Fonts.emplace(familyName, nullptr);
            return nullptr;
        }
    }
}

