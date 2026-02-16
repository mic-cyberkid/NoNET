#include "firewall.h"
#include <iostream>
#include <comdef.h>

#ifdef _WIN32
#include <oleauto.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")
#endif

Firewall::Firewall() {
#ifdef _WIN32
    initializeCOM();
#endif
}

Firewall::~Firewall() {
#ifdef _WIN32
    uninitializeCOM();
#endif
}

#ifdef _WIN32
HRESULT Firewall::initializeCOM() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) return hr;
    m_comInitialized = true;

    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&m_pNetFwPolicy2);
    return hr;
}

void Firewall::uninitializeCOM() {
    if (m_pNetFwPolicy2) {
        m_pNetFwPolicy2->Release();
        m_pNetFwPolicy2 = nullptr;
    }
    if (m_comInitialized) {
        CoUninitialize();
        m_comInitialized = false;
    }
}

INetFwPolicy2* Firewall::getPolicy() {
    return m_pNetFwPolicy2;
}
#endif

std::error_code Firewall::setDefaultOutboundBlock() {
#ifdef _WIN32
    if (!m_pNetFwPolicy2) return std::make_error_code(std::errc::not_connected);

    long currentProfileTypes = 0;
    HRESULT hr = m_pNetFwPolicy2->get_CurrentProfileTypes(&currentProfileTypes);
    if (FAILED(hr)) return std::make_error_code(std::errc::io_error);

    auto setBlock = [&](long profile) {
        if (currentProfileTypes & profile) {
            m_pNetFwPolicy2->put_DefaultOutboundAction(profile, NET_FW_ACTION_BLOCK);
        }
    };

    setBlock(NET_FW_PROFILE2_DOMAIN);
    setBlock(NET_FW_PROFILE2_PRIVATE);
    setBlock(NET_FW_PROFILE2_PUBLIC);

    return std::error_code();
#else
    return std::make_error_code(std::errc::not_supported);
#endif
}

std::error_code Firewall::addAllowRule(const std::wstring& appPath, const std::wstring& ruleName) {
#ifdef _WIN32
    if (!m_pNetFwPolicy2) return std::make_error_code(std::errc::not_connected);

    INetFwRules* pFwRules = NULL;
    INetFwRule* pFwRule = NULL;
    HRESULT hr = m_pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) return std::make_error_code(std::errc::io_error);

    hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
    if (FAILED(hr)) {
        pFwRules->Release();
        return std::make_error_code(std::errc::io_error);
    }

    BSTR bName = SysAllocString(ruleName.c_str());
    BSTR bPath = SysAllocString(appPath.c_str());
    pFwRule->put_Name(bName);
    pFwRule->put_ApplicationName(bPath);
    pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    pFwRule->put_Enabled(VARIANT_TRUE);
    pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_ANY);

    long currentProfileTypes = 0;
    m_pNetFwPolicy2->get_CurrentProfileTypes(&currentProfileTypes);
    pFwRule->put_Profiles(currentProfileTypes);

    hr = pFwRules->Add(pFwRule);

    if (bName) SysFreeString(bName);
    if (bPath) SysFreeString(bPath);
    pFwRule->Release();
    pFwRules->Release();

    return FAILED(hr) ? std::make_error_code(std::errc::io_error) : std::error_code();
#else
    return std::make_error_code(std::errc::not_supported);
#endif
}

std::error_code Firewall::removeRule(const std::wstring& ruleName) {
#ifdef _WIN32
    if (!m_pNetFwPolicy2) return std::make_error_code(std::errc::not_connected);

    INetFwRules* pFwRules = NULL;
    HRESULT hr = m_pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) return std::make_error_code(std::errc::io_error);

    BSTR bName = SysAllocString(ruleName.c_str());
    hr = pFwRules->Remove(bName);
    if (bName) SysFreeString(bName);
    pFwRules->Release();

    return FAILED(hr) ? std::make_error_code(std::errc::io_error) : std::error_code();
#else
    return std::make_error_code(std::errc::not_supported);
#endif
}

std::vector<FirewallRule> Firewall::listRules() {
    std::vector<FirewallRule> rules;
#ifdef _WIN32
    if (!m_pNetFwPolicy2) return rules;

    INetFwRules* pFwRules = NULL;
    HRESULT hr = m_pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) return rules;

    IUnknown* pEnumerator = NULL;
    hr = pFwRules->get__NewEnum(&pEnumerator);
    if (FAILED(hr)) {
        pFwRules->Release();
        return rules;
    }

    IEnumVARIANT* pVariant = NULL;
    hr = pEnumerator->QueryInterface(__uuidof(IEnumVARIANT), (void**)&pVariant);
    pEnumerator->Release();
    if (FAILED(hr)) {
        pFwRules->Release();
        return rules;
    }

    VARIANT var;
    VariantInit(&var);
    while (pVariant->Next(1, &var, NULL) == S_OK) {
        INetFwRule* pFwRule = NULL;
        if (SUCCEEDED(var.punkVal->QueryInterface(__uuidof(INetFwRule), (void**)&pFwRule))) {
            BSTR bstrName, bstrAppPath;
            VARIANT_BOOL enabled;
            pFwRule->get_Name(&bstrName);
            pFwRule->get_ApplicationName(&bstrAppPath);
            pFwRule->get_Enabled(&enabled);

            rules.push_back({
                bstrName ? bstrName : L"",
                L"",
                bstrAppPath ? bstrAppPath : L"",
                enabled == VARIANT_TRUE
            });

            if (bstrName) SysFreeString(bstrName);
            if (bstrAppPath) SysFreeString(bstrAppPath);
            pFwRule->Release();
        }
        VariantClear(&var);
    }

    pVariant->Release();
    pFwRules->Release();
#endif
    return rules;
}
