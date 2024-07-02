#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../RealTimePatternDetection/KrabsetwEventMonitor.h"
#include "../RealTimePatternDetection/KrabsetwEventMonitor.cpp"

class MockUserTraceWrapper : public KrabsetwUserTraceWrapper {
public:
	explicit MockUserTraceWrapper(const std::wstring& In_name) : KrabsetwUserTraceWrapper(In_name) {}
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
		ON_CALL(*this, SetSchema(testing::_)).WillByDefault(testing::Return());
		ON_CALL(*this, GetSchemaEventId()).WillByDefault(testing::Return(1));
	}
	~MockParserWrapper() override = default;
	MOCK_METHOD(uint32_t, ParseUInt32, (const std::wstring&), (override));
	MOCK_METHOD(std::wstring, ParseWString, (const std::wstring&), (override));
	MOCK_METHOD(void, SetSchema, (const krabs::schema&), (override));
	MOCK_METHOD(int, GetSchemaEventId, (), (override, const));
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

	KrabsetwEventMonitor monitor(session);
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

TEST_F(KrabsetwEventMonitorTest, CallbackShouldBeCalledAfterProcessStartEventTest) {
	const auto session = std::make_shared<KrabsetwUserTraceWrapper>(L"MockMonitor");
	MockParserWrapper parser;
	KrabsetwEventMonitor monitor(session);
	const MockCallback mock_callback;

	monitor.SetProcessStartEventTriggeredCallback([&](const EventInfo& In_event_info) {
		mock_callback.ProcessStartCallback(In_event_info);
	});

	EXPECT_CALL(mock_callback, ProcessStartCallback(::testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(parser, ParseUInt32(::testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(parser, ParseWString(::testing::_))
		.Times(testing::AtLeast(1));
	EXPECT_CALL(parser, GetSchemaEventId())
		.Times(testing::AtLeast(1));
	EXPECT_CALL(parser, SetSchema(::testing::_))
		.Times(testing::AtLeast(1));

	krabs::testing::record_builder builder(krabs::guid(L"{22FB2CD6-0E7B-422B-A0C7-2FAD1FD0E716}"), krabs::id(1), krabs::version(2));

	monitor.HandleProcessStartEvent(parser, builder.create_stub_record(), krabs::trace_context());
}