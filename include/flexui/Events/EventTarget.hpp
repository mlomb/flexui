#pragma once

#include <string>
#include <functional>

namespace flexui {

	class EventTarget {
	public:

		void addEventListener(const std::string& event, std::function<void> func);

	private:

	};

}