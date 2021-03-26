#include "flexui/Nodes/Document.hpp"

#include "flexui/Layout/LayoutObject.hpp"
#include "flexui/Style/StyleEngine.hpp"
#include "flexui/Nodes/Element.hpp"

namespace flexui {

	Document::Document(ResourceProvider* resource_provider, TextureProvider* texture_provider) :
		m_ResourceProvider(resource_provider),
		m_TextureProvider(texture_provider),
		m_StyleEngine(this),
		m_LayoutEngine(this),
		m_RenderEngine(this),
		m_EventController(this)
	{
		m_Document = this;
		m_Layout = new LayoutObject();
	}

	Document::~Document()
	{
	}

	Element* Document::findElementsAt(Node* node, const Vec2& point, std::vector<Element*>* found)
	{
		if (node == nullptr || node->getLayoutObject() == nullptr)
			return nullptr;

		Element* element = node->getNodeType() == NodeType::ELEMENT ? static_cast<Element*>(node) : nullptr;

		const auto& bbox = node->getLayoutObject()->getLayoutRect();

		if (element && !element->isVisible())
			return nullptr; // Element is not visible

		if (bbox.contains(point)) {
			if (found && element)
				found->push_back(element);

			if (node->isContainerNode()) {
				ContainerNode* container_node = static_cast<ContainerNode*>(node);

				// iterate in reverse order (z-index)
				for (Node* child = container_node->getLastChild(); child; child = child->getPreviousSibling()) {
					Element* result = findElementsAt(child, point, found);
					if (result)
						return result;
				}
			}

			if(element)
				return element;
		}

		// no hit
		return nullptr;
	}

}