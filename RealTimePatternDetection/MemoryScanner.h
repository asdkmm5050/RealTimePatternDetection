#pragma once
#include "EventInfo.h"

enum MemoryScannerType {
	windows,
};

class MemoryScanner {
public:
	/**
	 * @brief Create memory scanner
	 * @return MemoryScanner ptr
	 */
	static MemoryScanner* Create(const MemoryScannerType& In_type);

	/**
	 * @brief Destroy memory scanner
	 * @param In_memory_scanner Input MemoryScanner ptr
	 */
	static void Destroy(const MemoryScanner* In_memory_scanner);

	/**
	 * @brief Scan process memory
	 * @param In_event_info Input process info
	 * @return true:target matched false:not matched
	 */
	virtual bool ScanMemory(const EventInfo& In_event_info) = 0;

	/**
	 * @brief Save ProcessInfo
	 * @param In_event_info Input ProcessInfo
	 */
	virtual void SaveEventInfo(const EventInfo& In_event_info) = 0;

	/**
	 * @brief Set target string for memory scan
	 * @param In_target Input target string
	 */
	virtual void SetTargetString(const std::wstring& In_target) = 0;

	/**
	 * @brief Set file path for memory scan
	 * @param In_file_path Input file path string
	 */
	virtual void SetFilePath(const std::wstring& In_file_path) = 0;

protected:
	virtual ~MemoryScanner() = default;
};