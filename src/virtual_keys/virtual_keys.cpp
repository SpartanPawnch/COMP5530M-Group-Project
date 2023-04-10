#include "virtual_keys.h"

void VirtualKeys::addKey() {
	VirtualKey vk;
	vk.name = "new key " + std::to_string(virtualKeys.size());
	vk.key = -1;
	vk.direction = 1.0f;
	virtualKeys.push_back(vk);
}

void VirtualKeys::removeKey(int index) {
	virtualKeys.erase(virtualKeys.begin() + index);
}

void VirtualKeys::glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods) {
	VirtualKeys* vk = static_cast<VirtualKeys*>(glfwGetWindowUserPointer(window));
	vk->glfw_callback_key_press(window, key, scancode, action, mods);
}

//when a key is pressed
void VirtualKeys::glfw_callback_key_press(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int /*aModifierFlags*/)
{
	if (listeningForKey) {
		virtualKeys.at(listeningForKeyIndex).key = aKey;
		listeningForKey = false;
		return;
	}
	for (unsigned int i = 0; i < virtualKeys.size(); i++) {
		if (virtualKeys.at(i).key == aKey) {
			std::cout << "Fired event " << virtualKeys.at(i).name << " in the " << virtualKeys.at(i).direction << " direction" << std::endl;
		}
	}
}
