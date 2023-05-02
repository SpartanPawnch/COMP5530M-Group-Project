#pragma once
#include <vector>
#include <map>
#include "ControllerComponent.h"


struct VirtualKey {
	std::string name;
	int key;
	float scale;
	int action;
};

struct PlayerControllerComponent : BaseComponent {
	PlayerControllerComponent();

	std::vector<VirtualKey> virtualKeys;

	bool listeningForKey;
	unsigned int listeningForKeyIndex;

	void addKey();
	void removeKey(int index);

	void addAction();

};