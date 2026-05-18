#pragma once

#include <hyprland/src/plugins/PluginAPI.hpp>

#include <hyprland/src/config/values/types/BoolValue.hpp>
#include <hyprland/src/config/values/types/IntValue.hpp>

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
    std::string arg;
};

inline struct {
    std::vector<SEdgeEffect> edgeEffects;
    std::optional<eEdge> alreadyActivated;

    Desktop::Rule::CWindowRuleEffectContainer::storageType ignoreConstraintRuleIdx;

    struct {
        SP<Config::Values::CIntValue> corner_barrier;
    } config;
} g_state;
