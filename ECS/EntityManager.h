#pragma once

#include <functional>
#include <unordered_set>
#include "Entity.h"

namespace ecs {

class EntityManager {
public:
	class Iterator {
	friend EntityManager;

	public:
		Entity& Get() {
			return *(*begin_);
		}

		bool HasCurrent() const {
			return begin_ != end_;
		}

		void Next() {
			++begin_;

			while (begin_ != end_ && !IsMatch(*begin_)) {
				++begin_;
			}
		}

	private:
		std::unordered_set<Entity*>::iterator begin_;
		std::unordered_set<Entity*>::iterator end_;
		Signer::Signature signature_;

		Iterator(
			std::unordered_set<Entity*>::iterator begin, 
			std::unordered_set<Entity*>::iterator end,
			Signer::Signature signature)
			: begin_(std::move(begin))
			, end_(std::move(end))
			, signature_(signature)
		{
			while (begin_ != end_ && !IsMatch(*begin_)) {
				++begin_;
			}
		}

		bool IsMatch(const Entity* entity) const {
			return Signer::IsMatch(entity->GetSignature(), signature_);
		}
	};

	using Action = std::function<void(Entity&)>;

	~EntityManager() {
		for (const auto it : entities_) {
			delete it;
		}

		entities_.clear();
	}

	template<typename First, typename... Rest>
	void For(Action action) const {
		static const Signer::Signature sample = Signer::GetSignature<First, Rest...>();

		for (const auto it : entities_) {
			if (Signer::IsMatch(it->GetSignature(), sample)) {
				action(*it);
			}			
		}
	}

	template<typename First, typename... Rest>
	Iterator GetIterator() {
		static const Signer::Signature sample = Signer::GetSignature<First, Rest...>();
		return GetIterator(sample);
	}

	Iterator GetIterator(Signer::Signature signature) {
		return {entities_.begin(), entities_.end(), signature};
	}

	Entity& CreateEntity() {
		auto it = entities_.insert(new Entity());
		return *(*it.first);
	}
	
	bool RemoveEntity(Entity& entity) {
		const auto it = entities_.find(&entity);

		if (it != entities_.end()) {
			delete *it;
			entities_.erase(it);
		}

		return false;
	}

private:
	std::unordered_set<Entity*> entities_;
};

} // namespace ecs

