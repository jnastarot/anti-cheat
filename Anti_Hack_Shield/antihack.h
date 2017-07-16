#pragma once
#pragma pack(push, 1)

#include "AH_internal.h"

class c_anti_hack {
	DWORD init_state = 0;
	bool IsDebugger = false;
	BOOL IsWow64;
	DWORD ProcessId = GetCurrentProcessId();

	HHOOK h_winhook = 0;

	BYTE ObjectTypeNumberOfThread  = 0xFF;
	BYTE ObjectTypeNumberOfProcess = 0xFF;

	WNDPROC ProcOriginal = 0;

	LPVOID dll_notify_cookie = 0;

	HWND wnd_protected = 0;
  
	_NtQueryInformationThread __NtQueryInformationThread = 0;
	_NtQuerySystemInformation __NtQuerySystemInformation = 0;
	_NtQueryObject            __NtQueryObject =            0;

	std::vector<LPVOID> suspected_threadbase;

	PVOID c_anti_hack::GetCurrentThreadStartAddress();
	bool  c_anti_hack::IsRemoteThread(DWORD ID);
	DWORD  c_anti_hack::check_current_thread();

	bool c_anti_hack::init_dll_profiler();
	bool c_anti_hack::init_fakemsg_profiler();
	bool c_anti_hack::init_fakemsg_oldprofiler();

	void c_anti_hack::uninit_dll_profiler();
	void c_anti_hack::uninit_fakemsg_profiler();
	void c_anti_hack::uninit_fakemsg_oldprofiler();
	 
	void c_anti_hack::set_privileges();
public:
	bool		MouseButtonsArray[3];
	__keyallow  KeyBoardArray[256];

	void c_anti_hack::on_load_dll(HMODULE hmod, std::wstring path);
	bool c_anti_hack::check_dll_on_legit(std::wstring path);

	void c_anti_hack::scan_process();

	bool c_anti_hack::protectHWND(HWND wnd);
	void c_anti_hack::unprotectHWND();

	void c_anti_hack::onStartThread();
	void c_anti_hack::onEndThread();

	DWORD   c_anti_hack::getState()   { return this->init_state;   }
	WNDPROC c_anti_hack::getMsgProc() { return this->ProcOriginal; }

	c_anti_hack::c_anti_hack();
	c_anti_hack::~c_anti_hack();
};
#pragma pack(pop)


/*
 new anti-sendmessage works only on windows 10 x64 
 old anti-sendmessage works everywhere
 sendinput			  works everywhere
 thread detect        tested on windows 7 x86 and windows 10 x64
 load dll			  works only on windows vista and later versions
*/