#include "flexui/Nodes/Elements/Slider.hpp"

#include "flexui/Events/Events.hpp"
#include "flexui/Events/EventsController.hpp"

namespace flexui {

	Slider::Slider()
	{
		setTag("Slider");
	}

	Slider::~Slider()
	{
	}

	void Slider::executeDefault(EventBase* evt)
	{
		Element::executeDefault(evt);

		switch (evt->type) {
		case EventTypeID::MOUSE_DOWN:
			//getSurface()->getEventsController()->captureElement(this);
			break;
		case EventTypeID::MOUSE_UP:
            //getSurface()->getEventsController()->captureElement(nullptr);
			break;
		}
	}

}
