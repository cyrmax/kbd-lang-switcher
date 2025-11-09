#include "SettingsStorage.hpp"

#include "log-macro.hpp"

#include <winlamb/file.h>
#include <winlamb/file_ini.h>
#include <winlamb/path.h>
#include <winlamb/syspath.h>

void SettingsStorage::load() {
    const auto settingsPath = _getSettingsPath();

    if (!wl::file::util::exists(settingsPath)) {
        _createDefaultFile();
        return;
    }

    try {
        wl::file_ini ini;
        ini.load_from_file(settingsPath);

        if (const auto* generalSection = ini.sections.get_if_exists(L"General")) {
            if (const auto* beepVal = generalSection->get_if_exists(L"BeepOnSwitch")) {
                this->beepOnSwitch = (*beepVal == L"true");
            }
        }

        ignoredLanguages.clear();
        if (const auto* ignoredLanguagesSection = ini.sections.get_if_exists(L"IgnoredLanguages")) {
            ignoredLanguages.reserve(ignoredLanguagesSection->size());
            for (const auto& entry : *ignoredLanguagesSection) {
                ignoredLanguages.emplace_back(entry.key);
            }
        }
    } catch (const std::exception& e) {
        LOG_ERROR("Error loading settings file: {}", e.what());
        _createDefaultFile();
    }
    m_isLoaded = true;
}

void SettingsStorage::ensureLoaded() {
    if (m_isLoaded) {
        return;
    }
    load();
}

void SettingsStorage::save() const {
    wl::file_ini ini;

    ini.sections[L"General"][L"BeepOnSwitch"] = (beepOnSwitch ? L"true" : L"false");

    auto& ignoredLanguagesSection = ini.sections[L"IgnoredLanguages"];
    for (const auto& lang : ignoredLanguages) {
        ignoredLanguagesSection[lang] = L"1";
    }

    const auto settingsPath = _getSettingsPath();
    const auto folderPath = wl::path::folder_from(settingsPath);

    if (!wl::file::util::exists(folderPath)) {
        wl::file::util::create_dir(folderPath);
    }

    ini.save_to_file(settingsPath);
}

std::wstring SettingsStorage::_getSettingsPath() const {
    return wl::syspath::app_data() + L"\\kbd-lang-switcher\\kbd-lang-switcher.ini";
}

void SettingsStorage::_createDefaultFile() {
    beepOnSwitch = true;
    ignoredLanguages.clear();
    save();
}
