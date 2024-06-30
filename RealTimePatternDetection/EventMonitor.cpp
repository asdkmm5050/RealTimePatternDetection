#include "EventMonitor.h"
#include "KrabsetwEventMonitor.h"

EventMonitor* EventMonitor::Create(const EventMonitorType& In_type) {
	switch (In_type) {
		case EventMonitorType::krabsetw: return new KrabsetwEventMonitor(std::make_shared<KrabsetwUserTraceWrapper>(L"KrabsetwEventMonitor"),
																		 std::make_shared<KrabsetwParserWrapper>());
	}
	return nullptr;
}

void EventMonitor::Destroy(const EventMonitor* In_event_monitor) {
	delete In_event_monitor;
}
