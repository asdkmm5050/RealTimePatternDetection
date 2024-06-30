#include "KrabsetwEventMonitor.h"
#include <iostream>

#include "EventInfo.h"

KrabsetwEventMonitor::KrabsetwEventMonitor(const std::shared_ptr<KrabsetwUserTraceWrapper>& In_session) :
	session_(In_session),
	provider_(L"Microsoft-Windows-Kernel-Process"),
	detect_thread_is_running_(false) {
	this->on_process_start_callback_ = [](const EventInfo&) {
		std::cout << "KrabsetwEventMonitor::KrabsetwEventMonitor : Event triggered callback not setup yet" << "\n";
	};
	this->provider_.any(0x10);
	this->provider_.add_on_event_callback([&](const EVENT_RECORD& In_record, const krabs::trace_context& In_trace_context) {
		try {
			this->HandleProcessStartEvent(In_record, In_trace_context);
		} catch (const krabs::could_not_find_schema&) {
			throw;
		} catch (const std::exception& e) {
			std::cerr << e.what() << "\n";
		}
	});

	this->session_->Enable(this->provider_);
}

KrabsetwEventMonitor::~KrabsetwEventMonitor() {
	KrabsetwEventMonitor::Stop();
}

void KrabsetwEventMonitor::Start() {
	if (this->detect_thread_is_running_) {
		return;
	}
	this->detect_thread_ = std::thread([this] {
		this->session_->Start();
	});
	this->detect_thread_is_running_ = true;
}

void KrabsetwEventMonitor::Stop() {
	if (this->detect_thread_is_running_) {
		this->session_->Stop();
		this->detect_thread_is_running_ = false;
		if (this->detect_thread_.joinable()) {
			this->detect_thread_.join();
		}
	}
}

void KrabsetwEventMonitor::SetProcessStartEventTriggeredCallback(const std::function<void(const EventInfo& In_event_info)>& In_callback) {
	this->on_process_start_callback_ = In_callback;
}

void KrabsetwEventMonitor::HandleProcessStartEvent(const EVENT_RECORD& In_record,
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
		event_info.SetEventTypeId(schema.event_id());
		event_info.SetEventTime(std::time(nullptr));

		std::wstringstream ss;
		ss << L"Process Created: PID=" << event_info.GetPid()
			<< L", UID=" << event_info.GetUid()
			<< L", FilePath=" << event_info.GetFilePath()
			<< L", EventTypeId=" << event_info.GetEventTypeId()
			<< L", EventTime=" << event_info.GetEventTime() << '\n';

		std::wcout << ss.str();

		this->on_process_start_callback_(event_info);
	}
}

void KrabsetwEventMonitor::getUidFromPid(const uint32_t& In_pid, std::wstring& Out_uid) {
	HANDLE process = nullptr;
	HANDLE token = nullptr;
	PTOKEN_USER token_user = nullptr;
	LPWSTR user_sid = nullptr;

	try {
		process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, In_pid);
		if (process == nullptr) {
			throw std::runtime_error("OpenProcess failed");
		}

		if (!OpenProcessToken(process, TOKEN_QUERY, &token)) {
			throw std::runtime_error("OpenProcessToken failed");
		}

		DWORD size = 0;
		GetTokenInformation(token, TokenUser, nullptr, 0, &size);
		token_user = reinterpret_cast<PTOKEN_USER>(new BYTE[size]);

		if (!GetTokenInformation(token, TokenUser, token_user, size, &size)) {
			throw std::runtime_error("GetTokenInformation failed");
		}

		if (!ConvertSidToStringSid(token_user->User.Sid, &user_sid)) {
			throw std::runtime_error("ConvertSidToStringSid failed");
		}

		Out_uid = user_sid;

	} catch (const std::exception& e) {
		std::cerr << "KrabsetwEventMonitor::getUidFromPid : PID:" << In_pid << " Error: " << e.what() << '\n';
	}

	if (user_sid != nullptr) {
		LocalFree(user_sid);
	}
	if (token_user != nullptr) {
		delete[] reinterpret_cast<BYTE*>(token_user);
	}
	if (token != nullptr) {
		CloseHandle(token);
	}
	if (process != nullptr) {
		CloseHandle(process);
	}
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

KrabsetwUserTraceWrapper::KrabsetwUserTraceWrapper(const std::wstring& In_session_name) :
	session_(std::make_shared<krabs::user_trace>(In_session_name)) {}

KrabsetwUserTraceWrapper::~KrabsetwUserTraceWrapper() = default;

void KrabsetwUserTraceWrapper::Enable(const krabs::provider<>& In_provider) {
	this->session_->enable(In_provider);
}

void KrabsetwUserTraceWrapper::Start() {
	this->session_->start();
}

void KrabsetwUserTraceWrapper::Stop() {
	this->session_->stop();
}
