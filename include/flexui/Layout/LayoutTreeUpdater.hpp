#pragma once

#include "flexui/TreeProcessor.hpp"

namespace flexui {

	class LayoutTreeUpdater : public TreeProcessor {
	public:
		LayoutTreeUpdater(Surface* surface);
		virtual ~LayoutTreeUpdater();

		void process() override;

	private:
		void updateLayout(Element* element);
	};

}