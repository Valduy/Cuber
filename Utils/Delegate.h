#pragma once

#include <list>
#include <algorithm>
#include "EventHandler.h"

namespace utils {

template<typename... TParams>
class Delegate {
private:
	using Iterator = std::list<IEventHandler<TParams...>*>::const_iterator;

public:

	Delegate()
		: handlers_ {}
		, current_it_(handlers_.end())
		, is_current_removed_(false)
	{}

	~Delegate() {
		for (auto handler : handlers_) {
			delete handler;
		}

		handlers_.clear();
	}

	template<typename TObject>
	bool Attach(TObject& object, EventHandler<TObject, TParams...>::TMethod method) {
		auto handler = new EventHandler<TObject, TParams...>(object, method);

		if (FindIterator(*handler) == handlers_.end()) {
			handlers_.push_back(handler);
			return true;
		}

		delete handler;
		return false;
	}

	template<typename TObject>
	bool Detach(TObject& object, EventHandler<TObject, TParams...>::TMethod method) {
		EventHandler<TObject, TParams...> handler(object, method);		
		auto it = FindIterator(handler);

		if (it != handlers_.end()) {
			if (it == current_it_) {
				is_current_removed_ = true;
			}
			else {
				delete *it;
				handlers_.erase(it);				
			}

			return true;
		}

		return false;
	}

	void Notify(TParams... params) {
		current_it_ = handlers_.begin();
		is_current_removed_ = false;

		while (current_it_ != handlers_.end()) {
			(*current_it_)->Call(params...);

			if (is_current_removed_) {
				is_current_removed_ = false;
				auto removed = current_it_;				
				++current_it_;
				delete *removed;
				handlers_.erase(removed);
			}
			else {
				++current_it_;
			}			
		}
	}

private:
	std::list<IEventHandler<TParams...>*> handlers_;
	Iterator current_it_;
	bool is_current_removed_;

	Iterator FindIterator(const IEventHandler<TParams...>& handler) {
		return std::find_if(
			handlers_.begin(),
			handlers_.end(),
			[&handler](const IEventHandler<TParams...>* h) {
				return h->IsEqual(handler);
			});
	}
};

} // namespace utils

