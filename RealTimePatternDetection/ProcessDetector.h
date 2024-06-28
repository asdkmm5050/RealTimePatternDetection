#pragma once
#include <memory>

#include "EventMonitor.h"
#include "MemoryScanner.h"

class ProcessDetector {
public:
	/**
	 * @brief Create ProcessDetector
	 * @param In_event_monitor Input EventMonitor shared_ptr
	 * @param In_memory_scanner Input MemoryScanner shared_ptr
	 * @return ProcessDetector ptr
	 */
	static ProcessDetector* Create(const std::shared_ptr<EventMonitor>& In_event_monitor,
	                               const std::shared_ptr<MemoryScanner>& In_memory_scanner);

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