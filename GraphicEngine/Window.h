#pragma once

#include <Windows.h>
#include <Windowsx.h>

#include "KeyboardState.h"
#include "MouseState.h"

namespace graph {

    class Window {
    public:
        HWND GetHandler();
        unsigned int GetWidth() const;
        unsigned int GetHeight() const;
        float GetAspectRatio() const;
        KeyboardState& GetKeyboardState();
        MouseState& GetMouseState();

        Window(HINSTANCE instance, LPCWSTR window_name, unsigned int width, unsigned int height);
        void Show();

    private:
        KeyboardState keyboard_state_;
        MouseState mouse_state_;
        HINSTANCE instance_;
        LPCWSTR window_name_;
        unsigned int width_;
        unsigned int height_;
        HWND handle_;        

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

        HWND RegisterAndCreateWindow();
        void RegisterWindowClass();
        HWND CreateWindowInstance();
    };

} // namespace engine