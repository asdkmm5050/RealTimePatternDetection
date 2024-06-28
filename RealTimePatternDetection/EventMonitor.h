#pragma once
#include <functional>
#include "EventInfo.h"

enum EventMonitorType
{
	krabsetw,
};

class EventMonitor {
public:
	/**
	 * @brief Create event monitor
	 * @param In_type Monitor type
	 * @return Monitor ptr
	 */
	static EventMonitor* Create(const EventMonitorType& In_type);

	/**
	 * @brief Destroy event monitor
	 * @param In_event_monitor Input Monitor ptr
	 */
	static void Destroy(const EventMonitor* In_event_monitor);

	/**
	 * @brief Start event monitor
	 */
	virtual void Start() = 0;

	/**
	 * @brief Stop event monitor
	 */
	virtual void Stop() = 0;

	/**
	 * @brief Set the callback function when event triggered
	 * @param In_callback Callback function
	 */
	virtual void SetEventTriggeredCallback(const std::function<void(const EventInfo& In_event_info)>& In_callback) = 0;

protected:
	virtual ~EventMonitor() = default;
};