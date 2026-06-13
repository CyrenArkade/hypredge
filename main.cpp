#define WLR_USE_UNSTABLE

#include <hyprland/src/includes.hpp>

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/helpers/MiscFunctions.hpp>
#include <hyprland/src/managers/PointerManager.hpp>
#include <hyprland/src/managers/input/InputManager.hpp>
#include <hyprland/src/desktop/state/FocusState.hpp>
#include <hyprland/src/event/EventBus.hpp>
#include <hyprland/src/state/MonitorState.hpp>
#include <hyprutils/string/VarList.hpp>
#include <hyprland/src/config/lua/bindings/LuaBindingsInternal.hpp>

#include "globals.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
}

int defineEdgeEffect(lua_State* L) {
    std::string_view arg_edge = luaL_checkstring(L, 1);
    if (!Config::Lua::Bindings::Internal::pushDispatcherFunction(L, 2))
        return Config::Lua::Bindings::Internal::configError(L, "hl.edge_effect: dispatcher must be a dispatcher (e.g. hl.dsp.window.close()) or a lua function");

    eEdge edge;
    if (arg_edge == "top")
        edge = TOP;
    else if (arg_edge == "bottom")
        edge = BOTTOM;
    else if (arg_edge == "left")
        edge = LEFT;
    else if (arg_edge == "right")
        edge = RIGHT;
    else if (arg_edge == "topleft")
        edge = TOPLEFT;
    else if (arg_edge == "topright")
        edge = TOPRIGHT;
    else if (arg_edge == "bottomleft")
        edge = BOTTOMLEFT;
    else if (arg_edge == "bottomright")
        edge = BOTTOMRIGHT;
    else {
        return Config::Lua::Bindings::Internal::configError(L, "hl.edge_effect: edge must be a valid edge (top, bottom, left, right, topleft, topright, bottomleft, bottomright)");
    }

    int dispatcher_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    g_state.edgeEffects.emplace_back(edge, std::to_string(dispatcher_ref));

    return true;
}

int moveCursorToEdge(lua_State* L) {
    std::string_view arg_edge = luaL_checkstring(L, 1);
    
    const auto pos = g_pPointerManager->position();
    const auto monitor = State::monitorState()->query().vec(pos).run();

    Vector2D warpTo;
    if (arg_edge == "top")
        warpTo = {pos.x, monitor->m_position.y + 1};
    else if (arg_edge == "bottom")
        warpTo = {pos.x, monitor->m_position.y + monitor->m_size.y - 2};
    else if (arg_edge == "left")
        warpTo = {monitor->m_position.x + 1, pos.y};
    else if (arg_edge == "right")
        warpTo = {monitor->m_position.x + monitor->m_size.x - 2, pos.y};
    else
        return false;

    g_pCompositor->warpCursorTo(warpTo, true);

    return true;
}

std::optional<eEdge> getEdge(const Vector2D localPos, const Vector2D monitorSize) {
    const auto CORNER_BARRIER = g_state.config.corner_barrier->value();
    
    const auto distToTop = localPos.y;
    const auto distToBottom = (monitorSize - localPos).y - 1;
    const auto distToLeft = localPos.x;
    const auto distToRight = (monitorSize - localPos).x - 1;
    const auto validEdgeX = distToRight > CORNER_BARRIER && distToLeft > CORNER_BARRIER;
    const auto validEdgeY = distToTop > CORNER_BARRIER && distToBottom > CORNER_BARRIER;

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
    
    auto monitor = State::monitorState()->query().vec(pos).run();
    auto window = Desktop::focusState()->window();
    auto localPos = pos - monitor->m_position;

    auto edge = getEdge(localPos, monitor->m_size);
    if (!edge.has_value()) {
        g_state.alreadyActivated = std::nullopt;
        return;
    }

    // If we've already activated from this edge,
    // then don't do it again.
    if (g_state.alreadyActivated.has_value() && g_state.alreadyActivated.value() == edge)
        return;
    g_state.alreadyActivated = edge;

    // If the mouse is constrained to a window and we don't have hypredge:ignore_constraints, don't activate.
    if (
        window
        && (!window->m_ruleApplicator->m_otherProps.props.contains(g_state.ignoreConstraintRuleIdx)
            || window->m_ruleApplicator->m_otherProps.props[g_state.ignoreConstraintRuleIdx]->effect != "on")
        && g_pInputManager->isConstrained()
    )
        return;

    for (auto edgeEffect : g_state.edgeEffects) {
        if (edgeEffect.edge != edge.value())
            continue;
        g_pKeybindManager->m_dispatchers["__lua"](edgeEffect.arg);
    }
}

static void onPreConfigReload() {
    g_state.edgeEffects.clear();
}

// Do NOT change this function.
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH        = __hyprland_api_get_hash();
    const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

    if (HASH != CLIENT_HASH) {
        HyprlandAPI::addNotification(PHANDLE, "[hyprland] Failure in initialization: Version mismatch (headers ver is not equal to running hyprland ver)",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[hs] Version mismatch");
    }

    g_state.ignoreConstraintRuleIdx = Desktop::Rule::windowEffects()->registerEffect("hypredge_ignore_constraints");
    g_state.config.corner_barrier = makeShared<Config::Values::CIntValue>("plugin:hypredge:corner_barrier", "Barrier around corners preventing edges from activating.", 100);

    HyprlandAPI::addConfigValueV2(PHANDLE, g_state.config.corner_barrier);
    
    HyprlandAPI::addLuaFunction(PHANDLE, "hypredge", "edge_effect", defineEdgeEffect);
    HyprlandAPI::addLuaFunction(PHANDLE, "hypredge", "move_cursor_to_edge", moveCursorToEdge);

    static auto mouseMovePtr = Event::bus()->m_events.input.mouse.move.listen([&](Vector2D pos, Event::SCallbackInfo& info) { onMouseMove(pos); });
    static auto clearConfigPtr = Event::bus()->m_events.config.preReload.listen([&] { onPreConfigReload(); });

    return {"hypredge", "Trigger dispatchers on screen edges", "CyrenArkade", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    Desktop::Rule::windowEffects()->unregisterEffect(g_state.ignoreConstraintRuleIdx);
}
