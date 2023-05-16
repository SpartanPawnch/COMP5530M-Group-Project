#include "PlayerControllerComponent.h"
#include <array>

#include "../System/InputSystem.h"

static int baseUuid = 0;
PlayerControllerComponent::PlayerControllerComponent()
    : listeningForKey(false), listeningForKeyIndex(0) {

    name = "Player Controller Component";
    uuid = baseUuid++;
}

PlayerControllerComponent::PlayerControllerComponent(const std::string& _name, int _uuid) {
    name = _name;
    uuid = _uuid;
}

void PlayerControllerComponent::addKey() {
    VirtualKey vk;
    vk.name = "new key " + std::to_string(virtualKeys.size());
    vk.key = -1;
    vk.scale = 1.0f;
    virtualKeys.push_back(vk);
}

void PlayerControllerComponent::removeKey(int index) {
    virtualKeys.erase(virtualKeys.begin() + index);
}

void PlayerControllerComponent::update(float dt, EntityState& state) {
    std::array<float, VirtualKey::ACTIONS_MAX> actions{.0f};
    static InputSystem* iSys = InputSystem::getInstance();

    // update virtual virtual keys
    for (size_t i = 0; i < virtualKeys.size(); i++) {
        if (virtualKeys[i].action < 0 || virtualKeys[i].key < 0)
            continue;

        actions[virtualKeys[i].action] = glm ::max(actions[virtualKeys[i].action],
            iSys->isDown[virtualKeys[i].key] ? virtualKeys[i].scale : .0f);
    }

    // update position
    state.position = state.position +
        dt *
            glm::vec3(actions[VirtualKey::RIGHT] - actions[VirtualKey::LEFT],
                actions[VirtualKey::UP] - actions[VirtualKey::DOWN],
                actions[VirtualKey::FORWARD] - actions[VirtualKey::BACK]);
}
