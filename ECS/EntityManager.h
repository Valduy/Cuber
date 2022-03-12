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
		std::vector<Entity*>::iterator begin_;
		std::vector<Entity*>::iterator end_;
		Signer::Signature signature_;

		Iterator(
			std::vector<Entity*>::iterator begin, 
			std::vector<Entity*>::iterator end,
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
		Entity* entity = new Entity();
		entities_.push_back(entity);
		return *entity;
	}
	
	bool RemoveEntity(Entity& entity) {
		for (auto it = entities_.begin(); it != entities_.end(); ++it) {
			if (*it == &entity) {
				entities_.erase(it);
				return true;
			}
		}

		return false;
	}

private:
	std::vector<Entity*> entities_;
};

} // namespace ecs

