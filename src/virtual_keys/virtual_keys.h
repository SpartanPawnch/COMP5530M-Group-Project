#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <mutex>
#include <GLFW/glfw3.h>

struct VirtualKey {
	std::string name;
	int key;
	float direction;
};

class VirtualKeys {
public:
	std::vector<VirtualKey> virtualKeys;
	bool listeningForKey = false;
	unsigned int listeningForKeyIndex = 0;

	VirtualKeys() = default;
	void addKey();
	void removeKey(int index);
	static void glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);
	void glfw_callback_key_press(GLFWwindow* window, int key, int scancode, int action, int mods);
};