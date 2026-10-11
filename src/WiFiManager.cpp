
#include "WiFiManager.h"
#include "Logger.h"

#include <windows.h>
#include <wlanapi.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

using namespace std;

namespace
{
    // Convert a UTF-8 string to Windows UTF-16.
    wstring toWide(const string& text)
    {
        if (text.empty())
            return L"";

        int length = MultiByteToWideChar(
            CP_UTF8, MB_ERR_INVALID_CHARS,
            text.c_str(), -1, nullptr, 0
        );

        if (length == 0)
            return L"";

        vector<wchar_t> buffer(length);

        if (MultiByteToWideChar(
                CP_UTF8, MB_ERR_INVALID_CHARS,
                text.c_str(), -1,
                buffer.data(), length
            ) == 0)
            return L"";

        return wstring(buffer.data());
    }

    // Convert Windows UTF-16 to UTF-8.
    string toUtf8(const wstring& text)
    {
        if (text.empty())
            return "";

        int length = WideCharToMultiByte(
            CP_UTF8, 0, text.c_str(), -1,
            nullptr, 0, nullptr, nullptr
        );

        if (length == 0)
            return "";

        vector<char> buffer(length);

        if (WideCharToMultiByte(
                CP_UTF8, 0, text.c_str(), -1,
                buffer.data(), length,
                nullptr, nullptr
            ) == 0)
            return "";

        return string(buffer.data());
    }

    // Read the connected profile name from Windows.
    string getConnectedProfile()
    {
        HANDLE client = nullptr;
        DWORD version = 0;

        if (WlanOpenHandle(
                2, nullptr, &version, &client
            ) != ERROR_SUCCESS)
            return "";

        PWLAN_INTERFACE_INFO_LIST interfaces = nullptr;
        string result;

        if (WlanEnumInterfaces(
                client, nullptr, &interfaces
            ) == ERROR_SUCCESS)
        {
            for (DWORD i = 0;
                 i < interfaces->dwNumberOfItems;
                 ++i)
            {
                const auto& interfaceInfo =
                    interfaces->InterfaceInfo[i];

                if (interfaceInfo.isState !=
                    wlan_interface_state_connected)
                    continue;

                DWORD dataSize = 0;
                PVOID data = nullptr;
                WLAN_OPCODE_VALUE_TYPE opcode;

                DWORD status = WlanQueryInterface(
                    client,
                    &interfaceInfo.InterfaceGuid,
                    wlan_intf_opcode_current_connection,
                    nullptr,
                    &dataSize,
                    &data,
                    &opcode
                );

                if (status == ERROR_SUCCESS && data)
                {
                    auto* connection =
                        static_cast<
                            WLAN_CONNECTION_ATTRIBUTES*
                        >(data);

                    result = toUtf8(
                        connection->strProfileName
                    );

                    WlanFreeMemory(data);

                    if (!result.empty())
                        break;
                }
                else if (data)
                {
                    WlanFreeMemory(data);
                }
            }
        }

        if (interfaces)
            WlanFreeMemory(interfaces);

        WlanCloseHandle(client, nullptr);
        return result;
    }
}

string getCurrentWiFi()
{
    return getConnectedProfile();
}

bool connectToWiFi(const string& profileName)
{
    wstring target = toWide(profileName);

    if (target.empty())
    {
        logMessage("Invalid Wi-Fi profile name.");
        return false;
    }

    // Check whether we are already connected.
    if (getConnectedProfile() == profileName)
    {
        logMessage("Already connected to: " + profileName);
        return true;
    }

    HANDLE client = nullptr;
    DWORD version = 0;

    if (WlanOpenHandle(
            2, nullptr, &version, &client
        ) != ERROR_SUCCESS)
    {
        logMessage("Unable to open Windows Wi-Fi API.");
        return false;
    }

    PWLAN_INTERFACE_INFO_LIST interfaces = nullptr;
    bool requestAccepted = false;

    if (WlanEnumInterfaces(
            client, nullptr, &interfaces
        ) == ERROR_SUCCESS)
    {
        for (DWORD i = 0;
             i < interfaces->dwNumberOfItems;
             ++i)
        {
            const auto& interfaceInfo =
                interfaces->InterfaceInfo[i];

            WLAN_CONNECTION_PARAMETERS parameters{};
            parameters.wlanConnectionMode =
                wlan_connection_mode_profile;
            parameters.strProfile = target.c_str();
            parameters.dot11BssType = dot11_BSS_type_any;

            DWORD status = WlanConnect(
                client,
                &interfaceInfo.InterfaceGuid,
                &parameters,
                nullptr
            );

            if (status == ERROR_SUCCESS)
            {
                requestAccepted = true;
                break;
            }
        }
    }

    if (interfaces)
        WlanFreeMemory(interfaces);

    WlanCloseHandle(client, nullptr);

    if (!requestAccepted)
    {
        logMessage(
            "Wi-Fi connection request failed: " +
            profileName
        );
        return false;
    }

    // Wait for Windows to complete the connection.
    for (int attempt = 0; attempt < 10; ++attempt)
    {
        this_thread::sleep_for(
            chrono::seconds(1)
        );

        if (getConnectedProfile() == profileName)
        {
            logMessage(
                "Connected to Wi-Fi profile: " +
                profileName
            );
            return true;
        }
    }

    logMessage(
        "Wi-Fi connection timed out: " +
        profileName
    );

    return false;
}
