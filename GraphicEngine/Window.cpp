#include "pch.h"
#include "Window.h"

#include <hidusage.h>
#include <iostream>

HWND graph::Window::GetHandler() const {
	return handle_;
}

LONG graph::Window::GetWidth() const {
	return width_;
}

LONG graph::Window::GetHeight() const {
	return height_;
}

FLOAT graph::Window::GetAspectRatio() const {
	const auto width = static_cast<float>(width_);
	const auto height = static_cast<float>(height_);
	return width / height;
}

graph::KeyboardState& graph::Window::GetKeyboardState() {
	return keyboard_state_;
}

graph::MouseState& graph::Window::GetMouseState() {
	return mouse_state_;
}

void graph::Window::ResetInputs() {
	mouse_state_.Reset();
}

graph::Window::Window(HINSTANCE instance, LPCWSTR window_name, LONG width, LONG height)
	: instance_(instance)
	, window_name_(window_name)
	, width_(width)
	, height_(height)
	, handle_(RegisterAndCreateWindow())
{
	RegisterRowInputDevice();
}

void graph::Window::Show() {
	ShowWindow(handle_, SW_SHOW);
	SetForegroundWindow(handle_);
	SetFocus(handle_);
	ShowCursor(false);
}

LRESULT graph::Window::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	Window* window = nullptr;

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
		return 0;
	}		
	case WM_INPUT: {
		UINT size = sizeof(RAWINPUT);
		static BYTE lpb[sizeof(RAWINPUT)];
		const auto raw_input = reinterpret_cast<HRAWINPUT>(lparam);

		GetRawInputData(raw_input, RID_INPUT, lpb, &size, sizeof(RAWINPUTHEADER));

		const auto raw = reinterpret_cast<RAWINPUT*>(lpb);

		if (raw->header.dwType == RIM_TYPEMOUSE) {
			RECT rect;
			GetWindowRect(window->handle_, &rect);
			const LONG center_x = rect.left + window->width_ / 2;
			const LONG center_y = rect.top + window->height_ / 2;
			SetCursorPos(center_x, center_y);

			window->mouse_state_.SetDeltaX(raw->data.mouse.lLastX);
			window->mouse_state_.SetDeltaY(raw->data.mouse.lLastY);
		}
		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
			if (raw->data.keyboard.Flags == RI_KEY_MAKE || raw->data.keyboard.Flags & RI_KEY_E0) {
				window->keyboard_state_.AddPressedKey(raw->data.keyboard.VKey);
			}
			if (raw->data.keyboard.Flags & RI_KEY_BREAK) {
				window->keyboard_state_.RemovePressedKey(raw->data.keyboard.VKey);
			}
		}

		return 0;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}
	default: {
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	}	
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

// TODO: if false then registration failed
BOOL graph::Window::RegisterRowInputDevice() {
	RAWINPUTDEVICE Rid[2];
	Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid[0].dwFlags = 0;
	Rid[0].hwndTarget = handle_;

	Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;
	Rid[1].dwFlags = 0;
	Rid[1].hwndTarget = handle_;

	return RegisterRawInputDevices(Rid, 2, sizeof(Rid[0]));
}
