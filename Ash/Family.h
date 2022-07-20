#pragma once

#include <map>
#include <set>
#include <tuple>
#include <functional>
#include <utility>
#include "Signature.h"
#include "IFamily.h"

namespace ash {

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
		}

		friend bool operator!= (const Iterator& lhs, const Iterator& rhs) {
			return lhs.temp_ != rhs.temp_;
		}

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
			: begin_(std::move(begin))
			, end_(std::move(end))
			, temp_(std::move(temp))
			, filter_(std::move(filter))
		{
			while (temp_ != end_ && !filter_(temp_->first)) {
				++temp_;
			}
		}
	};

	#pragma endregion Iterator

	Iterator GetIterator() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
	}

	Iterator begin() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.begin());
	}

	Iterator end() {
		return GetIterator(nodes_.begin(), nodes_.end(), nodes_.end());
	}

	static bool IsMatch(Entity& entity) {
		for (auto id : Signature<Args...>::kIds) {
			if (!entity.Contain(id)) {
				return false;
			}
		}

		return true;
	}
	
	#pragma region IFamily

	void Invalidate() override {
		for (auto entity : to_remove_) {
			nodes_.erase(entity);
		}

		for (const auto entity : to_add_) {
			AddEntity(*entity);
		}
	}

	bool TryAddEntity(Entity& entity) override {
		if (nodes_.contains(&entity)) {
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
		if (nodes_.contains(&entity)) {
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
	struct NodeBuilder;

	template<typename Head, typename... Tail>
	struct NodeBuilder<Head, Tail...> {
		static std::tuple<Head&, Tail&...> Create(Entity& entity) {
			std::tuple<Head&> head(entity.Get<Head>());
			auto tail = NodeBuilder<Tail...>::Create(entity);
			return std::tuple_cat(head, tail);
		}
	};

	template<>
	struct NodeBuilder<> {
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
		std::tuple<Args&...> tail = NodeBuilder<Args...>::Create(entity);
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
		const auto it = to_remove_.find(entity);
		return it != to_remove_.end();
	}
};

} // namespace ash