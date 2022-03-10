#pragma once

namespace graph {

class MouseState {
friend class Window;

public:
	MouseState()
		: x_(0)
		, y_(0)
	{}

	float GetX() const {
		return x_;
	}

	float GetY() const {
		return y_;
	}

private:
	float x_;
	float y_;

	void SetX(float x) {
		x_ = x;
	}

	void SetY(float y) {
		y_ = y;
	}
};

}