#include "main_taskbar_icon.hpp"

#include "log-macro.hpp"

#include <functional>
#include <wx/app.h>
#include <wx/menu.h>

MainTaskbarIcon::MainTaskbarIcon(KeyboardManager& keyboardManager) : m_keyboardManager(keyboardManager) {}

wxMenu* MainTaskbarIcon::CreatePopupMenu() {
    LOG_TRACE_CALLED();
    auto* popupMenu = new wxMenu;
    auto* layoutsSubmenu = m_keyboardManager.getKeyboardLayouts()
                               .transform([](const std::vector<KeyboardLayoutInfo>& layouts) {
                                   auto* submenu = new wxMenu;
                                   if (layouts.empty()) {
                                       LOG_WARN("Layouts collection is empty. Returning stub.");
                                       submenu->Append(wxID_ANY, "No layouts available");
                                   } else {
                                       for (const auto& layout : layouts) {
                                           submenu->Append(wxID_ANY, wxString(layout.languageName));
                                       }
                                   }
                                   return submenu;
                               })
                               .or_else([]() -> std::optional<wxMenu*> {
                                   LOG_WARN("Unable to get keyboard layouts");
                                   auto* submenu = new wxMenu;
                                   submenu->Append(wxID_ANY, "No layouts available");
                                   return submenu;
                               })
                               .value_or(nullptr);
    if (layoutsSubmenu) {
        popupMenu->AppendSubMenu(layoutsSubmenu, "Layouts");
    }
    popupMenu->AppendSeparator();
    popupMenu->Append(wxID_PREFERENCES, "Settings");
    popupMenu->Append(wxID_EXIT, "Quit");
    return popupMenu;
}

void MainTaskbarIcon::OnMenuSettings(wxCommandEvent& event) {
    // TODO
}

void MainTaskbarIcon::OnMenuExit(wxCommandEvent& event) {
    wxTheApp->Exit();
}

BEGIN_EVENT_TABLE(MainTaskbarIcon, wxTaskBarIcon)
EVT_MENU(wxID_EXIT, MainTaskbarIcon::OnMenuExit)
END_EVENT_TABLE()
