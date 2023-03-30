#pragma once

#include <vector>
#include <string>

#include <imgui.h>

class BaseComponent{
public:
    BaseComponent();
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt);
    virtual void stop();
    virtual void drawInterface();

    int uuid;
    std::string name;
};