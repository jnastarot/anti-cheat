#include "stdafx.h"
#include "Window.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {

	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}



window::window(std::string name, std::string winclass){
	WRegisterClass(winclass);

	wnd = CreateWindowA(winclass.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, GetModuleHandle(0), nullptr);

	ShowWindow(wnd, SW_SHOW);
	UpdateWindow(wnd);
}


window::~window(){
	DestroyWindow(this->wnd);
}


bool window::onMessage() {
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}
HWND window::getSelfHandle() {
	return this->wnd;
}

ATOM window::WRegisterClass(std::string winclass) {
	WNDCLASSEXA wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = GetModuleHandle(0);
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = winclass.c_str();
	wcex.hIconSm = 0;
	return RegisterClassEx(&wcex);
}