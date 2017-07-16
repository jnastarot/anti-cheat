#pragma once


LRESULT CALLBACK winhook_profiler(int code, WPARAM wParam, LPARAM lParam) {

	MSG * message = (MSG*)lParam;

	switch (message->message) {
		case WM_LBUTTONDOWN: {
			anti_cheat->MouseButtonsArray[0] = true;
			break;
		}
		case WM_LBUTTONUP: {
			anti_cheat->MouseButtonsArray[0] = false;
			break;
		}
		case WM_MBUTTONDOWN: {
			anti_cheat->MouseButtonsArray[1] = true;
			break;
		}
		case WM_MBUTTONUP: {
			anti_cheat->MouseButtonsArray[1] = false;
			break;
		}
		case WM_RBUTTONDOWN: {
			anti_cheat->MouseButtonsArray[2] = true;
			break;
		}
		case WM_RBUTTONUP: {
			anti_cheat->MouseButtonsArray[2] = false;
			break;
		}

		case WM_KEYDOWN: {
			anti_cheat->KeyBoardArray[message->wParam].Key = true;
			break;
		}

		case WM_KEYUP: {
			anti_cheat->KeyBoardArray[message->wParam].Key = false;
			break;
		}
       

		default:break;
	}


	return CallNextHookEx(0, code, wParam, lParam);
}
