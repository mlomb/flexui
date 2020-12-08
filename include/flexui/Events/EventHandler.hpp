#pragma once

#include <functional>
#include <memory>
#include <vector>

namespace flexui {

	template<class TEvent>
	class EventHandler {
	public:
		typedef typename std::function<void(TEvent&)> EventCallback;
		typedef typename std::shared_ptr<EventCallback> EventCallbackPtr;
		typedef typename std::vector<EventCallbackPtr> Callbacks;

		EventHandler() { }

		EventCallbackPtr addListener(EventCallback callback) {
			EventCallbackPtr ptr(new EventCallback(callback));
			m_Callbacks.emplace_back(ptr);
			return ptr;
		}

		template<class T>
		EventCallbackPtr addListener(T* instance, void(T::* callback)(TEvent&)) {
			return AddListener(std::bind(callback, instance, std::placeholders::_1));
		}

		void removeListener(EventCallbackPtr callbackPtr) {
			auto it = std::find(m_Callbacks.begin(), m_Callbacks.end(), callbackPtr);
			if (it != m_Callbacks.end())
				m_Callbacks.erase(it);
		}

		void fireEvent(TEvent& event) {
			for (typename Callbacks::reverse_iterator callback = m_Callbacks.rbegin(); callback != m_Callbacks.rend(); ++callback) {
				(*(*callback))(event);
			}
		}

	private:
		Callbacks m_Callbacks;
	};

}
