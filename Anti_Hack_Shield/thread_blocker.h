#pragma once

PVOID c_anti_hack::GetCurrentThreadStartAddress() {
	NTSTATUS ntStatus;
	PVOID dwStartAddress;
		
	ntStatus = __NtQueryInformationThread(GetCurrentThread(), (THREADINFOCLASS)ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(dwStartAddress), NULL);

	if (ntStatus != STATUS_SUCCESS) return 0;
	return dwStartAddress;
}

bool  c_anti_hack::IsRemoteThread(DWORD ID) {
	ULONG retlen = 1;
	ULONG table_length = 0;
	SYSTEM_HANDLE_INFORMATION * HandleTable = 0;
	bool ret_status = false;

	while (retlen) {
	  NTSTATUS status =	__NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, HandleTable, table_length, &retlen);
	  table_length += retlen;

	  if (status == STATUS_SUCCESS) {
		  break;
	  }
	  
	  if (status == STATUS_INFO_LENGTH_MISMATCH) {
		  if (HandleTable) { delete[]HandleTable; }
		  HandleTable = (SYSTEM_HANDLE_INFORMATION *)new BYTE[table_length];
	  }

	}

	if (this->ObjectTypeNumberOfThread == 0xFF) { //Получаем id хандла потоков
		POBJECT_TYPE_INFORMATION objInfo = (POBJECT_TYPE_INFORMATION)new BYTE[0x1000];
		for (int i = 0; i < HandleTable->HandleCount; i++) {
			if (HandleTable->Handles[i].ProcessId == this->ProcessId) {
				if (__NtQueryObject((HANDLE)HandleTable->Handles[i].Handle, (OBJECT_INFORMATION_CLASS)ObjectTypeInformation, objInfo, 0x1000, NULL) == STATUS_SUCCESS) {
					if (!lstrcmpW(objInfo->Name.Buffer, L"Thread")) {
						this->ObjectTypeNumberOfThread = HandleTable->Handles[i].ObjectTypeNumber;
						this->ObjectTypeNumberOfProcess = this->ObjectTypeNumberOfThread - 1;
					}
				}
			}
		}
		delete[]objInfo;
	}

	if (this->ObjectTypeNumberOfThread != 0xFF) {
		for (int i = 0; i < HandleTable->HandleCount; i++) { //Перебор всех хандлов нашего процесса и получение хандла текущего потока
			if (HandleTable->Handles[i].ProcessId == this->ProcessId && HandleTable->Handles[i].ObjectTypeNumber == this->ObjectTypeNumberOfThread) {
				if (ID == GetThreadId((HANDLE)HandleTable->Handles[i].Handle)) {
					ret_status = true;
				}
			}
		}
	}
	else {
		ret_status = true;
	}

	delete[]HandleTable;

	return ret_status;
}

DWORD c_anti_hack::check_current_thread() {

	if (IsRemoteThread(GetCurrentThreadId())) {
		PVOID StartAddress = GetCurrentThreadStartAddress();
		if (StartAddress) {

#ifdef _M_IX86
			MEMORY_BASIC_INFORMATION32 MBI;
#else
			MEMORY_BASIC_INFORMATION64 MBI;
#endif
			DWORD mbi_size = sizeof(MBI);

			if (VirtualQuery(StartAddress, (MEMORY_BASIC_INFORMATION*)&MBI, mbi_size)) { //Получаем информацию о памяти с адресом старта

				if (MBI.Type&MEM_IMAGE) {  //если память выделена под образ

					for (unsigned int i = 0; i < this->suspected_threadbase.size(); i++) { //Проверка подозрительных адресов
						if (this->suspected_threadbase[i] == StartAddress) {
							return THREAD_REASON_SUSPECTED_ADDRESS;
						}
					}

					return 0;
				}
			}
		}
		return THREAD_REASON_UNBASE;
	}

	return THREAD_REASON_REMOTE;
}



void c_anti_hack::onStartThread() {
	DWORD thread_code = check_current_thread();
	  if(thread_code){
		  printf("thread [%x] was closed reason : ",GetCurrentThread());

		  switch (thread_code) {
			  case THREAD_REASON_REMOTE: {
				  printf("[Remoted thread]\n");
				  break;
			  }
			  case THREAD_REASON_UNBASE: {
				  printf("[Base address not IMAGE]\n");
				  break;
			  }
			  case THREAD_REASON_SUSPECTED_ADDRESS: {
				  printf("[Suspected base address]\n");
				  break;
			  }
		  }

		TerminateThread(GetCurrentThread(), 1);
	  }
}


void c_anti_hack::onEndThread() {


}


