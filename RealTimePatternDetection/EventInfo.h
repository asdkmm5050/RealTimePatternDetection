#pragma once
#include <string>

class EventInfo {
public:
	EventInfo();
	EventInfo(const int& In_pid,
			  const std::wstring& In_uid,
			  const std::wstring& In_file_path,
			  const int& In_event_type_id,
			  const time_t& In_time);
	~EventInfo();

	void SetPid(const int& In_pid);
	void SetUid(const std::wstring& In_uid);
	void SetFilePath(const std::wstring& In_file_path);
	void SetEventTypeId(const int& In_type_id);
	void SetEventTime(const time_t& In_time);

	int GetPid() const;
	std::wstring GetUid() const;
	std::wstring GetFilePath() const;
	int GetEventTypeId() const;
	time_t GetEventTime() const;

private:
	int pid_;
	std::wstring uid_;
	std::wstring file_path_;
	int event_type_id_;
	time_t event_time_;
};

inline EventInfo::EventInfo() :
	pid_(0),
	event_type_id_(0),
	event_time_(0) {}

inline EventInfo::EventInfo(const int& In_pid, const std::wstring& In_uid, const std::wstring& In_file_path, const int& In_event_type_id, const time_t& In_time) :
	pid_(In_pid),
	uid_(In_uid),
	file_path_(In_file_path),
	event_type_id_(In_event_type_id),
	event_time_(In_time) {}

inline EventInfo::~EventInfo() = default;

inline void EventInfo::SetPid(const int& In_pid) {
	this->pid_ = In_pid;
}

inline void EventInfo::SetUid(const std::wstring& In_uid) {
	this->uid_ = In_uid;
}

inline void EventInfo::SetFilePath(const std::wstring& In_file_path) {
	this->file_path_ = In_file_path;
}

inline void EventInfo::SetEventTypeId(const int& In_type_id) {
	this->event_type_id_ = In_type_id;
}

inline void EventInfo::SetEventTime(const time_t& In_time) {
	this->event_time_ = In_time;
}

inline int EventInfo::GetPid() const {
	return this->pid_;
}

inline std::wstring EventInfo::GetUid() const {
	return this->uid_;
}

inline std::wstring EventInfo::GetFilePath() const {
	return this->file_path_;
}

inline int EventInfo::GetEventTypeId() const {
	return this->event_type_id_;
}

inline time_t EventInfo::GetEventTime() const {
	return this->event_time_;
}
