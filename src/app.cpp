#include "app.hpp"

#include "log-macro.hpp"
#include "loggerSetup.hpp"
#include "main_taskbar_icon.hpp"
#include "windows-keyboard-api.hpp"

#include <wx/msw/wrapwin.h>
#include <wx/wx.h>

static const char* placeholder_icon_xpm[] = {
    "16 16 1 1",
    "g c #00C000", // A medium green color
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg",
    "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg", "gggggggggggggggg"};

bool MyApp::OnInit() {
    InitializeLogging(true);
    if (!wxApp::OnInit()) {
        LOG_CRITICAL("Failed to start the WX application");
        return false;
    }
    if (!wxTaskBarIcon::IsAvailable()) {
        LOG_ERROR("Tray icons are not available on this system");
        wxMessageBox("Your system does not support system tray icons. The application will not work.",
                     "No system tray warning", wxOK | wxICON_EXCLAMATION);
        // TODO: somehow handle delayed app shutdown only when the OK button is pressed
    }
    m_keyboardManager = std::make_unique<KeyboardManager>();
    auto* taskbarIcon = new MainTaskbarIcon(*m_keyboardManager);
    if (!taskbarIcon->SetIcon(wxIcon(placeholder_icon_xpm), "Keyboard language switcher")) {
        LOG_CRITICAL("Failed to show the taskbar icon");
        return false;
    }
    return true;
}
