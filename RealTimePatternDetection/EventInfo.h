#pragma once
#include <string>

class EventInfo {
public:
	EventInfo();
	~EventInfo();

	void SetPid(const int& In_pid);
	void SetUid(const std::wstring& In_uid);
	void SetFilePath(const std::wstring& In_file_path);

	int GetPid() const;
	std::wstring GetUid() const;
	std::wstring GetFilePath() const;

private:
	int pid_;
	std::wstring uid_;
	std::wstring file_path_;
};

inline EventInfo::EventInfo() :
	pid_(0) {}

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

inline int EventInfo::GetPid() const {
	return this->pid_;
}

inline std::wstring EventInfo::GetUid() const {
	return this->uid_;
}

inline std::wstring EventInfo::GetFilePath() const {
	return this->file_path_;
}
