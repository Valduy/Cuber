#include "pch.h"
#include <vector>
#include <algorithm>
#include "../Utils/Delegate.h"
#include "../Utils/Event.h"

namespace utils_tests {

class BooleanObserver {
public:
	bool IsFired() const {
		return is_fired_;
	}

	template<typename... Args>
	void Callback(Args... params) {
		is_fired_ = true;
	}

private:
	bool is_fired_ = false;
};

class Subject {
public:
	utils::Delegate<Subject&> delegate_;

public:
	const utils::Event<Subject&> Event = { delegate_ };

	void Notify() {
		delegate_.Notify(*this);
	}
};

class SubjectObserver {
public:
	size_t GetFiresCount() const {
		return count;
	}

	void Callback(Subject& subject) {
		count += 1;
		subject.Event.Detach(*this, &SubjectObserver::Callback);
	}

private:
	size_t count = 0;
};

class DelegateFixture : public testing::TestWithParam<size_t> {};

bool IsAllEventsFired(const std::vector<BooleanObserver>& observers);
bool IsAllEventsNotFired(const std::vector<BooleanObserver>& observers);
bool IsAllFiresCountEqual(const std::vector<SubjectObserver>& observers, size_t count);

TEST_P(DelegateFixture, Notify_AttachMethodsToDelegateWithoutParams_EventsAreFired) {
	utils::Delegate delegate;
	std::vector<BooleanObserver> observers(GetParam());

	for (auto& o : observers) {
		delegate.Attach(o, &BooleanObserver::Callback);
	}

	delegate.Notify();

	EXPECT_TRUE(IsAllEventsFired(observers));
}

TEST_P(DelegateFixture, Notify_AttachMethodsToDelegateWithParams_EventsAreFired) {
	utils::Delegate<int, double, float> delegate;
	std::vector<BooleanObserver> observers(GetParam());

	for (auto& o : observers) {
		delegate.Attach(o, &BooleanObserver::Callback);
	}

	delegate.Notify(1, 2.0, 3.0);

	EXPECT_TRUE(IsAllEventsFired(observers));
}

TEST_P(DelegateFixture, Detach_AttachAndDetachMethodsToDelegateWithoutParams_EventsAreNotFired) {
	utils::Delegate delegate;
	std::vector<BooleanObserver> observers(GetParam());

	for (auto& o : observers) {
		delegate.Attach(o, &BooleanObserver::Callback);
	}

	for (auto& o : observers) {
		delegate.Detach(o, &BooleanObserver::Callback);
	}

	delegate.Notify();

	EXPECT_TRUE(IsAllEventsNotFired(observers));
}

TEST_P(DelegateFixture, Detach_AttachAndDetachMethodsToDelegateWithParams_EventsAreNotFired) {
	utils::Delegate<int, double, float> delegate;
	std::vector<BooleanObserver> observers(GetParam());

	for (auto& o : observers) {
		delegate.Attach(o, &BooleanObserver::Callback);
	}

	for (auto& o : observers) {
		delegate.Detach(o, &BooleanObserver::Callback);
	}

	delegate.Notify(1, 2.0, 3.0);

	EXPECT_TRUE(IsAllEventsNotFired(observers));
}

TEST_P(DelegateFixture, Attach_AttachOneMethodMultipleTime_EventFiredOnce) {
	Subject subject;
	std::vector<SubjectObserver> observers(GetParam());

	for (auto& o : observers) {
		subject.Event.Attach(o, &SubjectObserver::Callback);
		subject.Event.Attach(o, &SubjectObserver::Callback);
	}

	subject.Notify();

	EXPECT_TRUE(IsAllFiresCountEqual(observers, 1));
}

TEST_P(DelegateFixture, Detach_DetachWhenEventFired_CallbackDetached) {
	Subject subject;
	std::vector<SubjectObserver> observers(GetParam());

	for (auto& o : observers) {
		subject.Event.Attach(o, &SubjectObserver::Callback);
	}

	subject.Notify();
	subject.Notify();

	EXPECT_TRUE(IsAllFiresCountEqual(observers, 1));
}

bool IsAllEventsFired(const std::vector<BooleanObserver>& observers) {
	return std::all_of(
		observers.begin(),
		observers.end(),
		[](const BooleanObserver& o) { return o.IsFired(); });
}

bool IsAllEventsNotFired(const std::vector<BooleanObserver>& observers) {
	return std::all_of(
		observers.begin(),
		observers.end(),
		[](const BooleanObserver& o) { return !o.IsFired(); });
}

bool IsAllFiresCountEqual(const std::vector<SubjectObserver>& observers, size_t count) {
	return std::all_of(
		observers.begin(),
		observers.end(),
		[&](const SubjectObserver& o) { return o.GetFiresCount() == count; });
}

INSTANTIATE_TEST_CASE_P(
	Delegate,
	DelegateFixture,
	testing::Values(0, 1, 2, 3), );

} // namespace utils_tests