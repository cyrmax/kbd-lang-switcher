#include "windows-keyboard-api.hpp"

#include <Windows.h>
// Separator to prevent include sorting
#include "defer.hpp"
#include "log-macro.hpp"

#include <WinUser.h>
#include <optional>
#include <shlwapi.h>
#include <vector>
#pragma comment(lib, "shlwapi.lib")

std::optional<std::vector<KeyboardLayoutInfo>> KeyboardManager::getKeyboardLayouts() const {
    LOG_TRACE_CALLED();
    std::vector<HKL> layoutsBuffer;
    layoutsBuffer.resize(10);
    int result = GetKeyboardLayoutList(10, layoutsBuffer.data());
    if (result == 0) {
        LOG_DEBUG("WinAPI GetKeyboardLayouts returned 0 result");
        return std::nullopt;
    }
    LOG_DEBUG("WinAPI GetKeyboardLayouts returned result of {}", result);
    std::vector<KeyboardLayoutInfo> layouts;
    layouts.reserve(result);
    for (size_t i = 0; i < 10 && i < result; ++i) {
        auto langName = getLanguageName(layoutsBuffer[i]).value_or(L"Default language");
        auto layoutName = getKeyboardLayoutName(layoutsBuffer[i]).value_or(L"Default layout");
        layouts.emplace_back(KeyboardLayoutInfo(langName, layoutName, layoutsBuffer[i]));
        LOG_DEBUG(L"Adding new layout with language name {} and layout name {}", langName, layoutName);
    }
    return layouts;
}

std::optional<std::wstring> KeyboardManager::getLanguageName(HKL hkl) const {
    LOG_TRACE_CALLED();
    LCID lcid = LOWORD(HandleToULong(hkl));
    wchar_t localeName[LOCALE_NAME_MAX_LENGTH] = {0};
    if (LCIDToLocaleName(lcid, localeName, LOCALE_NAME_MAX_LENGTH, 0) == 0) {
        LOG_WARN(L"Failed to get locale name for LCID {}", lcid);
        return std::nullopt;
    }
    wchar_t langName[256] = {0};
    if (GetLocaleInfoEx(localeName, LOCALE_SNAME, langName, 256) > 0) {
        return langName;
    }
    LOG_WARN(L"Failed to get locale info from WinAPI for localeName {}", localeName);
    return std::nullopt;
}

std::optional<std::wstring> KeyboardManager::getKeyboardLayoutName(HKL hkl) const {
    LOG_TRACE_CALLED();
    wchar_t klid[KL_NAMELENGTH];
    swprintf_s(klid, L"%08X", HandleToULong(hkl));
    std::wstring regPath = L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\" + std::wstring(klid);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, regPath.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS) {
        return std::nullopt;
    }
    defer(RegCloseKey(hKey));
    wchar_t layoutDisplayName[256] = {0};
    DWORD dataSize = sizeof(layoutDisplayName);
    if (RegGetValueW(hKey, L"", L"Layout Display Name", RRF_RT_REG_SZ, nullptr, layoutDisplayName, &dataSize) ==
        ERROR_SUCCESS) {
        wchar_t localizedName[256] = {0};
        if (SHLoadIndirectString(layoutDisplayName, localizedName, 256, nullptr) == S_OK) {
            return localizedName;
        }
    }

    // Fallback to Layout Text
    dataSize = sizeof(layoutDisplayName);
    if (RegGetValueW(hKey, L"", L"Layout Text", RRF_RT_REG_SZ, nullptr, layoutDisplayName, &dataSize) ==
        ERROR_SUCCESS) {
        return layoutDisplayName;
    }
    return std::nullopt;
}
