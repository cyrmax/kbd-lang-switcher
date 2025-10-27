#pragma once

#include "windows-keyboard-api.hpp"

#include <map>
#include <memory>
#include <wx/wx.h>

class EventFrame : public wxFrame {
  public:
    EventFrame();
    ~EventFrame();

  private:
    std::unique_ptr<KeyboardManager> m_keyboardManager;
    std::map<int, HKL> m_hotkeyIDToHKLMap;

    void OnHotkey(wxKeyEvent&);

    void registerHotkeys();
    void unregisterHotkeys();
};
