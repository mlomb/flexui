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
                    ((part.pseudos & current->getPseudoStates()) == part.pseudos)
                    &&
                    (
                        (
                            // wildcard always matches
                            part.identifier.type == SelectorIdentifierType::TAG &&
                            part.identifier.value == HashedString("*")
                            ) ||
                        (part.identifier.type == SelectorIdentifierType::ID && current->getID() == part.identifier.value) ||
                        (part.identifier.type == SelectorIdentifierType::TAG && current->getTag() == part.identifier.value) ||
                        std::find(current->getClasses().begin(), current->getClasses().end(), part.identifier) != current->getClasses().end()
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
