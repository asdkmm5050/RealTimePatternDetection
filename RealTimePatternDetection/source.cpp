#include <iostream>
#include <fstream>
#include <thread>

#include "ProcessDetector.h"

int main() {
	std::ofstream err_file("error.log");
	std::cerr.rdbuf(err_file.rdbuf());
	{
		const std::shared_ptr<ProcessDetector> process_detector(
			ProcessDetector::Create(EventMonitorType::krabsetw, MemoryScannerType::windows),
			&ProcessDetector::Destroy);

		process_detector->SetTargetString(L"Code.exe");
		process_detector->SetFilePath(L"C:\\\\Users");

		process_detector->Start();

		std::string tmp;
		std::cin >> tmp;//Just for stop

		process_detector->Stop();
	}
	err_file.close();
}
