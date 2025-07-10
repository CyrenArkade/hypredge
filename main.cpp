#define WLR_USE_UNSTABLE

#include <hyprland/src/includes.hpp>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/Monitor.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/managers/PointerManager.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>

#include "globals.hpp"

Hyprlang::CParseResult handleEdgeEffect(const char* command, const char* value) {
    Hyprlang::CParseResult result;

    CVarList vars(value);

    if (vars[0].empty() || vars[1].empty()) {
        result.setError("requires edge and dispatcher");
        return result;
    }

    eEdge edge;
    if (vars[0] == "top")
        edge = TOP;
    else if (vars[0] == "bottom")
        edge = BOTTOM;
    else if (vars[0] == "left")
        edge = LEFT;
    else if (vars[0] == "right")
        edge = RIGHT;
    else if (vars[0] == "topleft")
        edge = TOPLEFT;
    else if (vars[0] == "topright")
        edge = TOPRIGHT;
    else if (vars[0] == "bottomleft")
        edge = BOTTOMLEFT;
    else if (vars[0] == "bottomright")
        edge = BOTTOMRIGHT;
    else {
        result.setError(std::format("invalid edge {}", vars[0]).c_str());
        return result;
    }

    const auto DISPATCHER = g_pKeybindManager->m_dispatchers.find(vars[1]);
    if (DISPATCHER == g_pKeybindManager->m_dispatchers.end()) {
        result.setError(std::format("invalid dispatcher {}", vars[1]).c_str());
        return result;
    }

    g_pGlobalState->edgeEffects.emplace_back(edge, vars[1], vars[2]);

    return result;
}

SDispatchResult moveCursorToEdge(std::string args) {
    const auto PMONITOR = g_pCompositor->getMonitorFromCursor();
    const auto pos = g_pPointerManager->position();

    Vector2D warpTo;
    if (args == "top")
        warpTo = {pos.x, PMONITOR->m_position.y + 1};
    else if (args == "bottom")
        warpTo = {pos.x, PMONITOR->m_position.y + PMONITOR->m_size.y - 2};
    else if (args == "left")
        warpTo = {PMONITOR->m_position.x + 1, pos.y};
    else if (args == "right")
        warpTo = {PMONITOR->m_position.x + PMONITOR->m_size.x - 2, pos.y};
    else
        return {.success = false, .error = std::format("hypredge:movecursortoedge: invalid edge {}", args)};

    g_pCompositor->warpCursorTo(warpTo, true);

    return {};
}

std::optional<eEdge> getEdge(const Vector2D localPos, const Vector2D monitorSize) {
    static auto* const PCORNERBARRIER = (Hyprlang::INT* const*)HyprlandAPI::getConfigValue(PHANDLE, "plugin:hypredge:corner_barrier")->getDataStaticPtr();

    const auto distToTop = localPos.y;
    const auto distToBottom = (monitorSize - localPos).y - 1;
    const auto distToLeft = localPos.x;
    const auto distToRight = (monitorSize - localPos).x - 1;
    const auto validEdgeX = distToRight > **PCORNERBARRIER && distToLeft > **PCORNERBARRIER;
    const auto validEdgeY = distToTop > **PCORNERBARRIER && distToBottom > **PCORNERBARRIER;

    if (!distToTop && !distToLeft)
        return TOPLEFT;
    else if (!distToTop && !distToRight)
        return TOPRIGHT;
    else if (!distToBottom && !distToLeft)
        return BOTTOMLEFT;
    else if (!distToBottom && !distToRight)
        return BOTTOMRIGHT;

    if (!distToTop && validEdgeX)
        return TOP;
    else if (!distToBottom && validEdgeX)
        return BOTTOM;
    else if (!distToLeft && validEdgeY)
        return LEFT;
    else if (!distToRight && validEdgeY)
        return RIGHT;

    return std::nullopt;
}

void onMouseMove(const Vector2D pos) {
    auto monitor = g_pCompositor->getMonitorFromVector(pos);
    auto localPos = pos - monitor->m_position;

    auto edge = getEdge(localPos, monitor->m_size);
    if (!edge.has_value()) {
        g_pGlobalState->alreadyActivated = std::nullopt;
        return;
    }

    // If we've already activated from this edge,
    // then don't do it again.
    if (g_pGlobalState->alreadyActivated.has_value() && g_pGlobalState->alreadyActivated.value() == edge)
        return;
    g_pGlobalState->alreadyActivated = edge;

    // If the mouse is constrained to a window, don't activate.
    if (g_pInputManager->isConstrained())
        return;

    for (auto edgeEffect : g_pGlobalState->edgeEffects) {
        if (edgeEffect.edge != edge.value())
            continue;
        g_pKeybindManager->m_dispatchers[edgeEffect.dispatcher](edgeEffect.arg);
    }
}

static void onPreConfigReload() {
    g_pGlobalState->edgeEffects.clear();
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
        HyprlandAPI::addNotification(PHANDLE, "[hypredge] Mismatched headers! Can't proceed.",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[hypredge] Version mismatch");
    }

    g_pGlobalState = makeUnique<SGlobalState>();

    HyprlandAPI::addConfigValue(PHANDLE, "plugin:hypredge:corner_barrier", Hyprlang::INT{100});

    HyprlandAPI::addConfigKeyword(PHANDLE, "edge-effect", handleEdgeEffect, {false});

    HyprlandAPI::addDispatcherV2(PHANDLE, "hypredge:movecursortoedge", moveCursorToEdge);

    static auto mouseMovePtr = HyprlandAPI::registerCallbackDynamic(PHANDLE, "mouseMove", [&](void* self, SCallbackInfo& info, std::any data) { onMouseMove(std::any_cast<Vector2D>(data)); });
    static auto clearConfigPtr = HyprlandAPI::registerCallbackDynamic(PHANDLE, "preConfigReload", [&](void* self, SCallbackInfo& info, std::any data) { onPreConfigReload(); });

    return {"hypredge", "Trigger dispatchers on screen edges", "CyrenArkade", "0.1"};
}
