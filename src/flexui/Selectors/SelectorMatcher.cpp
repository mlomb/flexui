#include "flexui/Selectors/SelectorMatcher.hpp"

#include "flexui/Nodes/Element.hpp"

namespace flexui {

    bool MatchesSelector(Element* element, const Selector& selector)
    {
        Element* current = element;
        Element* backElem = nullptr;
        int backIndex = -1;
        int partIndex = (int)selector.size() - 1;

        while (partIndex >= 0)
        {
            if (!current)
                break;

            bool matchesPart = false;

            // match part
            {
                const SelectorPart& part = selector[partIndex];

                matchesPart =
                    // match pseudos
                    ((part.pseudos & current->m_PseudoStates) == part.pseudos)
                    &&
                    (
                        (
                            // wildcard always matches
                            part.identifier.type == SelectorIdentifierType::TAG &&
                            part.identifier.text_hash == HashStr("*")
                            ) ||
                        current->m_ID == part.identifier ||
                        current->m_Tag == part.identifier ||
                        std::find(current->m_Classes.begin(), current->m_Classes.end(), part.identifier) != current->m_Classes.end()
                        );

            }

            return matchesPart; // for now only match the last part

            //if (--backIndex < 0)
            //    return true;
            
            //current = current->getParent();
        }
        return false;
    }

}
