#pragma once

#include "EventMonitor.h"
#include "MemoryScanner.h"

class ProcessDetector {
public:
	/**
	 * @brief Create ProcessDetector with an event monitor and a memory scanner
	 * @param In_monitor_type Input EventMonitor type
	 * @param In_scanner_type Input MemoryScanner type
	 * @return ProcessDetector ptr
	 */
	static ProcessDetector* Create(const EventMonitorType& In_monitor_type,
								   const MemoryScannerType& In_scanner_type);

	/**
	 * @brief Destroy ProcessDetector
	 * @param In_process_detector Input ProcessDetector ptr
	 */
	static void Destroy(const ProcessDetector* In_process_detector);

	/**
	 * @brief Start EventMonitor
	 */
	virtual void Start() = 0;

	/**
	 * @brief Stop EventMonitor
	 */
	virtual void Stop() = 0;

	/**
	 * @brief Set target string for scan memory
	 * @param In_target Input target string
	 */
	virtual void SetTargetString(const std::wstring& In_target) = 0;

	/**
	 * @brief Set file path for scan memory
	 * @param In_file_path Input file path string
	 */
	virtual void SetFilePath(const std::wstring& In_file_path) = 0;

protected:
	virtual ~ProcessDetector() = default;
};