#pragma once
#include "EventInfo.h"

// Class for generating JSON files from event information
class EventInfoJsonGenerator {
public:
	/**
	 * @brief Get the singleton instance of EventInfoJsonGenerator.
	 *        Saves events to a JSON file every 30 seconds if there is at least one event.
	 * @return Reference to the singleton instance.
	 */
	static EventInfoJsonGenerator& GetInstance();

	/**
	 * @brief Add event information to the queue for writing to a file.
	 * @param In_event_info Event information to add.
	 */
	virtual void AddIntoQueue(const EventInfo& In_event_info) = 0;

	/**
	 * @brief Start the process of saving events at regular intervals.
	 */
	virtual void StartSave() = 0;

	/**
	 * @brief Stop the process of saving events.
	 */
	virtual void StopSave() = 0;

	/**
	 * @brief Save events into a JSON file.
	 * @param In_file_name Name of the file to save the events.
	 */
	virtual void Save(const std::wstring& In_file_name) = 0;

	/**
	 * @brief Set the interval for saving events.
	 * @param In_sec Interval in seconds.
	 */
	virtual void SetSaveInterval(const int& In_sec) = 0;

	// Deleted copy constructor and assignment operator to enforce singleton pattern
	EventInfoJsonGenerator(const EventInfoJsonGenerator&) = delete;
	EventInfoJsonGenerator& operator=(const EventInfoJsonGenerator&) = delete;

protected:
	// Protected constructor and destructor to enforce singleton pattern
	EventInfoJsonGenerator() = default;
	virtual ~EventInfoJsonGenerator() = default;
};
