#pragma once
#include "windows-keyboard-api.hpp"

#include <map>
#include <memory>
#include <shellapi.h>
#include <tray.hpp>
#include <winlamb/window_main.h>

class EventWindow final : public wl::window_main {
  public:
    explicit EventWindow(KeyboardManager&);
    ~EventWindow();

  private:
    KeyboardManager& m_keyboardManager;
    std::unique_ptr<Tray::Tray> m_tray;
    std::map<unsigned long long, HKL> m_hotkeyIDToHKLMap;

    void registerHotkeys();
    void unregisterHotkeys();
    void handleHotkey(unsigned long long hotkeyId);
};
