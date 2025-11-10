#include "SettingsStorage.hpp"

#include "log-macro.hpp"

#include <SimpleIni.h>
#include <filesystem>
#include <winlamb/syspath.h>

namespace fs = std::filesystem;

void SettingsStorage::load() {
    LOG_TRACE_CALLED();
    const auto settingsPath = _getSettingsPath();

    if (!fs::exists(settingsPath)) {
        _createDefaultFile();
        return;
    }

    try {
        CSimpleIniW ini;
        ini.LoadFile(settingsPath.c_str());

        const auto beepOnSwitchVal = ini.GetValue(L"General", L"BeepOnSwitch", L"true");
        this->beepOnSwitch = (beepOnSwitchVal == L"true");

        this->ignoredLanguages.clear();
        auto numOfLanguages = ini.GetSectionSize(L"IgnoredLanguages");
        if (numOfLanguages > 0) {
            this->ignoredLanguages.reserve(numOfLanguages);
            auto languages = ini.GetSection(L"IgnoredLanguages");
            for (auto it = languages->begin(); it != languages->end(); ++it) {
                this->ignoredLanguages.push_back(it->first.pItem);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading settings file: {}", e.what());
        _createDefaultFile();
    }
    m_isLoaded = true;
}

void SettingsStorage::ensureLoaded() {
    LOG_TRACE_CALLED();
    if (m_isLoaded) {
        return;
    }
    load();
}

void SettingsStorage::save() const {
    try {
        LOG_TRACE_CALLED();
        CSimpleIniW ini;

        ini.SetValue(L"General", L"BeepOnSwitch", this->beepOnSwitch ? L"true" : L"false");

        for (const auto& lang : ignoredLanguages) {
            ini.SetValue(L"IgnoredLanguages", lang.c_str(), L"1");
        }

        const auto settingsPath = _getSettingsPath();
        const auto folderPath = fs::path(settingsPath).parent_path().wstring();
        LOG_DEBUG(L"Settings folder path is {}", folderPath);

        if (!fs::exists(folderPath)) {
            LOG_DEBUG(L"About to create settings folder");
            fs::create_directories(folderPath);
        }

        LOG_DEBUG(L"Saving settings to ini file");
        ini.SaveFile(settingsPath.c_str());
    } catch (const std::exception& e) { LOG_ERROR("Failed to save settings: {}", e.what()); }
}

std::wstring SettingsStorage::_getSettingsPath() const {
    LOG_TRACE_CALLED();
    auto appDataPath = std::filesystem::path(wl::syspath::app_data());
    LOG_DEBUG(L"App Data Path is {}", appDataPath.wstring());
    auto fullPath = appDataPath / L"kbd-lang-switcher" / L"kbd-lang-switcher.ini";
    LOG_DEBUG(L"The full path constructed {}", fullPath.wstring());
    return fullPath.wstring();
}

void SettingsStorage::_createDefaultFile() {
    LOG_TRACE_CALLED();
    beepOnSwitch = true;
    ignoredLanguages.clear();
    save();
}
