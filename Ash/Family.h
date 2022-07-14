#pragma once

#include <map>
#include <set>
#include <tuple>
#include <functional>
#include "Signature.h"
#include "Entity.h"

namespace ash {

class IFamily {
public: 
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
class Family {
public:

	using Node = std::tuple<Entity&, Args&...>;

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
		std::function<bool (Entity*)> filter_;

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

	Iterator GetIterator() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
	}

	Iterator begin() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
	}

	Iterator end() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.end());
	}

	bool TryAddEntity(Entity& entity) {
		if (nodes_.find(&entity) != nodes_.end()) {
			return false;
		}

		if (IsMatch(entity)) {
			AddEntity(entity);
			return true;
		}
		
		return false;
	}

	static bool IsMatch(Entity& entity) {
		for (auto id : Signature<Args...>::kIds) {
			if (!entity.Contain(id)) {
				return false;
			}
		}

		return true;
	}
	
	void OnComponentAddedToEntity(Entity& entity, Type::Id id) {
		if (nodes_.find(&entity) != nodes_.end()) {
			return;
		}

		if (IsMatch(entity)) {
			to_add_.insert(&entity);
			to_remove_.erase(&entity);
		}
	}

	void OnComponentRemovedFromEntity(Entity& entity, Type::Id id) {
		if (Signature<Args...>::Contains(id)) {
			to_add_.erase(&entity);
			to_remove_.insert(&entity);
		}
	}

	void Invalidate() {
		for (auto entity : to_remove_) {
			nodes_.erase(entity);
		}

		for (auto entity : to_add_) {
			AddEntity(*entity);
		}
	}

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

} // namespace ash