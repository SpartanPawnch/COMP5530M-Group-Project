#include "LightComponent.h"

#include <glm/vec4.hpp>

#include "../../../render-engine/LightSource.h"
#include "../../../render-engine/RenderManager.h"

static int baseUuid = 0;

static RenderManager* renderMgr;

LightComponent::LightComponent() {
    uuid = baseUuid++;
    name = "Point Light Component";
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addLightSource(position, ambient, diffuse, specular);
}

LightComponent::LightComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addLightSource(position, ambient, diffuse, specular);
}

LightComponent::~LightComponent() {
    if (desc.use_count() == 1)
        renderMgr->removeLightSource(desc->idx);
}
void LightComponent::start() {
}
void LightComponent::update(float dt, EntityState& state) {
    LightSource* lightPtr = renderMgr->getLightSource(desc->idx);
    lightPtr->setPosition(glm::vec3(state.runtimeTransform * glm::vec4(position, 1.f)));
    lightPtr->setAmbient(ambient);
    lightPtr->setDiffuse(diffuse);
    lightPtr->setSpecular(specular);
}
void LightComponent::stop() {
}
