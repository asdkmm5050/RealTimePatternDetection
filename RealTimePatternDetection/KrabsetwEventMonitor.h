#pragma once
#include "EventMonitor.h"
#include <krabs.hpp>

class KrabsetwEventMonitor : public EventMonitor {
public:
	KrabsetwEventMonitor();
	~KrabsetwEventMonitor() override;

	void Start() override;
	void Stop() override;
	void SetEventTriggeredCallback(const std::function<void(const EventInfo& In_event_info)>& In_callback) override;

private:
	void handleEvent(const EVENT_RECORD& In_record,
					 const krabs::trace_context& In_trace_context) const;

	static void getUidFromPid(const uint32_t& In_pid, std::wstring& Out_uid);

	static std::vector<std::wstring> getAllDrives();

	static std::wstring devicePathToDrivePath(const std::wstring& In_device_path);

	std::function<void(const EventInfo& In_process_info)> on_event_triggered_callback_;
	krabs::user_trace trace_;
	krabs::provider<> provider_;
};