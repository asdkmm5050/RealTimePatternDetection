#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../RealTimePatternDetection/WindowsMemoryScanner.h"
#include "../RealTimePatternDetection/WindowsMemoryScanner.cpp"

class MockWindowsApiWrapper : public WindowsApiWrapper {
public:
	MockWindowsApiWrapper() :
		buffer_(100, L'a') {

		memory_basic_info_.State = MEM_COMMIT;
		memory_basic_info_.Protect = PAGE_READWRITE;
		memory_basic_info_.RegionSize = 1000;
		memory_basic_info_.BaseAddress = static_cast<PVOID>(nullptr);

		system_info_.lpMinimumApplicationAddress = reinterpret_cast<void*>(0);
		system_info_.lpMaximumApplicationAddress = reinterpret_cast<void*>(1000);
		ON_CALL(*this, OpenProcess(testing::_, testing::_, testing::_))
			.WillByDefault(testing::Invoke([](DWORD, BOOL, DWORD) -> void* {
			return reinterpret_cast<void*>(1);
		}));
		ON_CALL(*this, VirtualQueryEx(testing::_, testing::_, testing::_, testing::_))
			.WillByDefault(testing::Invoke([&](HANDLE, LPCVOID In_adder, const PMEMORY_BASIC_INFORMATION In_basic_info, SIZE_T) -> SIZE_T {
			*In_basic_info = this->memory_basic_info_;

			if (In_adder != memory_basic_info_.BaseAddress) {
				return 0;
			}
			return 1;
		}));

		ON_CALL(*this, ReadProcessMemory(testing::_, testing::_, testing::_, testing::_, testing::_))
			.WillByDefault(testing::Invoke([&](HANDLE, LPCVOID, const LPVOID In_buffer, SIZE_T, SIZE_T* Out_size) {
			memcpy(In_buffer, this->buffer_.data(), this->buffer_.size());
			*Out_size = this->buffer_.size();
			return true;
		}));

		ON_CALL(*this, GetSystemInfo(testing::_))
			.WillByDefault(testing::Invoke([&](LPSYSTEM_INFO In_system_info) {
			*In_system_info = this->system_info_;
		}));

		ON_CALL(*this, CloseHandle(testing::_))
			.WillByDefault(testing::Return(TRUE));
	}

	~MockWindowsApiWrapper() override = default;

	MOCK_METHOD(HANDLE, OpenProcess, (DWORD, BOOL, DWORD), (override));
	MOCK_METHOD(BOOL, ReadProcessMemory, (HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T*), (override));
	MOCK_METHOD(SIZE_T, VirtualQueryEx, (HANDLE, LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T), (override));
	MOCK_METHOD(void, GetSystemInfo, (LPSYSTEM_INFO), (override));
	MOCK_METHOD(BOOL, CloseHandle, (HANDLE), (override));
private:
	std::wstring buffer_;
	MEMORY_BASIC_INFORMATION memory_basic_info_;
	SYSTEM_INFO system_info_;
};

class WindowsMemoryScannerTest : public testing::Test {
public:
	void SetUp() override {}

	void TearDown() override {}
};

TEST_F(WindowsMemoryScannerTest, ScanMemoryTest) {
	const auto mock_api_wrapper = std::make_shared<MockWindowsApiWrapper>();

	WindowsMemoryScanner scanner(mock_api_wrapper);

	const EventInfo event_info(1, L"", L"home/home", 1, std::time(nullptr));

	EXPECT_CALL(*mock_api_wrapper, OpenProcess(testing::_, testing::_, testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*mock_api_wrapper, ReadProcessMemory(testing::_, testing::_, testing::_, testing::_, testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*mock_api_wrapper, VirtualQueryEx(testing::_, testing::_, testing::_, testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*mock_api_wrapper, GetSystemInfo(testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*mock_api_wrapper, CloseHandle(testing::_))
		.Times(testing::AtLeast(1));

	EXPECT_TRUE(scanner.ScanMemory(event_info, L"a", L"home"));
	EXPECT_FALSE(scanner.ScanMemory(event_info, L"b", L"home"));
	EXPECT_FALSE(scanner.ScanMemory(event_info, L"a", L"school"));
	EXPECT_FALSE(scanner.ScanMemory(event_info, L"b", L"school"));
}