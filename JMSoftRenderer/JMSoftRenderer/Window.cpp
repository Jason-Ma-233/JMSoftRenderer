#include "Window.h"
#include <time.h>
#include <sstream>

std::unordered_map<HWND, Window*> Window::screen_refs = std::unordered_map<HWND, Window*>();

LRESULT CALLBACK screen_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	auto result = Window::screen_refs.find(hWnd);
	if (result == Window::screen_refs.end())
		return DefWindowProc(hWnd, msg, wParam, lParam);

	Window* window_ptr = result->second;
	switch (msg) {
	case WM_CLOSE: window_ptr->screen_running = false; break;
	case WM_KEYDOWN: window_ptr->screen_keys[wParam & 511] = 1; break;
	case WM_KEYUP: window_ptr->screen_keys[wParam & 511] = 0; break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

Window::Window(int w, int h, const TCHAR* title) {
	WNDCLASS wc = { CS_BYTEALIGNCLIENT, (WNDPROC)screen_events, 0, 0, 0,
		nullptr, nullptr, nullptr, nullptr, _T("SCREEN_WINDOW") };
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), w, -h, 1, 32, BI_RGB,
		(DWORD)w * h * 4, 0, 0, 0, 0 } };
	RECT rect = { 0, 0, w, h };
	int wx, wy, sx, sy;
	LPVOID ptr;
	HDC hDC;

	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	if (!RegisterClass(&wc)) return;

	window_title = title;
	screen_handle = CreateWindow(_T("SCREEN_WINDOW"), title,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		0, 0, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);
	if (screen_handle == nullptr) return;

	hDC = GetDC(screen_handle);
	screen_dc = CreateCompatibleDC(hDC);
	ReleaseDC(screen_handle, hDC);

	screen_hb = CreateDIBSection(screen_dc, &bi, DIB_RGB_COLORS, &ptr, 0, 0);
	if (screen_hb == nullptr) return;

	screen_ob = (HBITMAP)SelectObject(screen_dc, screen_hb);
	screen_fb = (unsigned char*)ptr;
	screen_w = w;
	screen_h = h;

	AdjustWindowRect(&rect, GetWindowLong(screen_handle, GWL_STYLE), 0);
	wx = rect.right - rect.left;
	wy = rect.bottom - rect.top;
	sx = (GetSystemMetrics(SM_CXSCREEN) - wx) / 2;
	sy = (GetSystemMetrics(SM_CYSCREEN) - wy) / 2;
	if (sy < 0) sy = 0;
	SetWindowPos(screen_handle, nullptr, sx, sy, wx, wy, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(screen_handle);

	ShowWindow(screen_handle, SW_NORMAL);
	dispatch();

	memset(screen_keys, 0, sizeof(int) * 512);
	memset(screen_fb, 0, w * h * 4);

	screen_running = true;
	screen_refs.emplace(screen_handle, this);
}

Window::~Window() {
	destory();
}

void Window::dispatch() {
	if (!screen_running) return;
	MSG msg;
	while (true) {
		if (!PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) break;
		if (!GetMessage(&msg, nullptr, 0, 0)) break;
		DispatchMessage(&msg);
	}
}

void Window::update() {
	if (!screen_running) return;
	HDC hDC = GetDC(screen_handle);
	BitBlt(hDC, 0, 0, screen_w, screen_h, screen_dc, 0, 0, SRCCOPY);
	ReleaseDC(screen_handle, hDC);
	dispatch();
	update_fps();
}

void Window::destory() {
	screen_running = false;
	if (screen_dc) {
		if (screen_ob) {
			SelectObject(screen_dc, screen_ob);
			screen_ob = nullptr;
		}
		DeleteDC(screen_dc);
		screen_dc = nullptr;
	}
	if (screen_hb) {
		DeleteObject(screen_hb);
		screen_hb = nullptr;
	}
	if (screen_handle) {
		screen_refs.erase(screen_handle);
		CloseWindow(screen_handle);
		screen_handle = nullptr;
	}
}

bool Window::setTitle(const TCHAR* title) {
	return SetWindowText(screen_handle, title);
}

void Window::update_fps() { // ms
	static int fps = 0;
	static clock_t lastTime = clock(); // ms
	static int frameCount = 0;
	++frameCount;
	clock_t curTime = clock();
	if (curTime - lastTime > 1000) // 取固定时间间隔为1秒
	{
		current_fps = frameCount;
		frameCount = 0;
		lastTime = curTime;

		std::ostringstream s;
		s << window_title << " Fps:" << current_fps;
		setTitle(_T(s.str().c_str()));
	}
}

int* Window::operator()(unsigned int index) {
	if (!screen_running) return nullptr;
	return (int*)screen_fb + index;
}

int* Window::operator()(int x, int y) {
	if (!screen_running) return nullptr;
	return (int*)screen_fb + (y * screen_w + x);
}