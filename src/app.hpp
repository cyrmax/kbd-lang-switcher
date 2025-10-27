#pragma once

#include "windows-keyboard-api.hpp"

#include <memory>
#include <wx/app.h>

class MyApp : public wxApp {
  public:
    virtual bool OnInit() final;

  private:
    std::unique_ptr<KeyboardManager> m_keyboardManager;
};
