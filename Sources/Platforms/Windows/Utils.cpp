#include "pch.hpp"
#include "Utils.hpp"
#include <Windows.h>

#define WIDEIMPL(x) L##x
#define WIDE(x) WIDEIMPL(x)
#define APP_NAME_WCHAR WIDE(APP_NAME)

bool ToggleAutoStart(bool state)
{
    HKEY key;

    if (RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_SET_VALUE,
        &key) != ERROR_SUCCESS)
    {
        return false;
    }

    LONG result;
    if(state)
    {
        //Get the exe path
        wchar_t buffer[MAX_PATH];
        DWORD length = GetModuleFileNameW(
            nullptr,
            buffer,
            MAX_PATH
        );
        std::wstring exePath(buffer, length);

        //set the value
        result = RegSetValueExW(
            key,
            APP_NAME_WCHAR,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(exePath.c_str()),
            static_cast<DWORD>((exePath.size() + 1) * sizeof(wchar_t))
        );
    }
    else
    {
        result = RegDeleteValueW(key, APP_NAME_WCHAR);
    }

    RegCloseKey(key);
    return result == ERROR_SUCCESS;
}

bool IsAutoStartEnable()
{
    HKEY key;

    if (RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
        0,
        KEY_QUERY_VALUE,
        &key) != ERROR_SUCCESS)
    {
        return false;
    }

    LONG result = RegQueryValueExW(
        key,
        APP_NAME_WCHAR,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    );

    RegCloseKey(key);

    return result == ERROR_SUCCESS;
}