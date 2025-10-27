#pragma once

#include "windows-keyboard-api.hpp"

#include <map>
#include <memory>
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
