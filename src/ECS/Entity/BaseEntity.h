#pragma once

#include <vector>

#include "../Component/BaseComponent.h"

//Requires Change!!
class BaseEntity{
public:
    BaseEntity();
    ~BaseEntity();
    void start();
    void update(float dt);
    void stop();
    void addComponent(BaseComponent component);
    void removeComponent(int uuid);

    //look for the best uuid generator
    int uuid;
    std::vector<BaseComponent> components;
};