#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../RealTimePatternDetection/KrabsetwEventMonitor.h"
#include "../RealTimePatternDetection/KrabsetwEventMonitor.cpp"

class MockUserTraceWrapper : public KrabsetwUserTraceWrapper {
public:
	MockUserTraceWrapper(const std::wstring& In_name) : KrabsetwUserTraceWrapper(In_name) {}
	~MockUserTraceWrapper() override = default;

	MOCK_METHOD(void, Enable, (const krabs::provider<>&), (override));
	MOCK_METHOD(void, Start, (), (override));
	MOCK_METHOD(void, Stop, (), (override));
};

class KrabsetwEventMonitorTest : public testing::Test {
protected:
	void SetUp() override {}

	void TearDown() override {}
};

TEST_F(KrabsetwEventMonitorTest, StartTest) {
	const auto session = std::make_shared<MockUserTraceWrapper>(L"MockMonitor");

	EXPECT_CALL(*session.get(), Start())
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*session.get(), Enable(testing::_))
		.Times(1);
	EXPECT_CALL(*session.get(), Stop())
		.Times(testing::AtLeast(1));

	KrabsetwEventMonitor monitor(session);
	monitor.Start();
	EXPECT_TRUE(monitor.detect_thread_is_running_);
}

TEST_F(KrabsetwEventMonitorTest, StopTest) {
	const auto session = std::make_shared<MockUserTraceWrapper>(L"MockMonitor");

	EXPECT_CALL(*session.get(), Start())
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*session.get(), Enable(testing::_))
		.Times(1);
	EXPECT_CALL(*session.get(), Stop())
		.Times(testing::AtLeast(1));

	KrabsetwEventMonitor monitor(session);
	monitor.Start();
	EXPECT_TRUE(monitor.detect_thread_is_running_);
	monitor.Stop();
	EXPECT_FALSE(monitor.detect_thread_is_running_);
}

class MockCallback {
public:
	MOCK_METHOD(void, ProcessStartCallback, (const EventInfo& event_info), (const));
};

TEST_F(KrabsetwEventMonitorTest, ProcessStartEventCallbackTest) {
	const auto session = std::make_shared<KrabsetwUserTraceWrapper>(L"MockMonitor");

	KrabsetwEventMonitor monitor(session);
	const MockCallback mock_callback;

	monitor.SetProcessStartEventTriggeredCallback([&](const EventInfo& In_event_info) {
		mock_callback.ProcessStartCallback(In_event_info);
	});

	EXPECT_CALL(mock_callback, ProcessStartCallback(::testing::_))
		.Times(testing::AtLeast(1));
	monitor.Start();
	system("start notepad.exe");
	system("taskkill /F /IM notepad.exe");
	monitor.Stop();
}