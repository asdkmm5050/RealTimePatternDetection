#include "EventInfoJsonGenerator.h"

#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#include "nlohmann/json.hpp"
#include <gtest/gtest_prod.h>

class EventInfoJsonGeneratorTest;

class EventInfoJsonGeneratorImpl : public EventInfoJsonGenerator {
public:

	friend class EventInfoJsonGeneratorTest;
	FRIEND_TEST(EventInfoJsonGeneratorTest, QueueSizeShouldBeIncreasedAfterAddToQueueTest);
	FRIEND_TEST(EventInfoJsonGeneratorTest, CheckSavingThreadStatusAfterStartSaveTest);
	FRIEND_TEST(EventInfoJsonGeneratorTest, CheckSavingThreadStatusAfterStopSaveTest);
	FRIEND_TEST(EventInfoJsonGeneratorTest, SaveEventInfoIntoAJsonFileTest);
	FRIEND_TEST(EventInfoJsonGeneratorTest, AutoSavingFeatureTest);

	EventInfoJsonGeneratorImpl();
	~EventInfoJsonGeneratorImpl() override;

	void AddIntoQueue(const EventInfo& In_event_info) override;

	void StartSave() override;

	void StopSave() override;

	void Save(const std::wstring& In_file_name) override;

	void SetSaveInterval(const int& In_sec) override;

private:
	static std::string ws2s(const std::wstring& In_w_str);

	std::vector<EventInfo> event_infos_;
	std::thread save_event_thread_;
	std::mutex event_infos_mutex_;
	std::condition_variable save_event_thread_cv_;
	std::mutex save_event_thread_cv_mutex_;
	bool save_event_thread_is_running_;
	time_t interval_time_;
};

EventInfoJsonGeneratorImpl::EventInfoJsonGeneratorImpl() :
	save_event_thread_is_running_(false),
	interval_time_(INT_MAX) {}

EventInfoJsonGeneratorImpl::~EventInfoJsonGeneratorImpl() {
	EventInfoJsonGeneratorImpl::StopSave();
	EventInfoJsonGeneratorImpl::Save(std::to_wstring(std::time(nullptr)) + L".json");
}

void EventInfoJsonGeneratorImpl::AddIntoQueue(const EventInfo& In_event_info) {
	std::lock_guard<std::mutex>auto_lock(this->event_infos_mutex_);
	this->event_infos_.push_back(In_event_info);
}

void EventInfoJsonGeneratorImpl::StartSave() {
	if (this->save_event_thread_is_running_) {
		return;
	}
	this->save_event_thread_is_running_ = true;

	bool thread_started(false);

	this->save_event_thread_ = std::thread([&] {
		thread_started = true;
		while (this->save_event_thread_is_running_) {
			std::unique_lock<std::mutex> lock(this->save_event_thread_cv_mutex_);
			if (this->save_event_thread_cv_.wait_for(lock, std::chrono::seconds(this->interval_time_), [&] {return !this->save_event_thread_is_running_; })) {
				break;
			}
			this->Save(std::to_wstring(std::time(nullptr)) + L".json");
		}
	});

	while (!thread_started);
}

void EventInfoJsonGeneratorImpl::StopSave() {
	if (this->save_event_thread_is_running_) {
		this->save_event_thread_is_running_ = false;
		this->save_event_thread_cv_.notify_all();
		if (this->save_event_thread_.joinable()) {
			this->save_event_thread_.join();
		}
	}
}
void EventInfoJsonGeneratorImpl::Save(const std::wstring& In_file_name) {
	if (!this->event_infos_.empty()) {
		nlohmann::json j;
		std::lock_guard<std::mutex>auto_lock(this->event_infos_mutex_);
		for (const auto& info : this->event_infos_) {
			j.push_back({
				{"pid", info.GetPid()},
				{"uid", ws2s(info.GetUid())},
				{"file_path", ws2s(info.GetFilePath())},
				{"event_type", info.GetEventTypeId()},
				{"event_time", info.GetEventTime()} });
		}
		std::ofstream out_file_stream(In_file_name, std::ofstream::app);
		auto json_str = j.dump(4);
		out_file_stream << std::setw(4) << j;
		out_file_stream.close();
		this->event_infos_.clear();
	}
}

void EventInfoJsonGeneratorImpl::SetSaveInterval(const int& In_sec) {
	this->interval_time_ = In_sec;
}

std::string EventInfoJsonGeneratorImpl::ws2s(const std::wstring& In_w_str) {
	const std::string str(In_w_str.begin(), In_w_str.end());
	return str;
}

EventInfoJsonGenerator& EventInfoJsonGenerator::GetInstance() {
	static EventInfoJsonGeneratorImpl instance;
	return instance;
}