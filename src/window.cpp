#include "window.hpp"

#include "log-macro.hpp"
#include "main_taskbar_icon.hpp"
#include "windows-keyboard-api.hpp"

#include <memory>
#include <wx/taskbar.h>

static const char* placeholder_icon_xpm[] = {
    "16 16 1 1",
    "g c #00C000", // A medium green color
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg"};

EventFrame::EventFrame()
    : wxFrame(nullptr, wxID_ANY, "kbd-lang-switcher-evt-wnd", wxDefaultPosition, wxDefaultSize, wxFRAME_NO_TASKBAR) {
    if (!wxTaskBarIcon::IsAvailable()) {
        LOG_ERROR("Tray icons are not available on this system");
        wxMessageBox("Your system does not support system tray icons. The application will not work.",
                     "No system tray warning", wxOK | wxICON_EXCLAMATION);
    }
    m_keyboardManager = std::make_unique<KeyboardManager>();
    auto* taskbarIcon = new MainTaskbarIcon(*m_keyboardManager);
    if (!taskbarIcon->SetIcon(wxIcon(placeholder_icon_xpm), "Keyboard language switcher")) {
        LOG_CRITICAL("Failed to show the taskbar icon");
        wxMessageBox("Failed to show the taskbar icon", "Error!", wxOK | wxICON_EXCLAMATION);
    }
    this->Bind(wxEVT_HOTKEY, &EventFrame::OnHotkey, this);
    registerHotkeys();
}

EventFrame::~EventFrame() {
    LOG_TRACE_CALLED();
    unregisterHotkeys();
}

void EventFrame::registerHotkeys() {
    LOG_TRACE_CALLED();
    static auto modifiers = wxMOD_SHIFT | wxMOD_ALT;
    m_hotkeyIDToHKLMap.clear();
    auto layouts = m_keyboardManager.get()->getKeyboardLayouts().value_or({});
    for (size_t i = 0; i < layouts.size(); ++i) {
        int hotkeyId = i + 1;
        int vKeyCode = '1' + i;
        if (i == 9) {
            vKeyCode = '0';
        }
        bool result = this->RegisterHotKey(hotkeyId, modifiers, vKeyCode);
        if (!result) {
            LOG_WARN("Failed to register hotkey {}", vKeyCode);
            continue;
        }
        m_hotkeyIDToHKLMap[hotkeyId] = layouts[i].hkl;
    }
}

void EventFrame::unregisterHotkeys() {
    LOG_TRACE_CALLED();
    for (const auto& pair : m_hotkeyIDToHKLMap) {
        this->UnregisterHotKey(pair.first);
    }
}

void EventFrame::OnHotkey(wxKeyEvent& event) {
    LOG_TRACE_CALLED();
    int hotkeyId = event.GetId();
    auto it = m_hotkeyIDToHKLMap.find(hotkeyId);
    if (it != m_hotkeyIDToHKLMap.end()) {
        m_keyboardManager.get()->setKeyboardLayout(GetForegroundWindow(), it->second);
    }
}
