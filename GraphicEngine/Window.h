#pragma once

#include <Windows.h>

namespace graph {

    class Window {
    public:
        HWND GetHandler();
        unsigned int GetWidth();
        unsigned int GetHeight();

        Window(HINSTANCE instance, LPCWSTR window_name, unsigned int width, unsigned int height);
        void Show();

    private:
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