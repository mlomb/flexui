#pragma once

#include <queue>

#include "flexui/TreeProcessor.hpp"

#include "flexui/Events/Events.hpp"

namespace flexui {

	class EventsController : public TreeProcessor {
	public:
		EventsController(Surface* surface);
		virtual ~EventsController();

		void process() override;
		void captureElement(Element* element);
		void releaseCapture();
		Element* getElementOverMouse() const;

		void sendMouseMove(const Vec2& position);
		void sendMouseButton(int button, bool down, const Vec2& position);

	private:
		std::queue<EventBase*> m_EventQueue;

		Element* m_LastElementUnderMouse;
		Element* m_CapturedElement;

		void queueEvent(EventBase* evt);
		void processEventQueue();
		void updateElementUnderMouse(const Vec2& mousePosition);
	};

}