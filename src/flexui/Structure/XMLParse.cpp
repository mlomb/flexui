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

    Element* parseElement(XMLNode* node, XMLParseResult& parseResult) {
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
                Text* text_element = new Text();
                std::string text;

                // get all childs as text
                XMLNode* child_node = node->FirstChild();
                while (child_node) {
                    XMLText* text_node = child_node->ToText();
                    if (text_node) {
                        text += text_node->Value();
                    } else {
                        parseResult.warnings.push_back("Unexpected non text node");
                    }

                    child_node = child_node->NextSibling();
                }

                add_recursive = false;
                text_element->setText(text);
                element = text_element;
                break;
            }
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
        XMLDocument doc;
        doc.Parse(source.c_str(), source.size());

        if (doc.ErrorID() != XMLError::XML_SUCCESS) {
            parseResult.errors.push_back(doc.ErrorStr());
            return nullptr;
        }

        // convert XML to Elements
        return parseElement(doc.RootElement(), parseResult);
    }
}
