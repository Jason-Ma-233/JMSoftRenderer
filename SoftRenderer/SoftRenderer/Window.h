#pragma once

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <unordered_map>
#include <windows.h>
#include <tchar.h>

#ifdef _MSC_VER
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")
#endif

class Window {
private:
	bool screen_running = false;
	int screen_w, screen_h;
	int screen_mx = 0, screen_my = 0, screen_mb = 0;
	int screen_keys[512];	            // 当前键盘按下状态
	HWND screen_handle = nullptr;		// 主窗口 HWND
	HDC screen_dc = nullptr;			// 配套的 HDC
	HBITMAP screen_hb = nullptr;		// DIB
	HBITMAP screen_ob = nullptr;		// 老的 BITMAP
	unsigned char *screen_fb = nullptr;	// frame buffer
	long screen_pitch = 0;
	int current_fps;

	static std::unordered_map<HWND, Window *> screen_refs;

	// win32 event handler
	friend LRESULT CALLBACK screen_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void update_fps();

public:
	Window(int w, int h, const TCHAR *title);  // 屏幕初始化
	~Window();  // 关闭屏幕

	bool is_run() { return screen_running; }
	bool is_key(unsigned int code) { return code >= 512 ? false : screen_keys[code]; }
	int get_fps() { return current_fps; }
	void dispatch();  // 处理消息
	void update();    // 显示 FrameBuffer
	void destory();   // 销毁
	bool setTitle(const TCHAR *title);

	// 返回FrameBuffer某个Pixel的Int32第一个颜色指针(每个pixel四字节对齐)
	int * operator()(unsigned int index = 0);
	int * operator()(int x, int y);
};

#endif