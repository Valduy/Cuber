#pragma once

namespace graph {

class MouseState {
friend class Window;

public:
	MouseState()
		: x_(0)
		, y_(0)
	{}

	int GetDeltaX() const {
		return x_;
	}

	int GetDeltaY() const {
		return y_;
	}
	
private:
	int x_;
	int y_;

	void SetDeltaX(int x) {
		x_ = x;
	}

	void SetDeltaY(int y) {
		y_ = y;
	}

	void Reset() {
		x_ = 0;
		y_ = 0;
	}
};

}