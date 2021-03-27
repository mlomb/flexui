#include "flexui/Style/StyleSelectorMatcher.hpp"

#include "flexui/Nodes/Element.hpp"
#include "flexui/Style/StyleDefinitions.hpp"
#include "flexui/Style/StyleSheet.hpp"

namespace flexui {

    bool SelectorMatch::operator<(const SelectorMatch& b) const
    {
        if (selector->specificity != b.selector->specificity)
            return selector->specificity < b.selector->specificity; // prefer higher specificity
        if (stylesheetIndex != b.stylesheetIndex)
            return stylesheetIndex < b.stylesheetIndex; // prefer stylesheet precedence
        if (selector->order != b.selector->order)
            return selector->order < b.selector->order; // prefer precedence in the same stylesheet
        return false; // tie
    }

    void StyleSelectorMatcher::FindMatches(Element* element, std::vector<StyleSheet*>& sheets, std::vector<SelectorMatch>& matches)
    {
        const StrHash empty_id = HashStr("");

        SelectorMatch match;

        for (int i = 0; i < sheets.size(); i++) {
            StyleSheet* sheet = sheets[i];

            match.stylesheetIndex = i;
            match.sheet = sheet;

            #define FAST_LOOKUP(hash, lookupTable) \
            { \
                auto it = sheet->lookupTable.find(hash); \
                if (it != sheet->lookupTable.end()) { \
                    const auto& relevant = (*it).second; \
                    for (StyleSelector* selector : relevant) { \
                        if (MatchesSelector(element, selector)) { \
                            match.selector = selector; \
                            matches.push_back(match); \
                        } \
                    } \
                } \
            }

            FAST_LOOKUP(HashStr("*"), m_OrderedTags); // match wildcard
            FAST_LOOKUP(element->m_Tag.text_hash, m_OrderedTags);

            if (element->m_ID.text_hash != empty_id)
                FAST_LOOKUP(element->m_ID.text_hash, m_OrderedIDs);

            for (auto& klass : element->m_Classes)
                FAST_LOOKUP(klass.text_hash, m_OrderedClasses);
        }
    }

}