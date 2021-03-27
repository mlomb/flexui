#pragma once

#include <vector>
#include <string>

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

	enum class SelectorPseudoState : uint8_t {
		NONE =          0,
		HOVER =    1 << 0,
		DISABLED = 1 << 1,
		CHECKED =  1 << 2,
		ACTIVE =   1 << 3,
		FOCUS =    1 << 4
	};

	struct SelectorIdentifier {
		SelectorIdentifierType type;
		HashedString value;
	};
	
	struct SelectorPart {
		SelectorIdentifier identifier;
		SelectorRelationship prev_rel;
		SelectorPseudoState pseudos;
	};

	typedef std::vector<SelectorPart> Selector;

	/// Compute a selector specificity according to the CSS spec
	int ComputeSelectorSpecificity(const Selector& selector);

	// operators for SelectorPseudoState
	inline SelectorPseudoState operator~(const SelectorPseudoState& a) { return (SelectorPseudoState)~(uint8_t)a; }
	inline SelectorPseudoState operator&(const SelectorPseudoState& a, const SelectorPseudoState& b) { return (SelectorPseudoState)((uint8_t)a & (uint8_t)b); }
	inline SelectorPseudoState& operator|=(SelectorPseudoState& a, const SelectorPseudoState b) { return (SelectorPseudoState&)((uint8_t&)a |= (uint8_t)b); }
	inline SelectorPseudoState& operator&=(SelectorPseudoState& a, const SelectorPseudoState b) { return (SelectorPseudoState&)((uint8_t&)a &= (uint8_t)b); }

}
