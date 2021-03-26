#pragma once

#include <queue>

#include "flexui/Events/Events.hpp"

namespace flexui {

	class Document;

	class EventsController {
	public:
		EventsController(Document* document);
		virtual ~EventsController();

		void process();
		void captureElement(Element* element);
		void releaseCapture();
		Element* getElementOverMouse() const;

		void sendMouseMove(const Vec2& position);
		void sendMouseButton(int button, bool down, const Vec2& position);

	private:
		Document* m_Document;

		std::queue<EventBase*> m_EventQueue;

		Element* m_LastElementUnderMouse;
		Element* m_CapturedElement;

		void queueEvent(EventBase* evt);
		void processEventQueue();
		void updateElementUnderMouse(const Vec2& mousePosition);
	};

}