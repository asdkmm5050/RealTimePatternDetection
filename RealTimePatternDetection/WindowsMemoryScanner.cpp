#include "WindowsMemoryScanner.h"
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

#include "EventInfoJsonGenerator.h"

WindowsMemoryScanner::WindowsMemoryScanner(const std::shared_ptr<WindowsApiWrapper>& In_windows_api_wrapper) :
	windows_api_wrapper_(In_windows_api_wrapper) {
	EventInfoJsonGenerator::GetInstance().SetSaveInterval(30);
	EventInfoJsonGenerator::GetInstance().StartSave();
}

WindowsMemoryScanner::~WindowsMemoryScanner() {
	EventInfoJsonGenerator::GetInstance().StopSave();
}

bool WindowsMemoryScanner::ScanMemory(const EventInfo& In_event_info,
									  const std::wstring& In_target_string,
									  const std::wstring& In_fail_path) {
	HANDLE process = nullptr;

	try {
		if (In_target_string.empty()) {
			throw std::runtime_error("Target string empty.");
		}

		auto file_path = In_event_info.GetFilePath();
		if (!file_path.empty() && std::search(file_path.begin(), file_path.end(),
											  In_fail_path.begin(), In_fail_path.end()) == file_path.end()) {
			return false;
		}

		process = this->windows_api_wrapper_->OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
														  FALSE,
														  static_cast<DWORD>(In_event_info.GetPid()));
		if (!process) {
			throw std::runtime_error("OpenProcess failed with error: " + std::to_string(GetLastError()));
		}

		SYSTEM_INFO system_info;
		MEMORY_BASIC_INFORMATION memory_basic_info = { nullptr };
		ZeroMemory(&system_info, sizeof(SYSTEM_INFO));
		this->windows_api_wrapper_->GetSystemInfo(&system_info);

		auto address = static_cast<PBYTE>(system_info.lpMinimumApplicationAddress);
		while (address < static_cast<PBYTE>(system_info.lpMaximumApplicationAddress)) {
			ZeroMemory(&memory_basic_info, sizeof(MEMORY_BASIC_INFORMATION));

			if (!this->windows_api_wrapper_->VirtualQueryEx(process, address, &memory_basic_info, sizeof(MEMORY_BASIC_INFORMATION))) {
				throw std::runtime_error("VirtualQueryEx failed with error: " + std::to_string(GetLastError()));
			}

			if (memory_basic_info.State == MEM_COMMIT && (memory_basic_info.Protect == PAGE_READWRITE || memory_basic_info.Protect == PAGE_READONLY)) {
				auto buffer = std::make_unique<wchar_t[]>(memory_basic_info.RegionSize);
				SIZE_T bytes_read;
				if (this->windows_api_wrapper_->ReadProcessMemory(process, address, buffer.get(), memory_basic_info.RegionSize, &bytes_read)) {
					if (std::search(buffer.get(), buffer.get() + bytes_read, In_target_string.begin(), In_target_string.end()) != buffer.get() + bytes_read) {
						this->windows_api_wrapper_->CloseHandle(process);
						return true;
					}
				} else {
					throw std::runtime_error("ReadProcessMemory failed with error: " + std::to_string(GetLastError()));
				}
			}

			address += memory_basic_info.RegionSize;
		}

		this->windows_api_wrapper_->CloseHandle(process);
		return false;

	} catch (const std::exception& e) {
		if (process) {
			this->windows_api_wrapper_->CloseHandle(process);
		}
		std::cerr << "WindowsMemoryScanner::ScanMemory : PID:" << In_event_info.GetPid() << " " << e.what() << '\n';
		return false;
	}
}


void WindowsMemoryScanner::SaveEventInfo(const EventInfo& In_event_info) {
	EventInfoJsonGenerator::GetInstance().AddIntoQueue(In_event_info);
}

void WindowsMemoryScanner::SetTargetString(const std::wstring& In_target) {
	this->target_string_ = In_target;
}

void WindowsMemoryScanner::SetFilePath(const std::wstring& In_file_path) {
	this->file_path_ = In_file_path;
}

std::wstring WindowsMemoryScanner::GetTargetString() {
	return this->target_string_;
}

std::wstring WindowsMemoryScanner::GetFilePath() {
	return this->file_path_;
}

HANDLE WindowsApiWrapper::OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId) {
	return ::OpenProcess(dwDesiredAccess, bInheritHandle, dwProcessId);
}

BOOL WindowsApiWrapper::ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead) {
	return ::ReadProcessMemory(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesRead);
}

SIZE_T WindowsApiWrapper::VirtualQueryEx(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer,
										 SIZE_T dwLength) {
	return ::VirtualQueryEx(hProcess, lpAddress, lpBuffer, dwLength);
}

void WindowsApiWrapper::GetSystemInfo(LPSYSTEM_INFO lpSystemInfo) {
	::GetSystemInfo(lpSystemInfo);
}

BOOL WindowsApiWrapper::CloseHandle(HANDLE hObject) {
	return ::CloseHandle(hObject);
}