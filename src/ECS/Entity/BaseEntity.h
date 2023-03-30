#pragma once

#include <vector>
#include <string>

#include "../Component/BaseComponent.h"

//Requires Change!!
class BaseEntity{
public:
    BaseEntity();
    ~BaseEntity();
    virtual void start();
    virtual void update(float dt);
    virtual void stop();
    void removeComponent(int uuid);
    void addChild(BaseEntity entity);

    //look for the best uuid generator
    int uuid;
    std::string name;
    std::vector<BaseComponent*> components;
    std::vector<BaseEntity> children;
};