#pragma once

#include "SettingsStorage.hpp"
#include "windows-keyboard-api.hpp"

#include <shellapi.h>
//
#include <winlamb/button.h>
#include <winlamb/checkbox.h>
#include <winlamb/dialog_modal.h>
#include <winlamb/listview.h>

class SettingsDialog final : public wl::dialog_modal {
  public:
    SettingsDialog(SettingsStorage&, KeyboardManager&);

  private:
    SettingsStorage& m_settings;
    KeyboardManager& m_keyboardManager;

    wl::listview m_lvLanguages;
    wl::checkbox m_chkBeep;
    wl::button m_btnOk;
    wl::button m_btnCancel;

    void _loadSettingsAndPopulateList();
    void _saveSettings();
};
