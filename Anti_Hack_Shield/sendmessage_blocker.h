#pragma once

#ifdef _M_IX86
	#define POINTER_TYPE DWORD
#else
	#define POINTER_TYPE DWORD64
#endif

typedef struct _fnClient {
	struct pfnelement {
		void * pFunc;
#ifdef _M_IX86
		DWORD aling_x32;
#endif
	}element[40];
}fnClient, *pfnClient;

#define _DispatchClientMessage_idx 21

typedef int (WINAPI * _RtlRetrieveNtUserPfn)(pfnClient* clientA, pfnClient* clientW, pfnClient* Unk);
typedef int (WINAPI * _DispatchClientMessage)(HWND *hwnd,
#ifdef _M_IX86
   DWORD aling_32,
#endif
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc);

_DispatchClientMessage o_DispatchClientMessage_A = 0;
_DispatchClientMessage o_DispatchClientMessage_W = 0;

pfnClient fnClientA, fnClientW, fnClientWorker;

POINTER_TYPE * vtbl_pDispatchA = 0;
POINTER_TYPE * vtbl_pDispatchW = 0;

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

int WINAPI h_DispatchClientMessage_W(HWND* hwnd,
#ifdef _M_IX86
	DWORD aling_32,
#endif	
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {

	if (hwnd) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}

	return o_DispatchClientMessage_W(hwnd,
#ifdef _M_IX86
		aling_32,
#endif
		msg, wparam, lparam, proc);
}

int WINAPI h_DispatchClientMessage_A(HWND* hwnd, 
#ifdef _M_IX86
	DWORD aling_32,
#endif	
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc) {



	switch (msg) {
		if (!validate_msg(msg)) {
			return 0;
		}
	}



	return  o_DispatchClientMessage_A(hwnd,
#ifdef _M_IX86
		aling_32,
#endif
		msg, wparam, lparam, proc);
}


bool c_anti_hack::init_fakemsg_profiler() {
	_RtlRetrieveNtUserPfn RtlRetrieveNtUserPfn = (_RtlRetrieveNtUserPfn)GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlRetrieveNtUserPfn");

	if (RtlRetrieveNtUserPfn) {
		RtlRetrieveNtUserPfn(&fnClientA, &fnClientW, &fnClientWorker);

		/*
		pfnClient->
		0x0 pFunc  ----->[ jmp [ntdll_vtbl[1]] ]->_user32_func1
		0x8 pFunc  ----->[ jmp [ntdll_vtbl[2]] ]->_user32_func2
		0x10 pFunc ----->[ jmp [ntdll_vtbl[3]] ]->_user32_func3
		0x18 pFunc ----->[ jmp [ntdll_vtbl[4]] ]->_user32_func4
		...
		...
		*/


#ifdef _M_IX86
		vtbl_pDispatchA = (POINTER_TYPE*)*(DWORD*)((DWORD)fnClientA->element[_DispatchClientMessage_idx].pFunc + 2);
		vtbl_pDispatchW = (POINTER_TYPE*)*(DWORD*)((DWORD)fnClientW->element[_DispatchClientMessage_idx].pFunc + 2);
#else
		vtbl_pDispatchA = (POINTER_TYPE*)(*(DWORD*)((DWORD64)fnClientA->element[_DispatchClientMessage_idx].pFunc + 2) + (POINTER_TYPE)fnClientA->element[_DispatchClientMessage_idx].pFunc + 6);
		vtbl_pDispatchW = (POINTER_TYPE*)(*(DWORD*)((DWORD64)fnClientW->element[_DispatchClientMessage_idx].pFunc + 2) + (POINTER_TYPE)fnClientW->element[_DispatchClientMessage_idx].pFunc + 6);
#endif

		o_DispatchClientMessage_A = (_DispatchClientMessage)*vtbl_pDispatchA;
		o_DispatchClientMessage_W = (_DispatchClientMessage)*vtbl_pDispatchW;


		DWORD lastProt;
		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		*vtbl_pDispatchA = (POINTER_TYPE)h_DispatchClientMessage_A;
		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, lastProt, &lastProt);

		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		*vtbl_pDispatchW = (POINTER_TYPE)h_DispatchClientMessage_W;
		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, lastProt, &lastProt);

		return true;
	}
	else {
		return false;
	}
}

void c_anti_hack::uninit_fakemsg_profiler() {
	DWORD lastProt;
	VirtualProtect((void*)vtbl_pDispatchA, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
	*vtbl_pDispatchA = (POINTER_TYPE)o_DispatchClientMessage_A;
	VirtualProtect((void*)vtbl_pDispatchA, 0xB8, lastProt, &lastProt);

	VirtualProtect((void*)vtbl_pDispatchW, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
	*vtbl_pDispatchW = (POINTER_TYPE)o_DispatchClientMessage_W;
	VirtualProtect((void*)vtbl_pDispatchW, 0xB8, lastProt, &lastProt);
}