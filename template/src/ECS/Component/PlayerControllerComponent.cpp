#include "PlayerControllerComponent.h"
#include <iostream>

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
