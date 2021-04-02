#pragma once

#include <cstdint>

namespace flexui {

	enum class PseudoStates : uint8_t {
		NONE =          0,
		HOVER =    1 << 0,
		DISABLED = 1 << 1,
		CHECKED =  1 << 2,
		ACTIVE =   1 << 3,
		FOCUS =    1 << 4
	};

	inline PseudoStates operator~(const PseudoStates& a) { return (PseudoStates)~(uint8_t)a; }
	inline PseudoStates operator&(const PseudoStates& a, const PseudoStates& b) { return (PseudoStates)((uint8_t)a & (uint8_t)b); }
	inline PseudoStates& operator|=(PseudoStates& a, const PseudoStates b) { return (PseudoStates&)((uint8_t&)a |= (uint8_t)b); }
	inline PseudoStates& operator&=(PseudoStates& a, const PseudoStates b) { return (PseudoStates&)((uint8_t&)a &= (uint8_t)b); }

}
