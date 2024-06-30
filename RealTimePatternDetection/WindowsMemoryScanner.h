#pragma once
#include <memory>

#include "MemoryScanner.h"
#include <gtest/gtest_prod.h>
#include <windows.h>

class WindowsMemoryScannerTest;

class WindowsApiWrapper {
public:
	WindowsApiWrapper() = default;
	virtual ~WindowsApiWrapper() = default;
	virtual HANDLE OpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId);
	virtual BOOL ReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
	virtual SIZE_T VirtualQueryEx(HANDLE hProcess, LPCVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength);
	virtual void GetSystemInfo(LPSYSTEM_INFO lpSystemInfo);
	virtual BOOL CloseHandle(HANDLE hObject);
};

class WindowsMemoryScanner : public MemoryScanner {
public:
	friend class WindowsMemoryScannerTest;
	FRIEND_TEST(WindowsMemoryScannerTest, ScanMemoryFunctionTest);

	explicit WindowsMemoryScanner(const std::shared_ptr<WindowsApiWrapper>& In_windows_api_wrapper);
	~WindowsMemoryScanner() override;
	bool ScanMemory(const EventInfo& In_event_info, const std::wstring& In_target_string, const std::wstring& In_fail_path) override;
	void SetTargetString(const std::wstring& In_target) override;
	void SetFilePath(const std::wstring& In_file_path) override;

	std::wstring GetTargetString() override;
	std::wstring GetFilePath() override;
private:
	std::shared_ptr<WindowsApiWrapper> windows_api_wrapper_;

	std::wstring target_string_;
	std::wstring file_path_;
};

