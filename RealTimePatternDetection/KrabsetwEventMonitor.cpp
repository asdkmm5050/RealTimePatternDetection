#include "KrabsetwEventMonitor.h"
#include <iostream>

#include "EventInfo.h"

KrabsetwEventMonitor::KrabsetwEventMonitor() :
	trace_(L"KrabsetwEventMonitor"),
	provider_(L"Microsoft-Windows-Kernel-Process") {
	this->on_event_triggered_callback_ = [](const EventInfo&) {
		std::cout << "KrabsetwEventMonitor::KrabsetwEventMonitor : Event triggered callback not setup yet" << "\n";
	};
	this->provider_.any(0x10);
	this->provider_.add_on_event_callback([&](const EVENT_RECORD& In_record, const krabs::trace_context& In_trace_context) {
		try {
			this->handleEvent(In_record, In_trace_context);
		} catch (const krabs::could_not_find_schema&) {
			throw;
		} catch (const std::exception& e) {
			std::cerr << e.what() << "\n";
		}
	});

	this->trace_.enable(this->provider_);
}

KrabsetwEventMonitor::~KrabsetwEventMonitor() {
	KrabsetwEventMonitor::Stop();
}

void KrabsetwEventMonitor::Start() {
	this->trace_.start();
}

void KrabsetwEventMonitor::Stop() {
	this->trace_.stop();
}

void KrabsetwEventMonitor::SetEventTriggeredCallback(const std::function<void(const EventInfo& In_event_info)>& In_callback) {
	this->on_event_triggered_callback_ = In_callback;
}

void KrabsetwEventMonitor::handleEvent(const EVENT_RECORD& In_record,
									   const krabs::trace_context& In_trace_context) const {
	const krabs::schema schema(In_record, In_trace_context.schema_locator);
	krabs::parser parser(schema);

	if (schema.event_id() == 1) { // Process Start

		EventInfo event_info;
		event_info.SetPid(static_cast<int>(parser.parse<uint32_t>(L"ProcessID")));
		event_info.SetFilePath(devicePathToDrivePath(parser.parse<std::wstring>(L"ImageName")));
		std::wstring uid;
		getUidFromPid(event_info.GetPid(), uid);
		event_info.SetUid(uid);

		std::wstringstream ss;
		ss << L"Process Created: PID=" << event_info.GetPid()
			<< L", UID=" << event_info.GetUid()
			<< L", FilePath=" << event_info.GetFilePath() << '\n';

		std::wcout << ss.str();

		this->on_event_triggered_callback_(event_info);
	}
}

void KrabsetwEventMonitor::getUidFromPid(const uint32_t& In_pid,
										 std::wstring& Out_uid) {
	const HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, In_pid);
	if (process == nullptr) {
		std::cerr << "KrabsetwEventMonitor::getUidFromPid : PID:" << In_pid << " OpenProcess failed. Error: " << GetLastError() << '\n';
		return;
	}

	HANDLE token;
	if (!OpenProcessToken(process, TOKEN_QUERY, &token)) {
		std::cerr << "KrabsetwEventMonitor::getUidFromPid : PID:" << In_pid << " OpenProcessToken failed. Error: " << GetLastError() << '\n';
		CloseHandle(process);
		return;
	}

	DWORD size = 0;
	GetTokenInformation(token, TokenUser, nullptr, 0, &size);
	const auto token_user = reinterpret_cast<PTOKEN_USER>(new BYTE[size]);

	if (!GetTokenInformation(token, TokenUser, token_user, size, &size)) {
		std::cerr << "KrabsetwEventMonitor::getUidFromPid : PID:" << In_pid << " GetTokenInformation failed. Error: " << GetLastError() << '\n';
		CloseHandle(token);
		CloseHandle(process);
		delete[] token_user;
		return;
	}

	LPWSTR user_sid;
	ConvertSidToStringSid(token_user->User.Sid, &user_sid);

	Out_uid = static_cast<std::wstring>(user_sid);

	CloseHandle(token);
	CloseHandle(process);
	delete[] token_user;
}

std::vector<std::wstring> KrabsetwEventMonitor::getAllDrives() {
	std::vector<std::wstring> drives;
	wchar_t drive_letters[256];
	GetLogicalDriveStringsW(std::size(drive_letters), drive_letters);

	wchar_t* current_drive = drive_letters;
	while (*current_drive) {
		drives.emplace_back(current_drive);
		current_drive += wcslen(current_drive) + 1;
	}

	return drives;
}

std::wstring KrabsetwEventMonitor::devicePathToDrivePath(const std::wstring& In_device_path) {
	const std::vector<std::wstring> drives = getAllDrives();

	for (const auto& drive : drives) {
		wchar_t device_name[MAX_PATH];
		if (QueryDosDeviceW(drive.substr(0, 2).c_str(), device_name, MAX_PATH)) {
			std::wstring device_prefix(device_name);
			if (In_device_path.find(device_prefix) == 0) {
				std::wstring drive_path = drive + In_device_path.substr(device_prefix.length());
				return drive_path;
			}
		}
	}

	return L"";
}