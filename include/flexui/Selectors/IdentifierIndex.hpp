#pragma once

#include <vector>
#include <unordered_map>

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

}
