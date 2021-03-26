#pragma once

#include "flexui/Nodes/Element.hpp"

namespace flexui {
	/// Button UI element
	class Button : public Element {
	public:
		Button();
		virtual ~Button();

		void executeDefault(EventBase* evt) override;
	};
}
