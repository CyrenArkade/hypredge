#define WLR_USE_UNSTABLE

#include <hyprland/src/includes.hpp>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/managers/PointerManager.hpp>

#include "globals.hpp"

void onMouseMove(const Vector2D pos) {
    auto monitor = g_pCompositor->getMonitorFromVector(pos);
    auto localPos = pos - monitor->vecPosition;
    if (localPos.x == 0 && monitor->activeWorkspace->getFullscreenWindow() == nullptr) {
        monitor->changeWorkspace(getWorkspaceIDNameFromString("e-1").id, false, true);
        g_pCompositor->warpCursorTo(Vector2D(monitor->vecSize.x - 2, pos.y), true);
    } else if (monitor->vecSize.x - localPos.x < 2 && monitor->activeWorkspace->getFullscreenWindow() == nullptr) {
        monitor->changeWorkspace(getWorkspaceIDNameFromString("e+1").id, false, true);
        g_pCompositor->warpCursorTo(Vector2D(1.0, pos.y), true);
    }
}

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();

    // ALWAYS add this to your plugins. It will prevent random crashes coming from
    // mismatched header versions.
    if (HASH != GIT_COMMIT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[MyPlugin] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[MyPlugin] Version mismatch");
    }

    static auto mouseMovePtr = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseMove", [&](void* self, SCallbackInfo& info, std::any data) { onMouseMove(std::any_cast<Vector2D>(data)); });

    return {"MyPlugin", "An amazing plugin that is going to change the world!", "Me", "1.0"};
}
