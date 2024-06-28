#include "WindowsMemoryScanner.h"
#include <algorithm>
#include <windows.h>
#include <tlhelp32.h>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "EventInfoDao.h"

WindowsMemoryScanner::WindowsMemoryScanner() = default;

WindowsMemoryScanner::~WindowsMemoryScanner() = default;

bool WindowsMemoryScanner::ScanMemory(const EventInfo& In_event_info) {
	if (this->target_string_.empty()) {
		throw std::runtime_error("Target string empty.");
	}

	auto file_path = In_event_info.GetFilePath();
	if (!file_path.empty() &&
		std::search(file_path.begin(), file_path.end(),
					this->file_path_.begin(), this->file_path_.end()) == file_path.end()) {
		return false;
	}

	const HANDLE process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, static_cast<DWORD>(In_event_info.GetPid()));
	if (process == nullptr) {
		std::cerr << "WindowsMemoryScanner::ScanMemory : PID:" << In_event_info.GetPid() << " OpenProcess failed with error: " << GetLastError() << '\n';
		return false;
	}

	SYSTEM_INFO system_info;
	MEMORY_BASIC_INFORMATION memory_basic_info = { nullptr };

	ZeroMemory(&system_info, sizeof(SYSTEM_INFO));
	GetSystemInfo(&system_info);

	auto address = static_cast<PBYTE>(system_info.lpMinimumApplicationAddress);

	while (address < static_cast<PBYTE>(system_info.lpMaximumApplicationAddress)) {
		ZeroMemory(&memory_basic_info, sizeof(MEMORY_BASIC_INFORMATION));

		if (!VirtualQueryEx(process, address, &memory_basic_info, sizeof(MEMORY_BASIC_INFORMATION))) {
			std::cerr << "WindowsMemoryScanner::ScanMemory : PID:" << In_event_info.GetPid() << " VirtualQueryEx failed with error: " << GetLastError() << '\n';
			break;
		}

		if (memory_basic_info.State == MEM_COMMIT && (memory_basic_info.Protect == PAGE_READWRITE || memory_basic_info.Protect == PAGE_READONLY)) {
			auto buffer = std::make_unique<wchar_t[]>(memory_basic_info.RegionSize);
			SIZE_T bytes_read;
			if (ReadProcessMemory(process, address, buffer.get(), memory_basic_info.RegionSize, &bytes_read)) {
				if (std::search(buffer.get(), buffer.get() + bytes_read, this->target_string_.begin(), this->target_string_.end()) != buffer.get() + bytes_read) {
					CloseHandle(process);
					return true;
				}
			} else {
				std::cerr << "WindowsMemoryScanner::ScanMemory : PID:" << In_event_info.GetPid() << " ReadProcessMemory failed with error: " << GetLastError() << '\n';
			}
		}

		address += memory_basic_info.RegionSize;
	}

	CloseHandle(process);
	return false;
}

void WindowsMemoryScanner::SaveEventInfo(const EventInfo& In_event_info) {
	EventInfoDao::GetInstance().AddIntoQueue(In_event_info);
}

void WindowsMemoryScanner::SetTargetString(const std::wstring& In_target) {
	this->target_string_ = In_target;
}

void WindowsMemoryScanner::SetFilePath(const std::wstring& In_file_path) {
	this->file_path_ = In_file_path;
}
