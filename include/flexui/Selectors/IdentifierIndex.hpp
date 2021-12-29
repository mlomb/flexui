#pragma once

#include <vector>
#include <unordered_map>

#include "flexui/Nodes/Element.hpp"
#include "flexui/Selectors/Selector.hpp"
#include "flexui/Selectors/SelectorMatcher.hpp"

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
	inline void IdentifierIndex<T>::findMatches(const Element* element, std::vector<T>& matches)
	{
        const uint32_t empty_id = HashStr("");

		const auto& relevant = findIdentifiers(SelectorIdentifierType::TAG, HashStr("*"));


		#define FAST_LOOKUP(type, str_hash) \
        { \
			const auto& relevant = findIdentifiers(type, str_hash); \
			for (auto& t : relevant) { \
				matches.push_back(t); \
			} \
        }

		FAST_LOOKUP(SelectorIdentifierType::TAG, HashStr("*")); // match wildcard
        FAST_LOOKUP(SelectorIdentifierType::TAG, element->getTag().hash());

        if (element->getID().hash() != empty_id)
            FAST_LOOKUP(SelectorIdentifierType::ID, element->getID().hash());

        for (auto& klass : element->getClasses())
            FAST_LOOKUP(SelectorIdentifierType::CLASS, klass.hash());
	}

}
