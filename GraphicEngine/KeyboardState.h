#pragma once

#include <unordered_set>

namespace graph {

class KeyboardState {

friend class Window;

public:
	KeyboardState() {
		
	}

	bool IsKeyDown(unsigned int key) const {
		return keys_.count(key);
	}

private:
	std::unordered_set<unsigned int> keys_;

	void AddPressedKey(unsigned int key) {
		keys_.insert(key);
	}

	void RemovePressedKey(unsigned int key) {
		keys_.erase(key);
	}
};

} // namespace graph