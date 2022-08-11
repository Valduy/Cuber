#pragma once

#include <map>
#include <tuple>
#include <utility>
#include "Signature.h"
#include "IFamily.h"

namespace ash {

template<typename... Args>
class Family : public IFamily {
public:
	using Tuple = std::tuple<Entity&, Args&...>;

private:

	#pragma region Node

	struct Node {
		Tuple data;
		std::shared_ptr<Node> next;
		std::shared_ptr<Node> prev;

		Node(Tuple tuple)
			: data(std::move(tuple))
			, next(nullptr)
			, prev(nullptr)
		{}
	};

	#pragma endregion Node

public:

	#pragma region Iterator

	class Iterator {
		
		friend class Family;

	public:
		Tuple& Get() const {
			return temp_->data;
		}

		bool HasCurrent() const {
			return temp_ != nullptr;
		}

		void Next() {
			temp_ = temp_->next;
		}

		Tuple& operator*() const {
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
		std::shared_ptr<Node> temp_;

		Iterator(std::shared_ptr<Node> ptr)
			: temp_(std::move(ptr))
		{}
	};

	#pragma endregion Iterator

	Iterator GetIterator() {
		return Iterator(head_);
	}

	Iterator begin() {
		return Iterator(head_);
	}

	Iterator end() {
		return Iterator(nullptr);
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

	bool TryAddEntity(Entity& entity) override {
		if (nodes_map_.contains(&entity)) {
			return false;
		}

		if (IsMatch(entity)) {
			AddEntity(entity);
			return true;
		}

		return false;
	}

	bool RemoveEntity(Entity& entity) override {
		auto it = nodes_map_.find(&entity);

		if (it != nodes_map_.end()) {
			Remove(it->second);
			nodes_map_.erase(it);
			return true;
		}

		return false;
	}

	void OnComponentAddedToEntity(Entity& entity, Type::Id id) override {
		TryAddEntity(entity);
	}

	void OnComponentRemovedFromEntity(Entity& entity, Type::Id id) override {
		if (Signature<Args...>::Contains(id)) {
			RemoveEntity(entity);
		}
	}

	#pragma endregion IFamily

private:
	template<typename...>
	struct TupleBuilder;

	template<typename Head, typename... Tail>
	struct TupleBuilder<Head, Tail...> {
		static std::tuple<Head&, Tail&...> Create(Entity& entity) {
			std::tuple<Head&> head(entity.Get<Head>());
			auto tail = TupleBuilder<Tail...>::Create(entity);
			return std::tuple_cat(head, tail);
		}
	};

	template<>
	struct TupleBuilder<> {
		static std::tuple<> Create(Entity& entity) {
			std::tuple<> head;
			return head;
		}
	};

	std::shared_ptr<Node> head_;
	std::shared_ptr<Node> tail_;
	std::map<Entity*, std::shared_ptr<Node>> nodes_map_ = {};

	static Tuple CreateTuple(Entity& entity) {
		std::tuple<Entity&> head(entity);
		std::tuple<Args&...> tail = TupleBuilder<Args...>::Create(entity);
		return std::tuple_cat(head, tail);
	}
		
	void AddEntity(Entity& entity) {
		auto data = CreateTuple(entity);
		std::shared_ptr<Node> node = std::make_shared<Node>(std::move(data));
		Insert(node);
		nodes_map_[&entity] = node;
	}

	void Insert(std::shared_ptr<Node> node) {
		if (head_ == nullptr) {
			node->prev = nullptr;
			node->next = nullptr;
			head_ = node;
			tail_ = node;
		} else {
			tail_->next = node;
			node->prev = tail_;
			node->next = nullptr;
			tail_ = node;
		}
	}

	void Remove(std::shared_ptr<Node> node) {
		if (head_ == node) {
			head_ = node->next;
		}
		if (tail_ == node) {
			tail_ = tail_->prev;
		}
		if (node->prev != nullptr) {
			node->prev->next = node->next;
		}
		if (node->next != nullptr) {
			node->next->prev = node->prev;
		}
	}
};

} // namespace ash