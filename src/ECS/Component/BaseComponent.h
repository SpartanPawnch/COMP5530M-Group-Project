#pragma once

#include <vector>

class BaseComponent{
public:
    BaseComponent();
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt);
    virtual void stop();

    int uuid;
};