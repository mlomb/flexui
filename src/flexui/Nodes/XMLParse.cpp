#include "flexui/Nodes/XMLParse.hpp"

// TinyXML2
// https://github.com/leethomason/tinyxml2
#include "tinyxml2/tinyxml2.h"

#include "flexui/Math.hpp"
#include "flexui/Log.hpp"

// Elements
#include "flexui/Nodes/Node.hpp"
#include "flexui/Nodes/Text.hpp"
#include "flexui/Nodes/Style.hpp"
#include "flexui/Nodes/Comment.hpp"
#include "flexui/Nodes/Elements/Button.hpp"
#include "flexui/Nodes/Elements/Slider.hpp"

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

    Node* parseNode(XMLNode* xml_node, XMLParseResult& parseResult)
    {
        if (xml_node->ToComment())
            return new Comment(xml_node->Value());

        Node* node = nullptr;

        XMLElement* xml_element = xml_node->ToElement();
        if (xml_element) {
            const char* name = xml_element->Value();
            FUI_DEBUG_ASSERT(name != nullptr);

            uint32_t name_hash = HashStr(name);

			if (name_hash == HashStr("style")) {
                // style element
                std::string css = "";
                XMLText* text_node = xml_node->FirstChild() ? xml_node->FirstChild()->ToText() : NULL;
                if (text_node) {
                    css = text_node->Value();
                }
                return new Style(css);
			}

            Element* element = nullptr;

            switch (name_hash) {
            case HashStr("button"): element = new Button(); break;
            case HashStr("slider"): element = new Slider(); break;
            default:
                element = new Element(name);
                break;
            }

            if (element) {
                node = element;
                parseAttributes(xml_element, element);

                XMLNode* child_xml_node = xml_node->FirstChild();
                while (child_xml_node) {
                    Node* child_node = parseNode(child_xml_node, parseResult);
                    if (child_node)
                        element->appendChild(child_node);

                    child_xml_node = child_xml_node->NextSibling();
                }
            }

        }

        XMLText* text_node = xml_node->ToText();
        if (text_node) {
            Text* text_element = new Text(text_node->Value());
            node = text_element;
        }

        if (!node) {
            parseResult.warnings.push_back("Can't parse node " + std::string(xml_node->Value()));
            return nullptr;
        }

        return node;
    }

    Node* ParseXML(const std::string& source, XMLParseResult& parseResult)
    {
        XMLDocument doc(true, tinyxml2::COLLAPSE_WHITESPACE);
        doc.Parse(source.c_str(), source.size());

        if (doc.ErrorID() != XMLError::XML_SUCCESS) {
            parseResult.errors.push_back(doc.ErrorStr());
            return nullptr;
        }

		// convert XML to Nodes
        Element* element = new Element("div");
		XMLNode* xml_node = doc.FirstChild();
        while (xml_node) {
            Node* node = parseNode(xml_node, parseResult);
            if(node)
                element->appendChild(node);
            xml_node = xml_node->NextSibling();
        }
        // TODO: if the document only has one child, return that instead
        return element;
    }
}
