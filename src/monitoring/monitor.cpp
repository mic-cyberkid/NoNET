#include "monitor.h"

#ifdef _WIN32
#include <windows.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

NetworkMonitor::NetworkMonitor() {}

NetworkMonitor::~NetworkMonitor() {}

std::vector<AppUsage> NetworkMonitor::getAppUsage() {
    std::vector<AppUsage> usage;
#ifdef _WIN32
    // Placeholder for Windows IP Helper API calls
    // e.g., GetExtendedTcpTable, GetExtendedUdpTable
#endif
    return usage;
}
