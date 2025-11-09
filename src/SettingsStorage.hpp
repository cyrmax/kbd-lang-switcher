#pragma once

#include <string>
#include <vector>

class SettingsStorage {
  public:
    bool beepOnSwitch = true;
    std::vector<std::wstring> ignoredLanguages;

    SettingsStorage() = default;

    void load();
    void ensureLoaded();
    void save() const;

  private:
    bool m_isLoaded = false;
    [[nodiscard]] std::wstring _getSettingsPath() const;
    void _createDefaultFile();
};
