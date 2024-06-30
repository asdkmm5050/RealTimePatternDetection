#pragma once
#include "EventInfo.h"

// Enum defining types of memory scanners
enum class MemoryScannerType {
    windows,  // Type of memory scanner for Windows
};

// Abstract class representing a Memory Scanner
class MemoryScanner {
public:
    /**
     * @brief Factory method to create a memory scanner instance based on type.
     * @param In_type Type of memory scanner to create.
     * @return Pointer to the created MemoryScanner instance.
     */
    static MemoryScanner* Create(const MemoryScannerType& In_type);

    /**
     * @brief Destructor to destroy the memory scanner instance.
     * @param In_memory_scanner Pointer to the MemoryScanner instance to destroy.
     */
    static void Destroy(const MemoryScanner* In_memory_scanner);

    /**
     * @brief Scan process memory to find a target string.
     * @param In_event_info Information about the process event to scan.
     * @param In_target_string Target string to search for in process memory.
     * @param In_file_path Path of event process.
     * @return true if the target string is found in memory, false otherwise.
     */
    virtual bool ScanMemory(const EventInfo& In_event_info, const std::wstring& In_target_string, const std::wstring& In_file_path) = 0;

    /**
     * @brief Set the target string to scan for in process memory.
     * @param In_target Target string to set.
     */
    virtual void SetTargetString(const std::wstring& In_target) = 0;

    /**
     * @brief Set the file path for handling scanning failures.
     * @param In_file_path File path to set.
     */
    virtual void SetFilePath(const std::wstring& In_file_path) = 0;

    /**
     * @brief Get the target string currently set for memory scanning.
     * @return Current target string.
     */
    virtual std::wstring GetTargetString() = 0;

    /**
     * @brief Get the file path currently set for handling scanning failures.
     * @return Current file path.
     */
    virtual std::wstring GetFilePath() = 0;

protected:
    // Protected destructor to ensure the class is abstract.
    virtual ~MemoryScanner() = default;
};
