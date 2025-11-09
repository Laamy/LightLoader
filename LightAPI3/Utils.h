#pragma once

#include <windows.h>
#include <string>

// cuz someone told me they rather widestring
// dont remember who i stole these from though
static std::string wstring_to_utf8(const std::wstring& wstr)
{
    if (wstr.empty()) return {};

    int bytes = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        static_cast<int>(wstr.size()), nullptr, 0,
        nullptr, nullptr);
    std::string result(bytes, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
        static_cast<int>(wstr.size()), &result[0],
        bytes, nullptr, nullptr);
    return result;
}

static std::wstring utf8_to_wstring(const std::string& utf8)
{
    if (utf8.empty())
        return {};

    int wlen = MultiByteToWideChar(CP_UTF8, 0,
        utf8.data(), static_cast<int>(utf8.size()),
        nullptr, 0);
    if (wlen == 0)
        throw std::runtime_error("MultiByteToWideChar failed");

    std::wstring wstr(wlen, L'\0');
    int ret = MultiByteToWideChar(CP_UTF8, 0,
        utf8.data(), static_cast<int>(utf8.size()),
        &wstr[0], wlen);
    if (ret == 0)
        throw std::runtime_error("MultiByteToWideChar failed");

    if (!wstr.empty() && wstr.back() == L'\0')
        wstr.pop_back();

    return wstr;
}
