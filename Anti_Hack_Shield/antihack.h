#pragma once


#include "AH_internal.h"

class c_anti_hack {
	DWORD init_state = 0;
	bool IsDebugger = false;
	DWORD ProcessId = GetCurrentProcessId();

	BYTE ObjectTypeNumberOfThread  = 0xFF;
//	BYTE ObjectTypeNumberOfProcess = 0xFF;

	LPVOID dll_notify_cookie = 0;

	HWND wnd_protected = 0;
  
	_NtQueryInformationThread __NtQueryInformationThread = 0;
	_NtQuerySystemInformation __NtQuerySystemInformation = 0;
	_NtQueryObject            __NtQueryObject =            0;


	PVOID c_anti_hack::GetCurrentThreadStartAddress();
	bool  c_anti_hack::IsRemoteThread(DWORD ID);
	DWORD  c_anti_hack::check_current_thread();

	bool c_anti_hack::init_dll_profiler();
	bool c_anti_hack::init_fakemsg_profiler();

	void c_anti_hack::uninit_dll_profiler();
	void c_anti_hack::uninit_fakemsg_profiler();

	void c_anti_hack::unprotect_all_HWND();

	std::vector<LPVOID> suspected_threadbase;
	 
	void c_anti_hack::set_privileges();

	void c_anti_hack::new_event();
public:
	void c_anti_hack::on_load_dll(HMODULE hmod, std::wstring path);
	bool c_anti_hack::check_dll_on_legit(std::wstring path);

	void c_anti_hack::scan_process();

	bool c_anti_hack::protectHWND(HWND wnd);
	void c_anti_hack::unprotectHWND();

	void c_anti_hack::onStartThread();
	void c_anti_hack::onEndThread();

	c_anti_hack::c_anti_hack();
	c_anti_hack::~c_anti_hack();
};
