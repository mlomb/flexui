#pragma once

#include <vector>

#include "flexui/Selectors/PseudoStates.hpp"
#include "flexui/Misc/String.hpp"

namespace flexui {

	enum class SelectorIdentifierType : uint8_t {
		ID,
		TAG,
		CLASS
	};

	enum class SelectorRelationship : uint8_t {
		NONE, // same target (for example tag.class:pseudo)
		DESCENDANT, // default (any in subtree)
		CHILD, // >
		ADJACENT_SIBLING, // +
		GENERAL_SIBLING // ~
	};

	struct SelectorIdentifier {
		SelectorIdentifierType type;
		HashedString value;
	};

	struct SelectorPart {
		SelectorIdentifier identifier;
		SelectorRelationship prev_rel;
		PseudoStates pseudos;
	};

	typedef std::vector<SelectorPart> Selector;

	/// Compute a selector specificity according to the CSS spec
	int ComputeSelectorSpecificity(const Selector& selector);

}
