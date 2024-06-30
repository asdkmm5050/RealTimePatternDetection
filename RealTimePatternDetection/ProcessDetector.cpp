#include "ProcessDetector.h"

#include <iostream>
#include <memory>

#include "EventInfoJsonGenerator.h"

class ProcessDetectorImpl : public ProcessDetector {
public:
	ProcessDetectorImpl(const EventMonitorType& In_monitor_type,
						const MemoryScannerType& In_scanner_type);

	~ProcessDetectorImpl() override;

	void Start() override;

	void Stop() override;

	void SetTargetString(const std::wstring& In_target) override;

	void SetFilePath(const std::wstring& In_file_path) override;
private:
	std::shared_ptr<EventMonitor> event_monitor_;
	std::shared_ptr<MemoryScanner> memory_scanner_;
};

ProcessDetectorImpl::ProcessDetectorImpl(const EventMonitorType& In_monitor_type,
										 const MemoryScannerType& In_scanner_type) {
	this->event_monitor_ = std::shared_ptr<EventMonitor>(EventMonitor::Create(In_monitor_type), &EventMonitor::Destroy);
	this->memory_scanner_ = std::shared_ptr<MemoryScanner>(MemoryScanner::Create(In_scanner_type), &MemoryScanner::Destroy);
	this->event_monitor_->SetProcessStartEventTriggeredCallback([this](const EventInfo& In_process_info) {
		if (this->memory_scanner_->ScanMemory(In_process_info, this->memory_scanner_->GetTargetString(), this->memory_scanner_->GetFilePath())) {
			EventInfoJsonGenerator::GetInstance().AddIntoQueue(In_process_info);
		}
	});
}

ProcessDetectorImpl::~ProcessDetectorImpl() = default;

void ProcessDetectorImpl::Start() {
	try {
		this->event_monitor_->Start();
	} catch (const std::exception& exception) {
		std::cerr << "ProcessDetectorImpl::Start() : " << exception.what() << "\n";
		std::cout << "ProcessDetectorImpl::Start() : " << exception.what() << "\n";
	}
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

ProcessDetector* ProcessDetector::Create(const EventMonitorType& In_monitor_type,
										 const MemoryScannerType& In_scanner_type) {
	return new ProcessDetectorImpl(In_monitor_type, In_scanner_type);
}

void ProcessDetector::Destroy(const ProcessDetector* In_process_detector) {
	delete In_process_detector;
}