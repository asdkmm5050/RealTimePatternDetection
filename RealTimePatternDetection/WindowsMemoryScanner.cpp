#include "WindowsMemoryScanner.h"
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>

#include "EventInfoJsonGenerator.h"

WindowsMemoryScanner::WindowsMemoryScanner(const std::shared_ptr<WindowsApiWrapper>& In_windows_api_wrapper) :
	windows_api_wrapper_(In_windows_api_wrapper),
	shellcode_count_(0) {}

WindowsMemoryScanner::~WindowsMemoryScanner() = default;

bool WindowsMemoryScanner::ScanMemory(const EventInfo& In_event_info,
									  const std::wstring& In_target_string,
									  const std::wstring& In_fail_path) {
	HANDLE process = nullptr;

	bool return_result(false);

	try {
		process = this->windows_api_wrapper_->OpenProcess(PROCESS_ALL_ACCESS,
														  FALSE,
														  static_cast<DWORD>(In_event_info.GetPid()));
		if (!process) {
			throw std::runtime_error("OpenProcess failed with error: " + std::to_string(GetLastError()));
		}

		if (In_target_string.empty()) {
			throw std::runtime_error("Target string empty.");
		}

		auto file_path = In_event_info.GetFilePath();
		if (!file_path.empty() && std::search(file_path.begin(), file_path.end(),
											  In_fail_path.begin(), In_fail_path.end()) == file_path.end()) {
			return false;
		}

		SYSTEM_INFO sys_info;
		this->windows_api_wrapper_->GetSystemInfo(&sys_info);
		MEMORY_BASIC_INFORMATION mem_info;
		LPVOID adder = sys_info.lpMinimumApplicationAddress;

		while (this->windows_api_wrapper_->VirtualQueryEx(process, adder, &mem_info, sizeof(mem_info))) {
			if (mem_info.State == MEM_COMMIT &&
				!(mem_info.Protect & PAGE_NOACCESS) &&
				!(mem_info.Protect & PAGE_GUARD)) {
				std::vector<char> buffer(mem_info.RegionSize);
				SIZE_T bytes_read;

				if (this->windows_api_wrapper_->ReadProcessMemory(process, mem_info.BaseAddress, buffer.data(), mem_info.RegionSize, &bytes_read)) {
					const auto result = strstr(buffer.data(), reinterpret_cast<const char*>(In_target_string.c_str()));
					if (result) {
						std::printf("WindowsMemoryScanner::ScanMemory : PID:%d Found shellcode at address: %p Number:%d\n",
									In_event_info.GetPid(), mem_info.BaseAddress, this->shellcode_count_++);
						return_result = true;
						break;
					}
				}
			}
			adder = static_cast<LPBYTE>(mem_info.BaseAddress) + mem_info.RegionSize;
		}

		if (!return_result) {
			std::printf("WindowsMemoryScanner::ScanMemory : PID:%d No shellcode\n", In_event_info.GetPid());
		}

	} catch (const std::exception& e) {
		std::cerr << "WindowsMemoryScanner::ScanMemory : PID:" << In_event_info.GetPid() << " " << e.what() << '\n';
	}

	if (process) {
		this->windows_api_wrapper_->CloseHandle(process);
	}
	return return_result;
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