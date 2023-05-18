#pragma once
#include <vector>
#include <map>
#include "ControllerComponent.h"
#include "RigidBodyComponent.h"

struct VirtualKey {
    enum Action {
        NONE = -1,
        FORWARD = 0,
        BACK,
        LEFT,
        RIGHT,
        UP,
        DOWN,
        ACTIONS_MAX
    };
    std::string name;
    int key;
    float scale;
    Action action = NONE;
};

struct PlayerControllerComponent : BaseComponent {
    PlayerControllerComponent();
    PlayerControllerComponent(const std::string& _name, int _uuid);

    void update(float dt, EntityState& state) override;

    std::vector<VirtualKey> virtualKeys;

    bool listeningForKey = false;
    unsigned int listeningForKeyIndex;

    std::vector<RigidBodyComponent>* rigidBodies;

    void addKey();
    void removeKey(int index);

    void addAction();
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();
};
