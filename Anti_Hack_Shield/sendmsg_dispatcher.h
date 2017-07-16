#pragma once


bool validate_msg(DWORD msg) {
	switch (msg) {
		//выборка оконных сообщений эмуляция которых будет блокироваться

		//keyborad
	case WM_KEYDOWN:case WM_KEYUP:
		//mouse
	case WM_RBUTTONDOWN:case WM_LBUTTONDOWN:case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:case WM_RBUTTONUP:case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONUP: case WM_MBUTTONDBLCLK:
	case WM_MOUSEWHEEL: case WM_MOUSEHWHEEL:
	{
		printf("Device Emulation [SendMessage]\n");
		return false;
	}
	default:
		return true;
	}
}

int WINAPI h_DispatchClientMessage_W_win64(HWND* hwnd,
#ifdef _M_IX86
	DWORD aling_32,
#endif	
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {

	if (hwnd) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}

	return ((_DispatchClientMessagex64)o_DispatchClientMessage_W)(hwnd,
#ifdef _M_IX86
		aling_32,
#endif
		msg, wparam, lparam, proc);
}

int WINAPI h_DispatchClientMessage_A_win64(HWND* hwnd,
#ifdef _M_IX86
	DWORD aling_32,
#endif	
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {

	switch (msg) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}

	return  ((_DispatchClientMessagex64)o_DispatchClientMessage_A)(hwnd,
#ifdef _M_IX86
		aling_32,
#endif
		msg, wparam, lparam, proc);
}


int WINAPI h_DispatchClientMessage_W_win32(HWND* hwnd,DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {

	if (hwnd) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}

	return ((_DispatchClientMessagex32)o_DispatchClientMessage_W)(hwnd,msg, wparam, lparam, proc);
}

int WINAPI h_DispatchClientMessage_A_win32(HWND* hwnd,DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {

	switch (msg) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}

	return ((_DispatchClientMessagex32)o_DispatchClientMessage_A)(hwnd,msg, wparam, lparam, proc);
}