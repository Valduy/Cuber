#pragma once

#include <unordered_set>
#include <Windows.h>

namespace graph {

enum class Keys {	
	kA = 0x41,
	kD = 0x44,
	kS = 0x53,
	kW = 0x57,
	kShift = VK_SHIFT,
	kControl = VK_CONTROL,
	kSpace = VK_SPACE,
	kLeft = VK_LEFT,
	kUp = VK_UP,
	kRight = VK_RIGHT,
	kDown = VK_DOWN,	
};

class KeyboardState {
friend class Window;

public:
	bool IsKeyDown(unsigned int key) const {
		return keys_.count(key);
	}

	bool IsKeyDown(Keys key) const {
		return IsKeyDown(static_cast<unsigned int>(key));
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