#include "ProcessDetector.h"

class ProcessDetectorImpl : public ProcessDetector {
public:
	ProcessDetectorImpl(const std::shared_ptr<EventMonitor>& In_event_monitor,
						const std::shared_ptr<MemoryScanner>& In_memory_scanner);

	~ProcessDetectorImpl() override;

	void Start() override;

	void Stop() override;

	void SetTargetString(const std::wstring& In_target) override;

	void SetFilePath(const std::wstring& In_file_path) override;
private:
	std::shared_ptr<EventMonitor> event_monitor_;
	std::shared_ptr<MemoryScanner> memory_scanner_;
};

ProcessDetectorImpl::ProcessDetectorImpl(const std::shared_ptr<EventMonitor>& In_event_monitor,
										 const std::shared_ptr<MemoryScanner>& In_memory_scanner) {
	this->event_monitor_ = In_event_monitor;
	this->memory_scanner_ = In_memory_scanner;
	this->event_monitor_->SetEventTriggeredCallback([this](const EventInfo& In_process_info) {
		if (this->memory_scanner_->ScanMemory(In_process_info)) {
			this->memory_scanner_->SaveEventInfo(In_process_info);
		}
	});
}

ProcessDetectorImpl::~ProcessDetectorImpl() = default;

void ProcessDetectorImpl::Start() {
	this->event_monitor_->Start();
}

void ProcessDetectorImpl::Stop() {
	this->event_monitor_->Stop();
}

void ProcessDetectorImpl::SetTargetString(const std::wstring& In_target) {
	this->memory_scanner_->SetTargetString(In_target);
}

void ProcessDetectorImpl::SetFilePath(const std::wstring& In_file_path) {
	this->memory_scanner_->SetFilePath(In_file_path);
}

ProcessDetector* ProcessDetector::Create(const std::shared_ptr<EventMonitor>& In_event_monitor,
										 const std::shared_ptr<MemoryScanner>& In_memory_scanner) {
	return new ProcessDetectorImpl(In_event_monitor, In_memory_scanner);
}

void ProcessDetector::Destroy(const ProcessDetector* In_process_detector) {
	delete In_process_detector;
}