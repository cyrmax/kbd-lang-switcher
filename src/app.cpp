#include "app.hpp"

#include "log-macro.hpp"
#include "loggerSetup.hpp"
#include "window.hpp"
#include "windows-keyboard-api.hpp"

#include <wx/msw/wrapwin.h>
#include <wx/wx.h>

bool MyApp::OnInit() {
    InitializeLogging(true);
    if (!wxApp::OnInit()) {
        LOG_CRITICAL("Failed to start the WX application");
        return false;
    }
    auto* frame = new EventFrame();
    return true;
}
