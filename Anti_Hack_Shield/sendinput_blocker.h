#pragma once

LRESULT(CALLBACK *ProcOriginal)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


//Наш обработчик сообщений окна
LRESULT CALLBACK ProcFilter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	if (message == WM_INPUT) {
		UINT dwSize = sizeof(RAWINPUT);
		BYTE *lpb = NULL;
		RAWINPUT raw_i;

		if (GET_RAWINPUT_CODE_WPARAM(wParam) == RIM_INPUT) {//Нажатие в момент того как окно было в фокусе

			//Получаем размер сообщения
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
			lpb = new BYTE[dwSize];

			//Получаем сообщение от девайса
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
				return 0;

			//hDevice == 0 если устройства нет,что значит что это эмуляция нажатия
			if (((RAWINPUT*)(lpb))->header.hDevice) {
				delete[] lpb;
			}
			else {
				delete[] lpb;
				printf("Device Emulation [SendInput]\n");
				return 0;
			}
		}
	}
	

	return ProcOriginal(hWnd, message, wParam, lParam);//Вызываем оригинальный обработчик окна
}


bool c_anti_hack::protectHWND(HWND def_hwnd) {
	if (!wnd_protected && IsWindow(def_hwnd)) {
		wnd_protected = def_hwnd;

		//Подмена оригинального обработчика сообщений на наш
		if (ProcOriginal = (LRESULT(CALLBACK *)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam))SetWindowLongPtrA(def_hwnd, GWLP_WNDPROC, (LONG_PTR)ProcFilter)) {

			RAWINPUTDEVICE rid;
			rid.usUsagePage = 0x01;
			rid.usUsage = 0x06;
			rid.dwFlags = RIDEV_INPUTSINK;
			rid.hwndTarget = def_hwnd;

			//Регистрация для сообщения WM_INPUT
			if (RegisterRawInputDevices(&rid, 1, sizeof(RAWINPUTDEVICE))) {
				return true;
			}
			else {
				SetWindowLongA(def_hwnd, GWLP_WNDPROC, (LONG)ProcOriginal);
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
		SetWindowLongPtrA(wnd_protected, GWLP_WNDPROC, (LONG_PTR)ProcOriginal);
	}
}