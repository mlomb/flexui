#pragma once

#include "flexui/Selectors/Selector.hpp"

namespace flexui {

	class Element;

	/// Check wether an Element matches a particular selector
	bool MatchesSelector(Element* element, const Selector& selector);

}