#include "PlayerControllerComponent.h"
#include <iostream>

PlayerControllerComponent::PlayerControllerComponent() : listeningForKey(false), listeningForKeyIndex(0) {}

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