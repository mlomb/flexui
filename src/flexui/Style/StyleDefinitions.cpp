#include "flexui/Style/StyleDefinitions.hpp"

namespace flexui {

	// See https://www.w3.org/TR/selectors-3/#specificity
	void StyleSelector::computeSpecificity() {
		int a = 0, b = 0, c = 0;

		for (const StyleSelectorPart& part : parts) {
			switch (part.identifier.type)
			{
			case StyleIdentifierType::ID:
				a++;
				break;
			case StyleIdentifierType::CLASS:
				b++;
				break;
			case StyleIdentifierType::TAG:
				if (part.identifier.text.size() == 1 && part.identifier.text[0] == '*') {
					// ignore wildcard
				}
				else
					c++;
				break;
			}

			uint32_t pseudos = (uint32_t)part.pseudo_states;
			while (pseudos) {
				b += pseudos & 1; // count the number of bits set
				pseudos >>= 1;
			}
		}

		specificity = a * 100 + b * 10 + c;
	}

}
