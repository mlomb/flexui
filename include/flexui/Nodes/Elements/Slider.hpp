#pragma once

#include "flexui/Nodes/Element.hpp"

namespace flexui {
	/// Slider UI element
	class Slider : public Element {
	public:
		Slider();
		virtual ~Slider();

		void executeDefault(EventBase* evt) override;

		virtual std::string getNodeName() const override { return "Slider"; };
	};
}
