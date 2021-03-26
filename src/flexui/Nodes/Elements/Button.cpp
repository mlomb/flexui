#include "flexui/Nodes/Elements/Button.hpp"

#include "flexui/Events/Events.hpp"
#include "flexui/Events/EventsController.hpp"
#include "flexui/Nodes/Document.hpp"

namespace flexui {

	Button::Button() : Element("button")
	{
	}

	Button::~Button()
	{
	}

	void Button::executeDefault(EventBase* evt)
	{
		Element::executeDefault(evt);

		switch (evt->type) {
		case EventTypeID::MOUSE_DOWN:
			getDocument()->getEventsController().captureElement(this);
			break;
		case EventTypeID::MOUSE_UP:
			getDocument()->getEventsController().captureElement(nullptr);
			break;
		}
	}

}
