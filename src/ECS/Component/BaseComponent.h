#pragma once

#include <vector>
#include <string>


class BaseComponent {
  public:
    BaseComponent();
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt);
    virtual void stop();

    int uuid;
    std::string name;
};
