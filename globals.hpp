#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>

inline HANDLE PHANDLE = nullptr;

enum eEdge : int8_t {
    TOP = 0,
    BOTTOM,
    LEFT,
    RIGHT,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT,
};

struct SEdgeEffect {
    eEdge edge;
    std::string dispatcher;
    std::string arg;
};

struct SGlobalState {
    std::vector<SEdgeEffect> edgeEffects;
    bool alreadyActivated;
};

inline UP<SGlobalState> g_pGlobalState;
