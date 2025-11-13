#include "SettingsDialog.hpp"

#include "../assets/resources.h"
#include "log-macro.hpp"

#include <algorithm>

SettingsDialog::SettingsDialog(SettingsStorage& settingsStorage, KeyboardManager& keyboardManager)
    : m_settings(settingsStorage), m_keyboardManager(keyboardManager) {
    LOG_TRACE_CALLED();
    setup.dialogId = IDD_SETTINGS;

    on_message(WM_INITDIALOG, [this](wl::params) -> LRESULT {
        LOG_TRACE("WM_INITDIALOG for SettingsDialog");

        m_lvLanguages.assign(this, IDC_LANGUAGES_LIST);
        m_chkBeep.assign(this, IDC_BEEP_CHECK);
        m_btnOk.assign(this, IDOK);
        m_btnCancel.assign(this, IDCANCEL);

        m_lvLanguages.style.full_row_select(true);
        m_lvLanguages.style.checkboxes(true);
        m_lvLanguages.columns.add(L"Available Keyboard Languages", 220);

        _loadSettingsAndPopulateList();
        center_on_parent();
        return TRUE;
    });

    on_command(IDOK, [this](wl::params params) -> INT_PTR {
        LOG_DEBUG("OK button event fired via on_command");
        _saveSettings();
        m_settings.save();
        EndDialog(hwnd(), IDOK);
        return TRUE;
    });

    on_command(IDCANCEL, [this](wl::params params) -> INT_PTR {
        LOG_DEBUG("Cancel button event fired via on_command");
        EndDialog(hwnd(), IDCANCEL);
        return TRUE;
    });
}

void SettingsDialog::_loadSettingsAndPopulateList() {
    LOG_TRACE_CALLED();
    m_chkBeep.set_check(m_settings.beepOnSwitch);

    auto layouts = m_keyboardManager.getKeyboardLayouts().value_or({});
    if (m_settings.ignoredLanguages.empty()) {
        for (const auto& layout : layouts) {
            m_lvLanguages.items.add(layout.languageName);
        }
    } else {
        for (const auto& lang : m_settings.ignoredLanguages) {
            auto item = m_lvLanguages.items.add(lang);
            ListView_SetCheckState(m_lvLanguages.hwnd(), item.index, TRUE);
        }
        for (const auto& layout : layouts) {
            if (std::find(m_settings.ignoredLanguages.begin(), m_settings.ignoredLanguages.end(),
                          layout.languageName) == m_settings.ignoredLanguages.end()) {
                m_lvLanguages.items.add(layout.languageName);
            }
        }
    }
}

void SettingsDialog::_saveSettings() {
    LOG_TRACE_CALLED();
    m_settings.beepOnSwitch = m_chkBeep.is_checked();

    m_settings.ignoredLanguages.clear();
    auto numOfItems = m_lvLanguages.items.count();
    for (size_t i = 0; i < numOfItems; ++i) {
        if (ListView_GetCheckState(m_lvLanguages.hwnd(), i) == TRUE) {
            m_settings.ignoredLanguages.push_back(m_lvLanguages.items[i].get_text());
        }
    }
}
