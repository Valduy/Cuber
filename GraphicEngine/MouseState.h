#pragma once

namespace graph {

class MouseState {
friend class Window;

public:
	MouseState()
		: x_(0)
		, y_(0)
	{}

	int GetX() const {
		return x_;
	}

	int GetY() const {
		return y_;
	}

private:
	int x_;
	int y_;

	void SetX(int x) {
		x_ = x;
	}

	void SetY(int y) {
		y_ = y;
	}
};

}