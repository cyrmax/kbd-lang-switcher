#pragma once

#include <optional>
#include <string>
#include <vector>
#include <windows.h>

struct KeyboardLayoutInfo {
    std::wstring languageName;
    std::wstring keyboardLayoutName;
    HKL hkl;
};

class KeyboardManager {
  public:
    std::optional<std::vector<KeyboardLayoutInfo>> getKeyboardLayouts() const;
    bool setKeyboardLayout(HWND, HKL) const;

  private:
    std::optional<std::wstring> getLanguageName(HKL) const;
    std::optional<std::wstring> getKeyboardLayoutName(HKL) const;
};
