#include "pch.hpp"
#include "Utils.hpp"
#include <Windows.h>
#include <vector>

#define WIDEIMPL(x) L##x
#define WIDE(x) WIDEIMPL(x)
#define APP_NAME_WCHAR WIDE(APP_NAME)

void SetIconOnWindow(GLFWwindow* window)
{
    HICON icon = static_cast<HICON>(LoadImageW(
        GetModuleHandle(NULL),
        L"IDI_ICON1",
        IMAGE_ICON,
        512,
        512,
        LR_DEFAULTCOLOR
    ));
    ICONINFO iconInfo{};
    GetIconInfo(icon, &iconInfo);

    BITMAP bitmap{};
    GetObject(iconInfo.hbmColor, sizeof(bitmap), &bitmap);

    const int icwidth = bitmap.bmWidth;
    const int icheight = bitmap.bmHeight;

    GLFWimage image{};
    std::vector<unsigned char> pixels;
    pixels.resize(icwidth * icheight * 4);

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = icwidth;
    bmi.bmiHeader.biHeight = -icheight; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdc = GetDC(nullptr);

    GetDIBits(
        hdc,
        iconInfo.hbmColor,
        0,
        icheight,
        pixels.data(),
        &bmi,
        DIB_RGB_COLORS
    );

    ReleaseDC(nullptr, hdc); 

    // Windows gives us BGRA, GLFW wants RGBA.
    for (size_t i = 0; i < pixels.size(); i += 4)
        std::swap(pixels[i], pixels[i + 2]);

    image.width = icwidth;
    image.height = icheight;
    image.pixels = pixels.data();

    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);

    glfwSetWindowIcon(window, 1, &image);
    DestroyIcon(icon);
}

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
        std::filesystem::path exePath(buffer, buffer + length);

        //set the value
        result = RegSetValueExW(
            key,
            APP_NAME_WCHAR,
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(exePath.c_str()),
            static_cast<DWORD>((exePath.wstring().size() + 1) * sizeof(wchar_t))
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

    wchar_t buffer[MAX_PATH];
    DWORD length = sizeof(buffer);
    LONG result = RegQueryValueExW(
        key,
        APP_NAME_WCHAR,
        nullptr,
        nullptr,
        reinterpret_cast<LPBYTE>(buffer),
        &length
    );
    RegCloseKey(key);

    if(result != ERROR_SUCCESS) return false;

    length = (length/sizeof(wchar_t)-1); //it was in byte so x2
    std::filesystem::path regPath(buffer, buffer + length);

    length = GetModuleFileNameW(
        nullptr,
        buffer,
        MAX_PATH
    );
    std::filesystem::path exePath(buffer, buffer + length);

    return regPath == exePath;
}