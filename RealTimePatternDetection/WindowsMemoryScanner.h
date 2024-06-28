#pragma once
#include <vector>

#include "MemoryScanner.h"

class WindowsMemoryScanner : public MemoryScanner {
public:
	explicit WindowsMemoryScanner();
	~WindowsMemoryScanner() override;
	bool ScanMemory(const EventInfo& In_event_info) override;
	void SaveEventInfo(const EventInfo& In_event_info) override;
	void SetTargetString(const std::wstring& In_target) override;
	void SetFilePath(const std::wstring& In_file_path) override;

private:
	std::wstring target_string_;
	std::wstring file_path_;
};

