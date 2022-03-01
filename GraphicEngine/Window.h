#pragma once

#include <Windows.h>

#include "KeyboardState.h"

namespace graph {

    class Window {
    public:
        HWND GetHandler();
        unsigned int GetWidth();
        unsigned int GetHeight();
        KeyboardState& GetKeyboardState();

        Window(HINSTANCE instance, LPCWSTR window_name, unsigned int width, unsigned int height);
        void Show();

    private:
        KeyboardState keyboard_state_;
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