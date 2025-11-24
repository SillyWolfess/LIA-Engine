#include "manager/keybindingManager.hpp"
#include "tools/macros.hpp"
#include "logs.hpp"

bool LIA::KeybindingManager::init() {
    LIA_info("Keybinding manager initialized");
    return true;
}

LIA::KeybindingManager::KeybindingManager() {
    LIA_trace("KeybindingManager constructor");
}

LIA::KeybindingManager::~KeybindingManager() {
    LIA_trace("KeybindingManager destroyed");
}