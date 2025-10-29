#include "EventWindow.hpp"

#include "../assets/resources.h"
#include "log-macro.hpp"
#include "windows-keyboard-api.hpp"

EventWindow::EventWindow(KeyboardManager& keyboardManager) : m_keyboardManager(keyboardManager) {
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

        auto show_settings = [&]() { return; };
        m_tray = std::make_unique<Tray::Tray>("Keyboard-Language-Switcher", hIcon);
        m_tray->addEntry(Tray::Button{"Settings...", show_settings});
        m_tray->addEntry(Tray::Separator{});
        m_tray->addEntry(Tray::Button{"Quit", [&]() { PostMessage(hwnd(), WM_CLOSE, 0, 0); }});

        registerHotkeys();

        return 0;
    });

    on_message(WM_HOTKEY, [&](wl::params params) -> LRESULT {
        LOG_TRACE_CALLED();
        handleHotkey(params.wParam);
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
