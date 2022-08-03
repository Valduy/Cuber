#pragma once

#include "IWindow.h"
#include "KeyboardState.h"
#include "MouseState.h"

#include <Windows.h>

namespace graph {

class Window : public IWindow {
public:
    HWND GetHandler() const;
    LONG GetWidth() const override;
    LONG GetHeight() const override;
    FLOAT GetAspectRatio() const override;
    KeyboardState& GetKeyboardState() override;
    MouseState& GetMouseState() override;
    void ResetInputs();

    Window(HINSTANCE instance, LPCWSTR window_name, LONG width, LONG height);
    void Show();

private:
    KeyboardState keyboard_state_;
    MouseState mouse_state_;
    HINSTANCE instance_;
    LPCWSTR window_name_;
    LONG width_;
    LONG height_;
    HWND handle_;        

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    HWND RegisterAndCreateWindow();
    void RegisterWindowClass();
    HWND CreateWindowInstance();
    BOOL RegisterRowInputDevice();
};

} // namespace engine