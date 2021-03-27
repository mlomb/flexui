#include "flexui/Selectors/Selector.hpp"

namespace flexui {

	// See https://www.w3.org/TR/selectors-3/#specificity
    int ComputeSelectorSpecificity(const Selector& selector)
    {
		int a = 0, b = 0, c = 0;

		for (const SelectorPart& part : selector) {
			switch (part.identifier.type)
			{
			case SelectorIdentifierType::ID:
				a++;
				break;
			case SelectorIdentifierType::CLASS:
				b++;
				break;
			case SelectorIdentifierType::TAG:
				if (part.identifier.text.size() == 1 && part.identifier.text[0] == '*') {
					// ignore wildcard
				}
				else
					c++;
				break;
			}

			uint32_t pseudos = (uint32_t)part.pseudos;
			while (pseudos) {
				b += pseudos & 1; // count the number of bits set
				pseudos >>= 1;
			}
		}

		return a * 100 + b * 10 + c;
    }

}
