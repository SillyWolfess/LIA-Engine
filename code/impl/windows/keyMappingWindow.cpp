#include "windows/keyMappingWindow.hpp"
#include "manager/keybindingManager.hpp"
#include "manager/eventManager.hpp"
#include "logs.hpp"

bool LIA::KeyMappingWindow::init() {
     _eventSource = "keyMapping";
     return true;
}

bool LIA::KeyMappingWindow::registerHandlers() {
    if (!subscribe("get_data_gui")) {
        LIA_fatal("Failed to subscribe to gui update");
        return false;
    }
    return true;
}

bool LIA::KeyMappingWindow::onGetGuiData(LIA::Event& event) {
    if (event.source.compare(_eventSource) != 0) {
        return false;
    }
    updateData(event.window);
    return true;
}

void LIA::KeyMappingWindow::updateData(std::string window) {
    LIA_TRY
        LIA::KeybindingManager *keyBindingManager = getKeybindingManager();
        std::vector<std::string> keyGroups = keyBindingManager->keySet();
        std::vector<std::string> data;
        for (std::string keyGroupName : keyGroups) {
            LIA::KeybindingControls &controls = keyBindingManager->getControls(keyGroupName);
            std::vector<std::string> keySet = controls.keySet();
            if (!keySet.empty()) {
                data.push_back(keyGroupName);
                for (std::string key : keySet) {
                    int keyId = controls.get(key);
                    char c = (char) keyId;
                    data.push_back(std::vformat("{} : {}", std::make_format_args(key, c)));
                }
            }
        }
        LIA::UpdateGuiListEvent ev(window, "list", data, -1);
        getEventManager()->handleEvent(ev);
    LIA_CATCH_EMPTY
}