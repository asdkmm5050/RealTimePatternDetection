#pragma once
#include "EventInfo.h"

class EventInfoDao {
public:
	/**
	 * @brief Get ProcessInfoDao singleton class
	 * @return The only instance
	 */
	static EventInfoDao& GetInstance();

	/**
	 * @brief Add ProcessInfo to the queue to write file
	 * @param In_event_info Input ProcessInfo
	 */
	virtual void AddIntoQueue(const EventInfo& In_event_info) = 0;

	EventInfoDao(const EventInfoDao&) = delete;
	EventInfoDao& operator=(const EventInfoDao&) = delete;

protected:
	EventInfoDao() = default;
	virtual ~EventInfoDao() = default;
};
