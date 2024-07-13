#include <iostream>
#include <thread>

#include "ProcessDetector.h"

int main() {
	const std::shared_ptr<ProcessDetector> process_detector(
		ProcessDetector::Create(EventMonitorType::krabsetw, MemoryScannerType::windows),
		&ProcessDetector::Destroy);

	process_detector->SetTargetString(L"i am a shellcode");
	process_detector->SetFilePath(L"C:\\\\Users");
	process_detector->Start();

	std::string tmp;
	std::cin >> tmp;//Just for stop

	process_detector->Stop();
}
