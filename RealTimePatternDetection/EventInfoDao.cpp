#include "EventInfoDao.h"

#include <fstream>
#include <mutex>
#include <thread>
#include <vector>

#include "nlohmann/json.hpp"

class EventInfoDaoImpl : public EventInfoDao {
public:
	EventInfoDaoImpl();
	~EventInfoDaoImpl() override;

	void AddIntoQueue(const EventInfo& In_event_info) override;

private:
	static std::string ws2s(const std::wstring& In_w_str);

	std::vector<EventInfo> event_infos_;
	std::thread save_event_thread_;
	std::mutex save_event_thread_mutex_;
	bool save_event_thread_is_running_;
};

EventInfoDaoImpl::EventInfoDaoImpl() :
	save_event_thread_is_running_(true) {
	save_event_thread_ = std::thread([&] {
		while (save_event_thread_is_running_) {
			if (this->event_infos_.empty()) continue;

			nlohmann::json j;
			{
				std::lock_guard<std::mutex>auto_lock(this->save_event_thread_mutex_);
				for (const auto& info : this->event_infos_) {
					j.push_back({
						{"pid", info.GetPid()},
						{"uid", ws2s(info.GetUid())},
						{"file_path", ws2s(info.GetFilePath())} });
				}
				this->event_infos_.clear();
			}
			std::ofstream out_file_stream(std::to_string(std::time(nullptr)) + ".json", std::ofstream::out);
			out_file_stream << std::setw(4) << j;
			out_file_stream.close();
			std::this_thread::sleep_for(std::chrono::seconds(30));
		}
	});
};

EventInfoDaoImpl::~EventInfoDaoImpl() {
	save_event_thread_is_running_ = false;
	if (save_event_thread_.joinable()) {
		save_event_thread_.join();
	}
}

void EventInfoDaoImpl::AddIntoQueue(const EventInfo& In_event_info) {
	std::lock_guard<std::mutex>auto_lock(this->save_event_thread_mutex_);
	this->event_infos_.push_back(In_event_info);
}

std::string EventInfoDaoImpl::ws2s(const std::wstring& In_w_str) {
	const std::string str(In_w_str.begin(), In_w_str.end());
	return str;
}

EventInfoDao& EventInfoDao::GetInstance() {
	static EventInfoDaoImpl instance;
	return instance;
}