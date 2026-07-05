#include "windows/debugWindow.hpp"
#include "manager/eventManager.hpp"
#include "appWindow.hpp"
#include "logs.hpp"

bool LIA::DebugWindow::init() {
    _eventSource = "debug";
    return true;
}

bool LIA::DebugWindow::registerHandlers() {
    if (!subscribe(ComponentEvent::GET_DATA_GUI)) {
        LIA_fatal("Failed to subscribe to gui update");
        return false;
    }
    if (!subscribe(ComponentEvent::POSITION_CHANGED)) {
        LIA_fatal("Failed to subscribe to position changed event");
        return false;
    }
    return true;
}

bool LIA::DebugWindow::onGetGuiData(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    updateData(event.window);
    return true;
}

bool LIA::DebugWindow::onPositionChanged(LIA::Event& event) {
    if (event.source.compare("camera") != 0) {
        return false;
    }
    LIA_TRY
        AppWindow* appWindow = getAppWindow();
        Camera camera = appWindow->getCamera();
        EventManager* eventManager = getEventManager();

        Position& pos = camera.getPosition();
        std::string cPos = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(pos.x, pos.y, pos.z));
        UpdateGuiEvent updateGuiEvent(_eventSource, "camera_position", cPos);
        eventManager->handleEvent(updateGuiEvent);

        Position& lookAt = camera.getLookAt();
        std::string cLookAt = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(lookAt.x, lookAt.y, lookAt.z));
        UpdateGuiEvent updateLookAtEvent(_eventSource, "camera_lookAt", cLookAt);
        eventManager->handleEvent(updateLookAtEvent);
    LIA_CATCH_EMPTY
    return true;
}

void LIA::DebugWindow::updateData(std::string window) {
    LIA_TRY
        AppWindow* appWindow = getAppWindow();
        EventManager* eventManager = getEventManager();
        Camera camera = appWindow->getCamera();
        
        Position& pos = camera.getPosition();
        std::string cPos = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(pos.x, pos.y, pos.z));
        UpdateGuiEvent updateGuiEvent(window, "camera_position", cPos);
        eventManager->handleEvent(updateGuiEvent);

        Position& lookAt = camera.getLookAt();
        std::string cLookAt = std::vformat("{:.3f} x {:.3f} x {:.3f}", std::make_format_args(lookAt.x, lookAt.y, lookAt.z));
        UpdateGuiEvent updateLookAtEvent(window, "camera_lookAt", cLookAt);
        eventManager->handleEvent(updateLookAtEvent);
    LIA_CATCH_EMPTY
}