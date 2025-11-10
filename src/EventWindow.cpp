#include "EventWindow.hpp"

#include "../assets/resources.h"
#include "SettingsDialog.hpp"
#include "log-macro.hpp"
#include "windows-keyboard-api.hpp"

#define WM_SHOWSETTINGS (WM_APP + 1)

EventWindow::EventWindow(KeyboardManager& keyboardManager, SettingsStorage& settingsStorage)
    : m_keyboardManager(keyboardManager), m_settings(settingsStorage) {
    LOG_TRACE_CALLED();
    setup.wndClassEx.lpszClassName = L"KBD_LANG_SWITCHER_EVT_WND";
    setup.title = L"KBD_LANG_SWITCHER_EVT_WND";
    setup.style = 0;
    setup.exStyle = WS_EX_TOOLWINDOW;

    on_message(WM_CREATE, [&](wl::params params) -> LRESULT {
        LOG_TRACE_CALLED();

        HICON hIcon = LoadIconW(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_APPICON));
        if (!hIcon) {
            LOG_ERROR(L"Failed to load icon resource with ID: {}", IDI_APPICON);
        }

        m_settings.ensureLoaded();

        auto show_settings = [&]() { PostMessage(hwnd(), WM_SHOWSETTINGS, 0, 0); };

        m_tray = std::make_unique<Tray::Tray>("Keyboard-Language-Switcher", hIcon);
        m_tray->addEntry(Tray::Button{"Settings...", show_settings});
        m_tray->addEntry(Tray::Separator{});
        m_tray->addEntry(Tray::Button{"Quit", [&]() { PostQuitMessage(0); }});

        registerHotkeys();

        return 0;
    });

    on_message(WM_HOTKEY, [&](wl::params params) -> LRESULT {
        LOG_TRACE_CALLED();
        handleHotkey(params.wParam);
        return 0;
    });

    on_message(WM_SHOWSETTINGS, [this](wl::params) -> LRESULT {
        static bool isSettingsDialogShown = false;
        if (isSettingsDialogShown) {
            return 0;
        }
        isSettingsDialogShown = true;

        SettingsDialog dlg(m_settings, m_keyboardManager);

        // Show the modal dialog, centered on our (hidden) window.
        if (dlg.show(this) == IDOK) {
            // User clicked OK. The dialog has already saved settings to the
            // m_settings object and the INI file. Now, we must apply them.
            LOG_INFO(L"Settings dialog closed with OK. Applying new settings.");
            _applySettings();
        } else {
            // User clicked Cancel or closed the dialog.
            // We must reload the settings from the file to discard any
            // temporary changes the dialog might have made to the object
            // before the user cancelled.
            LOG_INFO(L"Settings dialog was cancelled. Reverting changes.");
            m_settings.load();
        }
        isSettingsDialogShown = false;
        return 0;
    });
}

EventWindow::~EventWindow() {
    LOG_TRACE_CALLED();
    unregisterHotkeys();
}

void EventWindow::registerHotkeys() {
    LOG_TRACE_CALLED();
    static auto modifiers = MOD_ALT | MOD_SHIFT;
    m_hotkeyIDToHKLMap.clear();
    auto layouts = m_keyboardManager.getKeyboardLayouts().value_or({});
    for (size_t i = 0; i < layouts.size(); ++i) {
        unsigned long long hotkeyId = i + 1;
        int vKeyCode = '1' + i;
        if (i == 9) {
            vKeyCode = '0';
        }
        if (RegisterHotKey(hwnd(), hotkeyId, modifiers, vKeyCode) != TRUE) {
            LOG_WARN(L"Failed to register global hotkey with ID {} and vCode {}", hotkeyId, vKeyCode);
            continue;
        }
        m_hotkeyIDToHKLMap[hotkeyId] = layouts[i].hkl;
    }
}

void EventWindow::unregisterHotkeys() {
    LOG_TRACE_CALLED();
    for (const auto& pair : m_hotkeyIDToHKLMap) {
        UnregisterHotKey(hwnd(), pair.first);
    }
}

void EventWindow::handleHotkey(unsigned long long hotkeyId) {
    LOG_TRACE_CALLED();
    auto it = m_hotkeyIDToHKLMap.find(hotkeyId);
    if (it != m_hotkeyIDToHKLMap.end()) {
        m_keyboardManager.setKeyboardLayout(GetForegroundWindow(), it->second);
    }
}

void EventWindow::_applySettings() {
    unregisterHotkeys();
    registerHotkeys();
}
