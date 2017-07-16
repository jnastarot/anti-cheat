#pragma once

bool c_anti_hack::protectHWND(HWND def_hwnd) {
	if (!wnd_protected && IsWindow(def_hwnd)) {
		wnd_protected = def_hwnd;

		printf("SetWindowLongPtrA\n");

		//Подмена оригинального обработчика сообщений на наш
		if (this->ProcOriginal = (WNDPROC)SetWindowLongPtrA(def_hwnd, GWLP_WNDPROC, (LONG_PTR)ProcFilter)) {

			RAWINPUTDEVICE rid;
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x06;
			rid.dwFlags = RIDEV_INPUTSINK;
			rid.hwndTarget = def_hwnd;
			printf("RegisterRawInputDevices\n");
			//Регистрация для сообщения WM_INPUT
			if (RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
				return true;
			}
			else {
				SetWindowLongPtrA(def_hwnd, GWLP_WNDPROC, (LONG_PTR)this->ProcOriginal);
				return false;
			}
		}
		else {
			return false;
		}
	}
	return false;
}

void c_anti_hack::unprotectHWND() {
	if (wnd_protected){
		SetWindowLongPtrA(wnd_protected, GWLP_WNDPROC, (LONG_PTR)this->ProcOriginal);
	}
}