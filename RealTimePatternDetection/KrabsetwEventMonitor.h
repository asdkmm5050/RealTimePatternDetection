#pragma once
#include "EventMonitor.h"
#include <krabs.hpp>
#include <thread>
#include <gtest/gtest_prod.h>

class KrabsetwEventMonitorTest;

class KrabsetwUserTraceWrapper {
public:
	KrabsetwUserTraceWrapper(const std::wstring& In_session_name);
	virtual ~KrabsetwUserTraceWrapper();
	virtual void Enable(const krabs::provider<>& In_provider);
	virtual void Start();
	virtual void Stop();

private:
	std::shared_ptr<krabs::user_trace> session_;
};

class KrabsetwEventMonitor : public EventMonitor {
public:
	friend class KrabsetwEventMonitorTest;
	FRIEND_TEST(KrabsetwEventMonitorTest, CheckMonitorThreadStatusAfterStartMonitorTest);
	FRIEND_TEST(KrabsetwEventMonitorTest, CheckMonitorThreadStatusAfterStopMonitorTest);
	FRIEND_TEST(KrabsetwEventMonitorTest, CallbackShouldBeCalledAfterProcessStartEventTest);

	KrabsetwEventMonitor(const std::shared_ptr<KrabsetwUserTraceWrapper>& In_session);
	~KrabsetwEventMonitor() override;

	void Start() override;
	void Stop() override;
	void SetProcessStartEventTriggeredCallback(const std::function<void(const EventInfo& In_event_info)>& In_callback) override;
	void HandleProcessStartEvent(const EVENT_RECORD& In_record,
								 const krabs::trace_context& In_trace_context) const;

private:

	static void getUidFromPid(const uint32_t& In_pid, std::wstring& Out_uid);

	static std::vector<std::wstring> getAllDrives();

	static std::wstring devicePathToDrivePath(const std::wstring& In_device_path);

	std::function<void(const EventInfo& In_process_info)> on_process_start_callback_;
	std::shared_ptr<KrabsetwUserTraceWrapper> session_;
	krabs::provider<> provider_;
	std::thread detect_thread_;
	bool detect_thread_is_running_;
};