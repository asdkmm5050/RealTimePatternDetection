#pragma once
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fstream>
#include <thread>
#include <nlohmann/json.hpp>

#include "../RealTimePatternDetection/EventInfoJsonGenerator.h"
#include "../RealTimePatternDetection/EventInfoJsonGenerator.cpp"

class MockEventInfoJsonGeneratorImpl : public EventInfoJsonGeneratorImpl {
public:
	MOCK_METHOD(void, Save, (const std::wstring&), (override));
};

class EventInfoJsonGeneratorTest : public testing::Test {
protected:
	void SetUp() override {
		std::remove("test.json");
	}

	void TearDown() override {
		std::remove("test.json");
	}
};

TEST_F(EventInfoJsonGeneratorTest, AddIntoQueueTest) {
	EventInfoJsonGeneratorImpl generator;

	EventInfo event;
	event.SetPid(1234);
	event.SetUid(L"test_uid");
	event.SetFilePath(L"test_path");

	generator.AddIntoQueue(event);
	EXPECT_EQ(generator.event_infos_.size(), 1);
	generator.AddIntoQueue(event);
	EXPECT_EQ(generator.event_infos_.size(), 2);
	generator.AddIntoQueue(event);
	EXPECT_EQ(generator.event_infos_.size(), 3);
	std::remove("test.json");
}

TEST_F(EventInfoJsonGeneratorTest, SaveTest) {
	EventInfoJsonGeneratorImpl generator;

	EventInfo event1;
	event1.SetPid(1234);
	event1.SetUid(L"test_uid_1");
	event1.SetFilePath(L"test_path_1");

	EventInfo event2;
	event2.SetPid(5678);
	event2.SetUid(L"test_uid_2");
	event2.SetFilePath(L"test_path_2");

	generator.AddIntoQueue(event1);
	generator.AddIntoQueue(event2);

	generator.Save(L"test.json");

	std::ifstream in_file_stream(L"test.json");

	ASSERT_TRUE(in_file_stream.is_open());

	nlohmann::json j;
	in_file_stream >> j;

	ASSERT_EQ(j.size(), 2);
	EXPECT_EQ(j[0]["pid"], 1234);
	EXPECT_EQ(j[0]["uid"], "test_uid_1");
	EXPECT_EQ(j[0]["file_path"], "test_path_1");

	EXPECT_EQ(j[1]["pid"], 5678);
	EXPECT_EQ(j[1]["uid"], "test_uid_2");
	EXPECT_EQ(j[1]["file_path"], "test_path_2");
}

TEST_F(EventInfoJsonGeneratorTest, StartSaveTest) {
	EventInfoJsonGeneratorImpl generator;
	generator.StartSave();

	EXPECT_TRUE(generator.save_event_thread_is_running_);
	EXPECT_TRUE(generator.save_event_thread_.joinable());
	generator.StopSave();
}

TEST_F(EventInfoJsonGeneratorTest, StopSaveTest) {
	EventInfoJsonGeneratorImpl generator;
	generator.StartSave();
	generator.StopSave();
	EXPECT_FALSE(generator.save_event_thread_is_running_);
}

TEST_F(EventInfoJsonGeneratorTest, AutoSavingFundationTest) {
	MockEventInfoJsonGeneratorImpl mock_event_info_json_generator;
	mock_event_info_json_generator.SetSaveInterval(0);

	EXPECT_CALL(mock_event_info_json_generator, Save(testing::_))
		.Times(testing::AtLeast(1));

	EventInfo event;
	event.SetPid(1234);
	event.SetUid(L"test_uid");
	event.SetFilePath(L"test_path");

	mock_event_info_json_generator.AddIntoQueue(event);
	mock_event_info_json_generator.StartSave();
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	mock_event_info_json_generator.StopSave();
}