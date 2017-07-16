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

typedef struct _fnClient32 {
	struct pfnelement {
		void * pFunc;
	}element[40];
}fnClient32, *pfnClient32;


#define _DispatchClientMessage_idx 21

typedef int (WINAPI * _RtlRetrieveNtUserPfn)(void** clientA, void** clientW, void** Unk);

typedef int (WINAPI * _DispatchClientMessagex64)(HWND *hwnd,
#ifdef _M_IX86 
	DWORD aling_32, 
#endif
	DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc);

typedef int (WINAPI * _DispatchClientMessagex32)(HWND *hwnd,DWORD msg, WPARAM wparam, LPARAM lparam, WNDPROC proc);


LPVOID o_DispatchClientMessage_A = 0;
LPVOID o_DispatchClientMessage_W = 0;

void * fnClientA,* fnClientW,* fnClientWorker;

POINTER_TYPE * vtbl_pDispatchA = 0;
POINTER_TYPE * vtbl_pDispatchW = 0;


#include "sendmsg_dispatcher.h"
#include "sendmsg_dispatcher_old.h"



bool c_anti_hack::init_fakemsg_profiler() {
	_RtlRetrieveNtUserPfn RtlRetrieveNtUserPfn = (_RtlRetrieveNtUserPfn)GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlRetrieveNtUserPfn");

	if (RtlRetrieveNtUserPfn && RtlRetrieveNtUserPfn(&fnClientA, &fnClientW, &fnClientWorker) == STATUS_SUCCESS) {
		//tested on windows 10 x64

		/*
		pfnClient->
		0x0 pFunc  ----->[ jmp [ntdll_vtbl[1]] ]->_user32_func1
		0x8 pFunc  ----->[ jmp [ntdll_vtbl[2]] ]->_user32_func2
		0x10 pFunc ----->[ jmp [ntdll_vtbl[3]] ]->_user32_func3
		0x18 pFunc ----->[ jmp [ntdll_vtbl[4]] ]->_user32_func4
		...
		...
		*/

		if (this->IsWow64) {
#ifdef _M_IX86
			vtbl_pDispatchA = (POINTER_TYPE*)*(DWORD*)((DWORD)((pfnClient)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 2);
			vtbl_pDispatchW = (POINTER_TYPE*)*(DWORD*)((DWORD)((pfnClient)fnClientW)->element[_DispatchClientMessage_idx].pFunc + 2);
#else
			vtbl_pDispatchA = (POINTER_TYPE*)(*(DWORD*)((DWORD64)((pfnClient)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 2) + (POINTER_TYPE)((pfnClient)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 6);
			vtbl_pDispatchW = (POINTER_TYPE*)(*(DWORD*)((DWORD64)((pfnClient)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 2) + (POINTER_TYPE)((pfnClient)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 6);
#endif
		}
		else {
			vtbl_pDispatchA = (POINTER_TYPE*)*(DWORD*)((DWORD)((pfnClient32)fnClientA)->element[_DispatchClientMessage_idx].pFunc + 2);
			vtbl_pDispatchW = (POINTER_TYPE*)*(DWORD*)((DWORD)((pfnClient32)fnClientW)->element[_DispatchClientMessage_idx].pFunc + 2);
		}

		

		o_DispatchClientMessage_A = (_DispatchClientMessagex64)*vtbl_pDispatchA;
		o_DispatchClientMessage_W = (_DispatchClientMessagex64)*vtbl_pDispatchW;

		DWORD lastProt;
		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		if (this->IsWow64) {
			*vtbl_pDispatchA = (POINTER_TYPE)h_DispatchClientMessage_A_win64;
		}
		else {
			*vtbl_pDispatchA = (POINTER_TYPE)h_DispatchClientMessage_A_win32;
		}

		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, lastProt, &lastProt);


		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		if (this->IsWow64) {
			*vtbl_pDispatchW = (POINTER_TYPE)h_DispatchClientMessage_W_win64;
		}
		else {
			*vtbl_pDispatchW = (POINTER_TYPE)h_DispatchClientMessage_W_win32;
		}
		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, lastProt, &lastProt);


		this->init_state |= ANTIHACK_MSG;
		return true;
	}

	return init_fakemsg_oldprofiler();
}

void c_anti_hack::uninit_fakemsg_profiler() {
	if (this->init_state&ANTIHACK_MSG_1) {
		uninit_fakemsg_oldprofiler();
	}

	if(this->init_state&ANTIHACK_MSG){
		DWORD lastProt;
		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		*vtbl_pDispatchA = (POINTER_TYPE)o_DispatchClientMessage_A;
		VirtualProtect((void*)vtbl_pDispatchA, 0xB8, lastProt, &lastProt);

		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, PAGE_EXECUTE_READWRITE, &lastProt);
		*vtbl_pDispatchW = (POINTER_TYPE)o_DispatchClientMessage_W;
		VirtualProtect((void*)vtbl_pDispatchW, 0xB8, lastProt, &lastProt);
	}
}


bool c_anti_hack::init_fakemsg_oldprofiler() {
	SetWindowsHookA(WH_GETMESSAGE, winhook_profiler);
	this->init_state |= ANTIHACK_MSG_1;
	return true;
}


void c_anti_hack::uninit_fakemsg_oldprofiler() {
	UnhookWindowsHook(WH_GETMESSAGE, winhook_profiler);
}