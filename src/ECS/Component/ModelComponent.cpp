#include "ModelComponent.h"
#include "../../scripting.h"

static int baseUuid = 0;

int ModelComponent::genUuid() {
    return baseUuid++;
}

ModelComponent::ModelComponent() {
    name = "Model Component";
    uuid = baseUuid++;
}
ModelComponent::ModelComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

void ModelComponent::start() {
}
void ModelComponent::update(float dt, EntityState& state) {
}
void ModelComponent::stop() {
}

void ModelComponent::readMaterials() {
    materials.clear();
    MaterialSystem* instance = MaterialSystem::getInstance();
    if (modelDescriptor) {
        materials.resize(modelDescriptor->getMeshCount());
        for (uint32_t i = 0; i < modelDescriptor->getMeshCount(); i++) {
            materials[i] = instance->getActiveMaterial(modelDescriptor->getMeshMaterialName(i));
        }
    }
}

// lua stuff
static const char* componentMT = "ONO_ModelComponent";
void ModelComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pop(state, 1);
}

void ModelComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    luaL_setmetatable(state, componentMT);
}
