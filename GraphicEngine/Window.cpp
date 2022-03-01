#include "pch.h"
#include "Window.h"

HWND graph::Window::GetHandler() {
	return handle_;
}

unsigned graph::Window::GetWidth() {
	return width_;
}

unsigned graph::Window::GetHeight() {
	return height_;
}

graph::KeyboardState& graph::Window::GetKeyboardState() {
	return keyboard_state_;
}

graph::Window::Window(HINSTANCE instance, LPCWSTR window_name, unsigned width, unsigned height)
	: instance_(instance)
	, window_name_(window_name)
	, width_(width)
	, height_(height)
	, handle_(RegisterAndCreateWindow())
{}

void graph::Window::Show() {
	ShowWindow(handle_, SW_SHOW);
	SetForegroundWindow(handle_);
	SetFocus(handle_);
	ShowCursor(true);
}

LRESULT graph::Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	Window* window;

	if (msg == WM_CREATE) {
		const auto create_struct = reinterpret_cast<CREATESTRUCT*>(lparam);
		window = static_cast<Window*>(create_struct->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
	} else {
		window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	switch (msg) {
	case WM_SIZE: {
		window->width_ = LOWORD(lparam);
		window->height_ = HIWORD(lparam);
		break;
	}		
	case WM_KEYDOWN: {
		const auto key = static_cast<unsigned int>(wparam);
		window->keyboard_state_.AddPressedKey(key);
		return 0;
	}		
	case WM_KEYUP: {
		const auto key = static_cast<unsigned int>(wparam);
		window->keyboard_state_.RemovePressedKey(key);
		return 0;
	}		
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

HWND graph::Window::RegisterAndCreateWindow() {
	RegisterWindowClass();
	return CreateWindowInstance();
}

void graph::Window::RegisterWindowClass() {
	WNDCLASSEX window_class;
	window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	window_class.lpfnWndProc = WndProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = instance_;
	window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	window_class.hIconSm = window_class.hIcon;
	window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
	window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	window_class.lpszMenuName = nullptr;
	window_class.lpszClassName = window_name_;
	window_class.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&window_class);
}

HWND graph::Window::CreateWindowInstance() {
	RECT window_rect = { 0, 0, static_cast<LONG>(width_), static_cast<LONG>(height_) };
	AdjustWindowRect(&window_rect, WS_OVERLAPPEDWINDOW, FALSE);

	return CreateWindowEx(
		WS_EX_APPWINDOW,
		window_name_,
		window_name_,
		WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME,
		(GetSystemMetrics(SM_CXSCREEN) - width_) / 2,
		(GetSystemMetrics(SM_CYSCREEN) - height_) / 2,
		window_rect.right - window_rect.left,
		window_rect.bottom - window_rect.top,
		nullptr,
		nullptr,
		instance_,
		this);
}
