#pragma once

#include <array>
#include <algorithm>
#include "Type.h"

namespace ash {

template<typename... Args>
using IdContainer = std::array<Type::Id, sizeof...(Args)>;

template<typename... Args>
class Signature {
public:
	static const IdContainer<Args...> kIds;

	Signature() = delete;

	static bool Contains(Type::Id id) {
		return std::binary_search(kIds.begin(), kIds.end(), id);
	}

private:
	template<typename...>
	struct Filler;

	template<typename Head, typename... Tail>
	struct Filler<Head, Tail...> {
		static void Fill(IdContainer<Args...>* container, size_t index) {
			(*container)[index] = Type::GetId<Head>();
			Filler<Tail...>::Fill(container, index + 1);
		}
	};

	template<>
	struct Filler<> {
		static void Fill(IdContainer<Args...>* container, size_t index) {}
	};

	static IdContainer<Args...> CreateIds() {
		IdContainer<Args...> ids;
		Filler<Args...>::Fill(&ids, 0);
		std::sort(ids.begin(), ids.end());
		return ids;
	}
};

template<typename... Args>
const IdContainer<Args...> Signature<Args...>::kIds = Signature<Args...>::CreateIds();

} // namespace ash;