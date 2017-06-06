#pragma once

#include "stdafx.h"


extern c_anti_hack* anti_cheat;



VOID NTAPI anti_cheat_timer(PVOID, BOOLEAN) {
	MessageBox(0, 0, 0, 0);
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                  _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
	if (AllocConsole()) { freopen("CONOUT$", "w", stdout); setlocale(LC_ALL, "RUS"); }

	window* main_window = new window("Anti-Hack", "Anti-Hack_Class");

	if (main_window->getSelfHandle()) {
		anti_cheat->protectHWND(main_window->getSelfHandle());
		main_window->onMessage();
		anti_cheat->unprotectHWND();
	}

	delete main_window;

	return 0;
}

