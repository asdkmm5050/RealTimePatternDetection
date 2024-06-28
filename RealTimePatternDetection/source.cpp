#include <iostream>
#include <fstream>
#include "ProcessDetector.h"

int main() {
	std::ofstream err_file("error.log");
	std::cerr.rdbuf(err_file.rdbuf());

	const std::shared_ptr<EventMonitor> event_monitor(EventMonitor::Create(krabsetw),
													  &EventMonitor::Destroy);
	const std::shared_ptr<MemoryScanner> memory_scanner(MemoryScanner::Create(windows),
														&MemoryScanner::Destroy);
	const std::shared_ptr<ProcessDetector> process_detector(ProcessDetector::Create(event_monitor, memory_scanner),
															&ProcessDetector::Destroy);
	process_detector->SetTargetString(L"Code.exe");
	process_detector->SetFilePath(L"C:\\\\Users");

	try {
		process_detector->Start();
	} catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
	}

	err_file.close();
}
