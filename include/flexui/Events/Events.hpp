#pragma once

#include <vector>

#include "flexui/Math.hpp"

namespace flexui {

	class Element;

	enum class EventTypeID {
		MOUSE_MOVE,
		MOUSE_ENTER,
		MOUSE_LEAVE,
		MOUSE_DOWN,
		MOUSE_UP,

		CAPTURE_IN,
		CAPTURE_OUT,

		INVALID
	};

	class EventBase {
	public:
		EventTypeID type = EventTypeID::INVALID;
		Element* target = nullptr;

		// Propagation path
		// See https://w3c.github.io/uievents/#event-flow
		std::vector<Element*> path;
	};

	struct MouseEvent : public EventBase {
		Vec2 mousePosition;
	};

	struct MouseButtonEvent : public MouseEvent {
		int button;
	};

}