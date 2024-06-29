#pragma once
#include <functional>
#include "EventInfo.h"

// Enum defining types of event monitors
enum class EventMonitorType {
    krabsetw,  // Type of event monitor using Krabsetw library
};

// Abstract class representing an Event Monitor
class EventMonitor {
public:
    /**
     * @brief Factory method to create an event monitor instance based on type.
     * @param In_type Type of event monitor to create.
     * @return Pointer to the created EventMonitor instance.
     */
    static EventMonitor* Create(const EventMonitorType& In_type);

    /**
     * @brief Destructor to destroy the event monitor instance.
     * @param In_event_monitor Pointer to the EventMonitor instance to destroy.
     */
    static void Destroy(const EventMonitor* In_event_monitor);

    /**
     * @brief Start the event monitoring process.
     */
    virtual void Start() = 0;

    /**
     * @brief Stop the event monitoring process.
     */
    virtual void Stop() = 0;

    /**
     * @brief Set a callback function to be called when a specific event (such as ProcessStart) is triggered.
     * @param In_callback Callback function to handle the event.
     */
    virtual void SetProcessStartEventTriggeredCallback(const std::function<void(const EventInfo&)>& In_callback) = 0;

protected:
    // Protected destructor to ensure the class is abstract.
    virtual ~EventMonitor() = default;
};
