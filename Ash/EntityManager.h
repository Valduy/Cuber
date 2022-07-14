#pragma once

#include <map>
#include <set>
#include <vector>
#include <tuple>
#include <functional>
#include "Type.h"
#include "Entity.h"
#include "Signature.h"

namespace ash {

class EntityManager {
public:

	#pragma region Family

	class IFamily {

		friend EntityManager;

	protected:
		virtual ~IFamily() {}

		virtual void Invalidate() = 0;

		// Method try to add an entity into family. If this entity contains all required components,
		// the family store it and create a node. Otherwise, the family ignores this entity.
		// Note: this method should only be called from the entity manager when the family is created, to initialize
		// that family by initial entities.
		virtual bool TryAddEntity(Entity& entity) = 0;

		// Method try to remove entity from family. If entity was contained in the family, method return true. Otherwise,
		// method return false.
		// Note: this method should be called from the entity manager on entity deletation.
		virtual bool RemoveEntity(Entity& entity) = 0;

		virtual void OnComponentAddedToEntity(Entity& entity, Type::Id id) = 0;
		virtual void OnComponentRemovedFromEntity(Entity& entity, Type::Id id) = 0;
	};

	template<typename... Args>
	class Family : public IFamily {
	public:

		using Node = std::tuple<Entity&, Args&...>;

		#pragma region Iterator

		class Iterator {

			friend class Family;

		public:
			Node& Get() const {
				return temp_->second;
			}

			bool HasCurrent() const {
				return temp_ != end_;
			}

			void Next() {
				do {
					++temp_;
				} while (temp_ != end_ && !filter_(temp_->first));
			}

			Node& operator*() const {
				return Get();
			}

			Iterator& operator++() {
				Next();
				return *this;
			}

			Iterator operator++(int) {
				Iterator tmp = *this;
				Next();
				return tmp;
			}

			friend bool operator== (const Iterator& lhs, const Iterator& rhs) {
				return lhs.temp_ == rhs.temp_;
			};

			friend bool operator!= (const Iterator& lhs, const Iterator& rhs) {
				return lhs.temp_ != rhs.temp_;
			};

		private:
			typename std::map<Entity*, Node>::iterator begin_;
			typename std::map<Entity*, Node>::iterator end_;
			typename std::map<Entity*, Node>::iterator temp_;
			std::function<bool(Entity*)> filter_;

			Iterator(
				typename std::map<Entity*, Node>::iterator begin,
				typename std::map<Entity*, Node>::iterator end,
				typename std::map<Entity*, Node>::iterator temp,
				std::function<bool(Entity*)> filter)
				: begin_(begin)
				, end_(end)
				, temp_(temp)
				, filter_(filter)
			{
				while (temp_ != end_ && !filter_(temp_->first)) {
					++temp_;
				}
			}
		};

		#pragma endregion Iterator

		static bool IsMatch(Entity& entity) {
			for (auto id : Signature<Args...>::kIds) {
				if (!entity.Contain(id)) {
					return false;
				}
			}

			return true;
		}

		Iterator GetIterator() {
			return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
		}

		Iterator begin() {
			return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
		}

		Iterator end() {
			return GetIterator(nodes_.begin(), nodes_.end(), nodes_.end());
		}

	protected:

		#pragma region IFamiliy

		void Invalidate() override {
			for (auto entity : to_remove_) {
				nodes_.erase(entity);
			}

			for (auto entity : to_add_) {
				AddEntity(*entity);
			}
		}

		bool TryAddEntity(Entity& entity) override {
			if (nodes_.find(&entity) != nodes_.end()) {
				return false;
			}

			if (IsMatch(entity)) {
				AddEntity(entity);
				return true;
			}

			return false;
		}

		bool RemoveEntity(Entity& entity) override {
			auto it = nodes_.find(&entity);

			if (it != nodes_.end()) {
				nodes_.erase(it);
				return true;
			}

			return false;
		}

		void OnComponentAddedToEntity(Entity& entity, Type::Id id) override {
			if (nodes_.find(&entity) != nodes_.end()) {
				return;
			}

			if (IsMatch(entity)) {
				to_add_.insert(&entity);
				to_remove_.erase(&entity);
			}
		}

		void OnComponentRemovedFromEntity(Entity& entity, Type::Id id) override {
			if (Signature<Args...>::Contains(id)) {
				to_add_.erase(&entity);
				to_remove_.insert(&entity);
			}
		}

		#pragma endregion IFamily

	private:
		template<typename...>
		struct Componenter;

		template<typename Head, typename... Tail>
		struct Componenter<Head, Tail...> {
			static std::tuple<Head&, Tail&...> Create(Entity& entity) {
				std::tuple<Head&> head(entity.Get<Head>());
				auto tail = Componenter<Tail...>::Create(entity);
				return std::tuple_cat(head, tail);
			}
		};

		template<>
		struct Componenter<> {
			static std::tuple<> Create(Entity& entity) {
				std::tuple<> head;
				return head;
			}
		};

		std::map<Entity*, Node> nodes_;
		std::set<Entity*> to_add_;
		std::set<Entity*> to_remove_;

		static Node CreateNode(Entity& entity) {
			std::tuple<Entity&> head(entity);
			std::tuple<Args&...> tail = Componenter<Args...>::Create(entity);
			return std::tuple_cat(head, tail);
		}

		void AddEntity(Entity& entity) {
			nodes_.insert({ &entity, CreateNode(entity) });
		}

		Iterator GetIterator(
			typename std::map<Entity*, Node>::iterator begin,
			typename std::map<Entity*, Node>::iterator end,
			typename std::map<Entity*, Node>::iterator temp)
		{
			return {
				begin,
				end,
				temp,
				[&](Entity* e) { return !IsRemoved(e); }
			};
		}

		bool IsRemoved(Entity* entity) {
			auto it = to_remove_.find(entity);
			return it != to_remove_.end();
		}
	};

	#pragma endregion Family

	Entity& Create() {
		Entity* entity = new Entity();
		entities_.insert(entity);
		return *entity;
	}

	void Destroy(Entity& entity) {
		// Cash for deletation and actually delete on invalidation
		destroy_pool_.push_back(&entity);

		//auto it = entities_.find(&entity);
		//
		//if (it != entities_.end()) {
		//	entities_.erase(it);
		//	return true;
		//}

		//return false;
	}

	void Invalidate() {
		for (auto it : families_) {
			auto family = it.second;
			family->Invalidate();

			for (auto entity : destroy_pool_) {
				family->RemoveEntity(*entity);
			}
		}

		for (auto entity : destroy_pool_) {
			entities_.erase(entity);
		}

		destroy_pool_.clear();
	}

	template<typename... Args>
	Family<Args...>& Filter() {
		Type::Id id = Type::GetId<Family<Args...>>();
		auto it = families_.find(id);

		if (it != families_.end()) {
			return static_cast<Family<Args...>*>(*it);
		}

		auto family = new Family<Args...>();
		families_[id] = family;

		for (auto entity : entities_) {
			family.TryAddEntity(*entity);
		}

		return *family;
	}

private:
	std::map<Type::Id, IFamily*> families_;
	std::set<Entity*> entities_;
	std::vector<Entity*> destroy_pool_;

};

} // namespace ash