#pragma once

#include <string>
#include <vector>
#include <system_error>

#ifdef _WIN32
#include <windows.h>
#include <netfw.h>
#endif

struct FirewallRule {
    std::wstring name;
    std::wstring description;
    std::wstring applicationPath;
    bool enabled;
};

class Firewall {
public:
    Firewall();
    ~Firewall();

    // Prevent copying
    Firewall(const Firewall&) = delete;
    Firewall& operator=(const Firewall&) = delete;

    std::error_code setDefaultOutboundBlock();
    std::error_code addAllowRule(const std::wstring& appPath, const std::wstring& ruleName);
    std::error_code removeRule(const std::wstring& ruleName);
    std::vector<FirewallRule> listRules();

private:
#ifdef _WIN32
    HRESULT initializeCOM();
    void uninitializeCOM();
    INetFwPolicy2* getPolicy();

    bool m_comInitialized = false;
    INetFwPolicy2* m_pNetFwPolicy2 = nullptr;
#endif
};
