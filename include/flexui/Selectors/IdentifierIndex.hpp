#pragma once

#include <vector>
#include <unordered_map>

#include "flexui/Nodes/Element.hpp"
#include "flexui/Selectors/Selector.hpp"

namespace flexui {

	// Note: does not check for duplicates
	template<typename T>
	class IdentifierIndex {
	public:
		IdentifierIndex() { };
		~IdentifierIndex() { };

		void addIdentifier(const SelectorIdentifierType type, const uint32_t hash, T value);
		void removeIdentifier(const SelectorIdentifierType type, const uint32_t hash, T value);

		const std::vector<T>& findIdentifiers(const SelectorIdentifierType type, const uint32_t hash);
		void findMatches(const Element* elem, std::vector<T>& matches);

	private:
		typedef std::unordered_map<uint32_t, std::vector<T>> IdentifierLookupTable;

		IdentifierLookupTable m_Identifiers[3];
	};

	template<typename T>
	inline void IdentifierIndex<T>::addIdentifier(const SelectorIdentifierType type, const uint32_t hash, T value)
	{
		m_Identifiers[(int)type][hash].push_back(value);
	}

	template<typename T>
	inline void IdentifierIndex<T>::removeIdentifier(const SelectorIdentifierType type, const uint32_t hash, T value)
	{
		auto& it1 = m_Identifiers[(int)type].find(hash);
		if (it1 != m_Identifiers[(int)type].end()) {
			auto& hash_vec = (*it1).second;
			auto& it2 = std::find(hash_vec.begin(), hash_vec.end(), value);
			if (it2 != hash_vec.end()) {
				hash_vec.erase(it2);
			}
		}
	}

	template<typename T>
	inline const std::vector<T>& IdentifierIndex<T>::findIdentifiers(const SelectorIdentifierType type, const uint32_t hash)
	{
		auto& it1 = m_Identifiers[(int)type].find(hash);
		if (it1 != m_Identifiers[(int)type].end()) {
			return (*it1).second;
		}
		static std::vector<T> empty;
		return empty;
	}

	template<typename T>
	inline void IdentifierIndex<T>::findMatches(const Element* elem, std::vector<T>& matches)
	{
        const StrHash empty_id = HashStr("");

        SelectorMatch match;

        #define FAST_LOOKUP(type, str_hash) \
        { \
			const auto& relevant = findIdentifiers(type, str_hash); \
            for (StyleSelector* selector : relevant) { \
                if (MatchesSelector(element, selector)) { \
                    match.selector = selector; \
                    matches.push_back(match); \
                } \
            } \
        }

		FAST_LOOKUP(SelectorIdentifierType::TAG, HashStr("*")); // match wildcard

        FAST_LOOKUP(HashStr("*"), m_OrderedTags);
        FAST_LOOKUP(element->m_Tag.text_hash, m_OrderedTags);

        if (element->m_ID.text_hash != empty_id)
            FAST_LOOKUP(element->m_ID.text_hash, m_OrderedIDs);

        for (auto& klass : element->m_Classes)
            FAST_LOOKUP(klass.text_hash, m_OrderedClasses);
	}

}
