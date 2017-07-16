#pragma once

//Наш обработчик сообщений окна
LRESULT CALLBACK ProcFilter(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	switch (message) {
			//Mouse
		case WM_LBUTTONDOWN: { 
			if (!anti_cheat->MouseButtonsArray[0]) {
				printf("Device Emulation [SendMessage]\n");
				return 0;
			}
			break;
		}
		case WM_MBUTTONDOWN: {
			if (!anti_cheat->MouseButtonsArray[1]) {
				printf("Device Emulation [SendMessage]\n");
				return 0;
			}
			break;
		}
		case WM_RBUTTONDOWN: {
			if (!anti_cheat->MouseButtonsArray[2]) {			
				printf("Device Emulation [SendMessage]\n");
				return 0;
			}
			break;
		}

            //KeyBoard
		case WM_KEYDOWN: { 
			if (!anti_cheat->KeyBoardArray[wParam].Key || anti_cheat->KeyBoardArray[wParam].Block_WM_KEY_DOWN_counter) {
				if (anti_cheat->KeyBoardArray[wParam].Block_WM_KEY_DOWN_counter) {
					anti_cheat->KeyBoardArray[wParam].Block_WM_KEY_DOWN_counter--;
				}
				printf("Device Emulation [SendMessage]\n");
				return 0;
			}
			break;
		}
		case WM_KEYUP: { 
			if (anti_cheat->KeyBoardArray[wParam].Block_WM_KEY_UP_counter) {
				anti_cheat->KeyBoardArray[wParam].Block_WM_KEY_UP_counter--;
				printf("Device Emulation [SendMessage]\n");
				return 0;
			}
			break;
		}


		case WM_INPUT: {
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
					if (((RAWINPUT*)(lpb))->data.keyboard.Message == WM_KEYDOWN) {
						anti_cheat->KeyBoardArray[((RAWINPUT*)(lpb))->data.keyboard.VKey].Block_WM_KEY_DOWN_counter++;
					}
					if (((RAWINPUT*)(lpb))->data.keyboard.Message == WM_KEYUP) {
						anti_cheat->KeyBoardArray[((RAWINPUT*)(lpb))->data.keyboard.VKey].Block_WM_KEY_UP_counter++;
					}
					delete[] lpb;
					printf("Device Emulation [SendInput]\n");
					return 0;
				}
			}
			break;
		}

		default: break;
	}

	
	WNDPROC OriginalProc = anti_cheat->getMsgProc();
	if (anti_cheat->getMsgProc()) {
		OriginalProc(hWnd, message, wParam, lParam);//Вызываем оригинальный обработчик окна
	}
	else {
		return 0;
	}
}
