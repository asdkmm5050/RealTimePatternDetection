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

class MockParserWrapper : public KrabsetwParserWrapper {
public:
	MockParserWrapper() {
		ON_CALL(*this, ParseUInt32(testing::_)).WillByDefault(testing::Return(1234));
		ON_CALL(*this, ParseWString(testing::_)).WillByDefault(testing::Invoke([](const std::wstring&) {
			return L"home/home";
		}));
	}
	~MockParserWrapper() override = default;
	MOCK_METHOD(uint32_t, ParseUInt32, (const std::wstring&), (override));
	MOCK_METHOD(std::wstring, ParseWString, (const std::wstring&), (override));
};

class KrabsetwEventMonitorTest : public testing::Test {
protected:
	void SetUp() override {}

	void TearDown() override {}
};

TEST_F(KrabsetwEventMonitorTest, CheckMonitorThreadStatusAfterStartMonitorTest) {
	const auto session = std::make_shared<MockUserTraceWrapper>(L"MockMonitor");
	EXPECT_CALL(*session, Start())
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*session, Enable(testing::_))
		.Times(1);
	EXPECT_CALL(*session, Stop())
		.Times(testing::AtLeast(1));

	KrabsetwEventMonitor monitor(session,
								 std::make_shared<KrabsetwParserWrapper>());
	monitor.Start();
	EXPECT_TRUE(monitor.detect_thread_is_running_);
}

TEST_F(KrabsetwEventMonitorTest, CheckMonitorThreadStatusAfterStopMonitorTest) {
	const auto session = std::make_shared<MockUserTraceWrapper>(L"MockMonitor");

	EXPECT_CALL(*session, Start())
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*session, Enable(testing::_))
		.Times(1);
	EXPECT_CALL(*session, Stop())
		.Times(testing::AtLeast(1));

	KrabsetwEventMonitor monitor(session,
								 std::make_shared<KrabsetwParserWrapper>());
	monitor.Start();
	EXPECT_TRUE(monitor.detect_thread_is_running_);
	monitor.Stop();
	EXPECT_FALSE(monitor.detect_thread_is_running_);
}

class MockCallback {
public:
	MOCK_METHOD(void, ProcessStartCallback, (const EventInfo& event_info), (const));
};

TEST_F(KrabsetwEventMonitorTest, CallbackShouldBeCalledAfterProcessStartEventTest) {
	const auto session = std::make_shared<KrabsetwUserTraceWrapper>(L"MockMonitor");
	const auto parser = std::make_shared<MockParserWrapper>();
	KrabsetwEventMonitor monitor(session, parser);
	const MockCallback mock_callback;

	monitor.SetProcessStartEventTriggeredCallback([&](const EventInfo& In_event_info) {
		mock_callback.ProcessStartCallback(In_event_info);
	});

	EXPECT_CALL(mock_callback, ProcessStartCallback(::testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*parser, ParseUInt32(::testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(*parser, ParseWString(::testing::_))
		.Times(testing::AtLeast(1));

	EVENT_RECORD event_record;
	event_record.EventHeader.EventDescriptor.Id = 1;
	event_record.ExtendedData = PEVENT_HEADER_EXTENDED_DATA_ITEM();
	event_record.UserData = nullptr;
	event_record.UserContext = nullptr;

	monitor.HandleProcessStartEvent(event_record, krabs::trace_context());
}