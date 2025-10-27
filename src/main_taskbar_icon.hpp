#pragma once

#include <memory>
#include <windows-keyboard-api.hpp>
#include <wx/taskbar.h>

class MainTaskbarIcon : public wxTaskBarIcon {
  public:
    explicit MainTaskbarIcon(KeyboardManager&);

  protected:
    virtual wxMenu* CreatePopupMenu() final;

  private:
    KeyboardManager& m_keyboardManager;
    void OnMenuSettings(wxCommandEvent&);
    void OnMenuExit(wxCommandEvent&);
    DECLARE_EVENT_TABLE();
};
