#pragma once

#include <vector>

class BaseComponent{
public:
    BaseComponent();
    ~BaseComponent();
    void start();
    void update(float dt);
    void stop();

    int uuid;
};