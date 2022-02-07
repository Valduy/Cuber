#pragma once

#include <map>
#include "Callback.h"

namespace ecs {

	template<typename TSubject, typename TArgs>
	class EventHandler {
		friend TSubject;

	public:
		~EventHandler() {
			for (auto it : observers_) {
				delete it->second;
			}

			observers_.clear();
		}

		template<typename TObserver>
		void Attach(TObserver* observer, Callback<TObserver, TArgs>::Action action) {
			auto it = observers_.find(observer);

			if (it != observers_.end()) {
				delete it->second;
			}

			observers_[observer] = new Callback(observer, action);
		}

		template<typename TObserver>
		void Detach(TObserver* observer) {
			auto it = observers_.find(observer);

			if (it != observers_.end()) {
				delete it->second;
				observers_.erase(it);
			}
		}

	private:
		std::map<void*, ICallable*> observers_;

		void Notify(TArgs args) {
			for (auto it : observers_) {
				it->second->Call(args);
			}
		}

	};

} // namespace ecs
