#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <map>

struct AppUsage {
    std::wstring appName;
    uint64_t bytesSent;
    uint64_t bytesReceived;
};

class NetworkMonitor {
public:
    NetworkMonitor();
    ~NetworkMonitor();

    std::vector<AppUsage> getAppUsage();

private:
#ifdef _WIN32
    // Implementation details for Windows IP Helper API
#endif
};
