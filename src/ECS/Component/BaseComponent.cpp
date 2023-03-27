#include "BaseComponent.h"

BaseComponent::BaseComponent()
{
    start();
}

BaseComponent::~BaseComponent()
{
    stop();
}