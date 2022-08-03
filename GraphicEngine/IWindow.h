#pragma once
#include "KeyboardState.h"
#include "MouseState.h"

#include <intsafe.h>
#include <minwindef.h>

namespace graph {

class IWindow {
public:
	virtual ~IWindow() = default;
    virtual LONG GetWidth() const = 0;
    virtual LONG GetHeight() const = 0;
    virtual FLOAT GetAspectRatio() const = 0;
    virtual KeyboardState& GetKeyboardState() = 0;
    virtual MouseState& GetMouseState() = 0;
};

} // namespace graph