#include "windows/settingsWindow.hpp"
#include "manager/eventManager.hpp"
#include "appWindow.hpp"
#include "logs.hpp"

bool LIA::SettingsWindow::init() {
    _eventSource = "settings";
    return true;
}

bool LIA::SettingsWindow::registerHandlers() {
    if (!subscribe(ComponentEvent::GET_DATA_GUI)) {
        LIA_fatal("Failed to subscribe to gui update");
        return false;
    }
    if (!subscribe(ComponentEvent::CHECKBOX_ACTION)) {
        LIA_fatal("Failed to subscribe to checkbox action");
        return false;
    }
    if (!subscribe(ComponentEvent::OPTION_ACTION)) {
        LIA_fatal("Failed to subscribe to option change action");
        return false;
    }
    return true;
}

bool LIA::SettingsWindow::onGetGuiData(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    updateData(event.window);
    return true;
}

bool LIA::SettingsWindow::onCheckboxAction(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    handleCheckbox(event);
    return true;
}

bool LIA::SettingsWindow::onOptionChanged(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    handleOptionChanged(event);
    return true;
}

void LIA::SettingsWindow::handleOptionChanged(LIA::Event& event) {
    LIA_TRY
        if (event.action.compare("resolution") == 0) {
            AppWindow* appWindow = getAppWindow();
            appWindow->setMode(event.argi);
        }
    LIA_CATCH_EMPTY
}

void LIA::SettingsWindow::handleCheckbox(LIA::Event& event) {
    LIA_TRY
        if (event.action.compare("fullscreen") == 0) {
            AppWindow* appWindow = getAppWindow();
            appWindow->toogleFullscreen();

            EventManager* eventManager = getEventManager();
            AppWindow::s_glfwSettings glfwSettings = appWindow->getSettings();
            UpdateGuiEvent updateGuiEvent(_eventSource, "fullscreen", glfwSettings.fullscreen);
            eventManager->handleEvent(updateGuiEvent);
        }
    LIA_CATCH_EMPTY
}

void LIA::SettingsWindow::updateData(std::string window) {
    LIA_TRY
        AppWindow* appWindow = getAppWindow();
        AppWindow::s_glfwSettings glfwSettings = appWindow->getSettings();
        EventManager* eventManager = getEventManager();
        UpdateGuiEvent updateGuiEvent(window, "fullscreen", glfwSettings.fullscreen);
        eventManager->handleEvent(updateGuiEvent);
        
        std::vector<std::string> resList;
        std::vector<AppWindow::s_windowModes> resolutions = appWindow->getResolutions();
        for (AppWindow::s_windowModes& mode: resolutions) {
            resList.push_back(std::vformat("{} x {}", std::make_format_args(mode.width, mode.height)));
        }
        UpdateGuiListEvent updateResolutionEvent(_eventSource, "resolution", resList, glfwSettings.windowMode);
        eventManager->handleEvent(updateResolutionEvent);
    LIA_CATCH_EMPTY
}