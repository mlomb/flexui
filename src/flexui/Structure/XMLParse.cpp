#include "flexui/Structure/XMLParse.hpp"

// TinyXML2
// https://github.com/leethomason/tinyxml2
#include "tinyxml2/tinyxml2.h"

#include "flexui/Math.hpp"
#include "flexui/Log.hpp"

// Elements
#include "flexui/Element.hpp"
#include "flexui/Elements/Button.hpp"
#include "flexui/Elements/Text.hpp"

namespace flexui {
    using namespace tinyxml2;

    void parseAttributes(XMLElement* xml_element, Element* element)
    {
        const char* id = xml_element->Attribute("id");
        const char* klass = xml_element->Attribute("class");

        if (id)
            element->setID(std::string(id));

        // parse class attribute
        if (klass) {
            // split classes by space
            size_t len = strlen(klass);
            int last_space = -1;
            for (int i = 0; i < len; i++) {
                if (klass[i] == ' ') {
                    // token in [last_space+1, i]
                    int l = last_space + 1;
                    int r = i;
                    if (r - l > 0) {
                        // non empty token
                        element->addClass(std::string(klass + l, (size_t)(r - l)));
                    }
                    last_space = i;
                }
            }
            // last token [last_space+1, len]
            int l = last_space + 1;
            int r = len;
            if (r - l > 0) {
                element->addClass(std::string(klass + l, (size_t)(r - l)));
            }
        }

    }

    Element* parseElement(XMLNode* node, XMLParseResult& parseResult)
    {
        if (node->ToComment())
            return nullptr; // skip comments

        Element* element = nullptr;
        bool add_recursive = true;

        XMLElement* xml_element = node->ToElement();
        if (xml_element) {
            const char* name = xml_element->Value();
            FUI_DEBUG_ASSERT(name != nullptr);

            switch (HashStr(name)) {
            case HashStr("Button"): element = new Button(); break;
            case HashStr("Element"): element = new Element(); break;
            case HashStr("Text"):
                parseResult.warnings.push_back("Text elements should not be created explicitly");
                break;
            }

            if(element)
                parseAttributes(xml_element, element);
        }

        XMLText* text_node = node->ToText();
        if (text_node) {
            Text* text_element = new Text();
            text_element->setText(text_node->Value());
            element = text_element;
        }

        if (!element) {
            printf("FIXME: %s\n", node->Value());
            return nullptr;
        }

        printf("PARSED: %s\n", node->Value());

        if (add_recursive) {
            XMLNode* child_node = node->FirstChild();
            while (child_node) {
                Element* child_element = parseElement(child_node, parseResult);
                if (child_element)
                    element->addElement(child_element);

                child_node = child_node->NextSibling();
            }
        }

        return element;
    }

    Element* ParseXML(const std::string& source, XMLParseResult& parseResult)
    {
        XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
        doc.Parse(source.c_str(), source.size());

        if (doc.ErrorID() != XMLError::XML_SUCCESS) {
            parseResult.errors.push_back(doc.ErrorStr());
            return nullptr;
        }

        // convert XML to Elements
        return parseElement(doc.RootElement(), parseResult);
    }
}
