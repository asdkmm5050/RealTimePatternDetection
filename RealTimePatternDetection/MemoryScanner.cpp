#include "MemoryScanner.h"
#include "WindowsMemoryScanner.h"
MemoryScanner* MemoryScanner::Create(const MemoryScannerType& In_type) {
	switch (In_type) {
		case windows: return new WindowsMemoryScanner();
	}
	return nullptr;
}

void MemoryScanner::Destroy(const MemoryScanner* In_memory_scanner) {
	delete In_memory_scanner;
}
