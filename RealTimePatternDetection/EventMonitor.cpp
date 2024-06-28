#include "EventMonitor.h"
#include "KrabsetwEventMonitor.h"

EventMonitor* EventMonitor::Create(const EventMonitorType& In_type) {
	switch (In_type) {
		case krabsetw: return new KrabsetwEventMonitor();
	}
	return nullptr;
}

void EventMonitor::Destroy(const EventMonitor* In_event_monitor) {
	delete In_event_monitor;
}
