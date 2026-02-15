#include <windows.h>
#include <netfw.h>
#include <objbase.h>
#include <oleauto.h>
#include <stdio.h>
#include <comdef.h>
#include <vector>
#include <string>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

HRESULT SetDefaultOutboundBlock(INetFwPolicy2* pNetFwPolicy2) {
    long currentProfileTypes = 0;
    HRESULT hr = pNetFwPolicy2->get_CurrentProfileTypes(&currentProfileTypes);
    if (FAILED(hr)) {
        return hr;
    }

    // Set for each active profile
    if (currentProfileTypes & NET_FW_PROFILE2_DOMAIN) {
        hr = pNetFwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_DOMAIN, NET_FW_ACTION_BLOCK);
        if (FAILED(hr)) return hr;
    }
    if (currentProfileTypes & NET_FW_PROFILE2_PRIVATE) {
        hr = pNetFwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PRIVATE, NET_FW_ACTION_BLOCK);
        if (FAILED(hr)) return hr;
    }
    if (currentProfileTypes & NET_FW_PROFILE2_PUBLIC) {
        hr = pNetFwPolicy2->put_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, NET_FW_ACTION_BLOCK);
        if (FAILED(hr)) return hr;
    }

    return S_OK;
}

HRESULT AddAllowRule(INetFwPolicy2* pNetFwPolicy2, const wchar_t* appPath, const wchar_t* ruleName) {
    INetFwRules* pFwRules = NULL;
    INetFwRule* pFwRule = NULL;
    HRESULT hr;

    hr = pNetFwPolicy2->get_Rules(&pFwRules);
    if (FAILED(hr)) goto Cleanup;

    hr = CoCreateInstance(__uuidof(NetFwRule), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwRule), (void**)&pFwRule);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Name(ruleName);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Description(L"Allow outbound access for specific application");
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_ApplicationName(appPath);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Direction(NET_FW_RULE_DIR_OUT);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Action(NET_FW_ACTION_ALLOW);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Enabled(VARIANT_TRUE);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Protocol(NET_FW_IP_PROTOCOL_ANY);
    if (FAILED(hr)) goto Cleanup;

    long currentProfileTypes = 0;
    hr = pNetFwPolicy2->get_CurrentProfileTypes(&currentProfileTypes);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRule->put_Profiles(currentProfileTypes);
    if (FAILED(hr)) goto Cleanup;

    hr = pFwRules->Add(pFwRule);
    if (FAILED(hr)) goto Cleanup;

Cleanup:
    if (pFwRule != NULL) pFwRule->Release();
    if (pFwRules != NULL) pFwRules->Release();
    return hr;
}

int wmain(int argc, wchar_t* argv[]) {
    if (argc < 2) {
        wprintf(L"Usage: %s <app_path1> [app_path2] ...\n", argv[0]);
        wprintf(L"Example: %s \"C:\\Program Files\\Mozilla Firefox\\firefox.exe\"\n", argv[0]);
        return 1;
    }

    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        wprintf(L"CoInitialize failed: %x\n", hr);
        return 1;
    }

    INetFwPolicy2* pNetFwPolicy2 = NULL;
    hr = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER, __uuidof(INetFwPolicy2), (void**)&pNetFwPolicy2);
    if (FAILED(hr)) {
        wprintf(L"CoCreateInstance failed: %x\n", hr);
        goto Cleanup;
    }

    // Set default outbound to block
    hr = SetDefaultOutboundBlock(pNetFwPolicy2);
    if (FAILED(hr)) {
        wprintf(L"Failed to set default outbound block: %x\n", hr);
        goto Cleanup;
    }

    // Add allow rules for each provided app path
    for (int i = 1; i < argc; ++i) {
        std::wstring ruleName = L"Allow_" + std::wstring(argv[i]);
        hr = AddAllowRule(pNetFwPolicy2, argv[i], ruleName.c_str());
        if (FAILED(hr)) {
            wprintf(L"Failed to add rule for %s: %x\n", argv[i], hr);
        } else {
            wprintf(L"Successfully added rule for %s\n", argv[i]);
        }
    }

    wprintf(L"Default outbound connections are now blocked except for specified apps.\n");
    wprintf(L"WARNING: This may break internet access for other applications. Run as administrator.\n");

Cleanup:
    if (pNetFwPolicy2 != NULL) pNetFwPolicy2->Release();
    CoUninitialize();
    return FAILED(hr) ? 1 : 0;
}
