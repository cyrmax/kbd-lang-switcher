#include "EventWindow.hpp"
#include "SettingsStorage.hpp"
#include "log-macro.hpp"
#include "loggerSetup.hpp"

#include <memory>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
    InitializeLogging(false);
    LOG_TRACE_CALLED();

    auto keyboardManager = std::make_unique<KeyboardManager>();
    auto settingsStorage = std::make_unique<SettingsStorage>();
    EventWindow hiddenWindow(*keyboardManager, *settingsStorage);

    int exitCode = hiddenWindow.winmain_run(hInstance, SW_HIDE);

    LOG_INFO("Main window loop has finished. Shutting down");
    spdlog::shutdown();

    return exitCode;
}
