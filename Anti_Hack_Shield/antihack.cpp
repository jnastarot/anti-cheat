#include "stdafx.h"
#include "antihack.h"


c_anti_hack *anti_cheat =  new c_anti_hack();


#include "dllload_blocker.h"
#include "thread_blocker.h"

#include "message_filterproc.h"

#include "sendinput_blocker.h"
#include "sendmessage_blocker.h"

#include "process_scanner.h"

c_anti_hack::c_anti_hack() {
	//this->set_privileges();

	ZeroMemory(KeyBoardArray, sizeof(KeyBoardArray));
	ZeroMemory(MouseButtonsArray, sizeof(MouseButtonsArray));

#ifdef _M_IX86 
	IsWow64Process(GetCurrentProcess(), &this->IsWow64);
#else
	this->IsWow64 = true;
#endif

#ifdef _M_IX86 
	PPEB pPeb =  (PPEB)(__readfsdword(0x30));
#else
	PPEB pPeb = (PPEB)(__readgsqword(0x60));
#endif

	if (pPeb->BeingDebugged) {
		//TerminateThread(GetCurrentThread(), 0);
	}
	

	 __NtQueryInformationThread = (_NtQueryInformationThread)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryInformationThread");
	 __NtQuerySystemInformation = (_NtQuerySystemInformation)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");
	            __NtQueryObject = (_NtQueryObject)GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQueryObject");

	this->init_dll_profiler();
	this->init_fakemsg_profiler();

	HMODULE kernel32   = GetModuleHandle("kernel32.dll");
	HMODULE kernelbase = GetModuleHandle("kernelbase.dll");

	this->suspected_threadbase.push_back(GetProcAddress(kernel32,	"LoadLibraryA"));
	this->suspected_threadbase.push_back(GetProcAddress(kernel32,	"LoadLibraryW"));
	this->suspected_threadbase.push_back(GetProcAddress(kernel32,	"ExitProcess"));
	this->suspected_threadbase.push_back(GetProcAddress(kernel32,	"FreeLibrary"));

	if (kernelbase) {
		this->suspected_threadbase.push_back(GetProcAddress(kernelbase, "LoadLibraryA"));
		this->suspected_threadbase.push_back(GetProcAddress(kernelbase, "LoadLibraryW"));
		this->suspected_threadbase.push_back(GetProcAddress(kernelbase, "ExitProcess"));
		this->suspected_threadbase.push_back(GetProcAddress(kernelbase, "FreeLibrary"));
	}
}


c_anti_hack::~c_anti_hack() {
	if (this->init_state&ANTIHACK_DLL) { 
		this->uninit_dll_profiler();     
	}

	if (this->init_state&ANTIHACK_MSG || this->init_state&ANTIHACK_MSG_1) {
		this->uninit_fakemsg_profiler(); 
	}
}



#define SE_MIN_WELL_KNOWN_PRIVILEGE       (2L)
#define SE_MAX_WELL_KNOWN_PRIVILEGE       (30L)

typedef NTSTATUS(WINAPI* _RtlAdjustPrivilege)(ULONG    Privilege, BOOLEAN  Enable, BOOLEAN  CurrentThread, PBOOLEAN Enabled);

void c_anti_hack::set_privileges() {
	_RtlAdjustPrivilege __RtlAdjustPrivilege = (_RtlAdjustPrivilege)GetProcAddress(GetModuleHandle("ntdll.dll"), "RtlAdjustPrivilege");

	BOOLEAN bWasEnabled;
	for (ULONG priv = SE_MIN_WELL_KNOWN_PRIVILEGE; priv <= SE_MAX_WELL_KNOWN_PRIVILEGE; priv++) {
		__RtlAdjustPrivilege(priv, TRUE, FALSE, &bWasEnabled);
	}
	
}

void NTAPI anticheat_thread_profiler(void*  dll, DWORD reason, void* reserved) {

	switch (reason) {
		case DLL_THREAD_ATTACH: {
			if (anti_cheat) {
				anti_cheat->onStartThread(); 
			}
			break;
		}
		case DLL_THREAD_DETACH: {
			if (anti_cheat) {
				anti_cheat->onEndThread();
			}
			break;
		}

		case DLL_PROCESS_ATTACH: {
			if (!anti_cheat) {
				// ( new ) on release x86 dont works here \_(0_0)_/
				//anti_cheat = new c_anti_hack();
			}
			break;
		}
		case DLL_PROCESS_DETACH: {
			if (anti_cheat) {
				anti_cheat->~c_anti_hack();
			}
			break;
		}
	}
}


#ifdef _WIN64
     #pragma comment (linker, "/INCLUDE:_tls_used")  
     #pragma comment (linker, "/INCLUDE:tls_callback_func") 
#else
     #pragma comment (linker, "/INCLUDE:__tls_used")
     #pragma comment (linker, "/INCLUDE:_tls_callback_func")
#endif

#ifdef _WIN64
    #pragma const_seg(".CRT$XLF")
    EXTERN_C const
#else
    #pragma data_seg(".CRT$XLF")
    EXTERN_C
#endif
PIMAGE_TLS_CALLBACK tls_callback_func = anticheat_thread_profiler;
#ifdef _WIN64
    #pragma const_seg()
#else
    #pragma data_seg()
#endif 