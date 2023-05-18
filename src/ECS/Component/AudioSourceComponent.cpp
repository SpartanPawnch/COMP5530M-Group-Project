#include "AudioSourceComponent.h"

#include "../../asset_import/audio.h"

static int baseUuid = 0;

#include "../../scripting.h"

AudioSourceComponent::AudioSourceComponent() {
    name = "Audio Source Component";
    uuid = baseUuid++;
}

AudioSourceComponent::AudioSourceComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

void AudioSourceComponent::start() {
    if (playOnStart)
        startSound();
}
void AudioSourceComponent::update(float dt, EntityState& state) {
    // update position
}
void AudioSourceComponent::stop() {
}

void AudioSourceComponent::startSound() {
    assert(clipDescriptor);
    audio::audioSetLoop(clipDescriptor->idx, loop);
    audio::audioPlay(clipDescriptor->idx);
}

// lua stuff
static const char* componentMT = "ONO_AudioSourceComponent";
static int luaPlaySound(lua_State* state) {
    // check argument count
    int argc = lua_gettop(state);
    if (argc != 1) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_AudioSourceComponent:play() - wrong number of arguments; "
            "Usage: ONO_AudioSourceComponent:play(var) or var:play()");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    AudioSourceComponent* c = (AudioSourceComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_AudioSourceComponent - audio clip is null", 0);
        return 0;
    }

    // play sound and finish
    c->startSound();

    lua_settop(state, 0);
    return 0;
}

void AudioSourceComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, &luaPlaySound);
    lua_setfield(state, -2, "play");
    lua_pop(state, 1);
}

void AudioSourceComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    luaL_setmetatable(state, componentMT);
}
