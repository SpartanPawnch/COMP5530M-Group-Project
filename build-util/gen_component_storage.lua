-- get working directory
local workdir = io.popen("cd"):read()
workdir = string.gsub(workdir, "\\", "\\\\")

-- get timestamp for component list
local dateModCmd = 'wmic datafile where Name="'
	.. workdir
	.. '\\\\src\\\\ECS\\\\Component\\\\component_list.txt" '
	.. "get LastModified"
local dateModHndl = io.popen(dateModCmd)

local datemodified = ""

if dateModHndl then
	-- ignore first line
	_ = dateModHndl:read()

	-- get date modified
	datemodified = dateModHndl:read()

	dateModHndl:close()
end
-- check if we need to do anything
local cacheFile = io.open("src/ECS/ComponentStorage/codegen.cache", "r")
if cacheFile then
	local dt = cacheFile:read()
	cacheFile:close()
	-- cache matches - do nothing
	if dt == datemodified and datemodified ~= "" then
		print("Codegen cache up to date")
		return
	end
end

local complistFile = io.open("src/ECS/Component/component_list.txt", "r")
assert(complistFile ~= nil, "Failed to open component_list.txt")
local types = {}
for line in complistFile:lines() do
	table.insert(types, line)
end

complistFile:close()

-- create header file
local headerFile = io.open("src/ECS/ComponentStorage/ComponentStorage.h", "w")
assert(headerFile, "Failed to open ComponentStorage.h")

-- static includes
headerFile:write([[
#pragma once

#include <vector>

#include <lua.hpp>

#include "../EntityState/EntityState.h"
#include "../ComponentLocation/ComponentLocation.h"

]])

-- generated includes
for _, type in ipairs(types) do
	headerFile:write('#include "../Component/' .. type .. '.h"\n')
end

-- begin struct
headerFile:write([[

struct ComponentStorage{
]])

-- generate vectors
for _, type in ipairs(types) do
	headerFile:write("    std::vector<" .. type .. "> vec" .. type .. ";\n")
end

-- define methods
headerFile:write([[
    //add component, type is inferred by compiler
    template<typename T>
    void addComponent(const T& component);

    //start all components of specific type
    template<typename T>
    void start();

    //update all components of specific type
    template<typename T>
    void update(float dt,EntityState& state);

    //call start for all types
    void startAll();

    //call update for all types
    void updateAll(float dt,EntityState& state);
    
    //clear all components
    void clearAll();

    //get raw pointer using component loc
    void* getProtectedPtr(const ComponentLocation& loc);

    //register metatables
    static void registerMetatables();

    //push lua table
    static void pushLuaTable(void* ptr, const ComponentLocation& loc, lua_State* state);

    //get CompType enum based on type
    template<typename T>
    static ComponentLocation::CompType typeToCompTypeEnum();

    // --- Template Specializations ---
]])

-- specialize add methods
for _, type in ipairs(types) do
	headerFile:write([[
    template<>
    void addComponent<]] .. type .. [[>(const ]] .. type .. [[& component){
        vec]] .. type .. [[.emplace_back(component);
    }

]])
end

-- specialize start methods
for _, type in ipairs(types) do
	headerFile:write([[
    template<>
    void start<]] .. type .. [[>(){
        for(unsigned int i=0;i<vec]] .. type .. [[.size();i++){
            vec]] .. type .. [[[i].start();
        }
    }

]])
end

-- specialize update methods
for _, type in ipairs(types) do
	headerFile:write([[
    template<>
    void update<]] .. type .. [[>(float dt,EntityState& state){
        for(unsigned int i=0;i<vec]] .. type .. [[.size();i++){
            vec]] .. type .. [[[i].update(dt,state);
        }
    }

]])
end

-- specialize typeToEnum methods
for _, type in ipairs(types) do
	headerFile:write([[
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<]] .. type .. [[>(){
        return ComponentLocation::]] .. string.upper(type) .. [[;
    }
]])
end

-- end struct
headerFile:write("};")
headerFile:close()

--create source file

local sourceFile = io.open("src/ECS/ComponentStorage/ComponentStorage.cpp", "w")
assert(sourceFile, "Failed to open ComponentStorage.cpp for writing")

-- add static includes
sourceFile:write([[
#include "ComponentStorage.h"

]])

-- create startAll method
sourceFile:write([[
void ComponentStorage::startAll(){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    start<]] .. type .. [[>();
]])
end
sourceFile:write("}\n")

-- create updateAll method
sourceFile:write([[
void ComponentStorage::updateAll(float dt, EntityState& state){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    update<]] .. type .. [[>(dt,state);
]])
end
sourceFile:write("}\n")

-- create clearAll method
sourceFile:write([[
void ComponentStorage::clearAll(){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    vec]] .. type .. [[.clear();
]])
end

sourceFile:write("}\n")

-- create registerMetatables method
sourceFile:write([[
void ComponentStorage::registerMetatables(){
]])

for _, type in ipairs(types) do
	sourceFile:write("    " .. type .. [[::registerLuaMetatable();
]])
end

sourceFile:write("}\n")

-- create getProtectedPtr method
sourceFile:write([[
void* ComponentStorage::getProtectedPtr(const ComponentLocation& loc){
    switch(loc.type){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    case ComponentLocation::]] .. string.upper(type) .. [[:
        if(loc.componentIdx>=vec]] .. type .. [[.size())
            return nullptr;
        return &vec]] .. type .. "[loc.componentIdx]" .. [[;
]])
end

sourceFile:write([[
    default:;
    }
    return nullptr;
}
]])

-- create pushLuaTable method
sourceFile:write([[
void ComponentStorage::pushLuaTable(void* ptr,const ComponentLocation& loc,lua_State* state){
    switch(loc.type){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    case ComponentLocation::]] .. string.upper(type) .. [[:
        if(ptr==nullptr)
            break;
        ((]] .. type .. "*)ptr)->" .. [[pushLuaTable(state);
        return;
]])
end

sourceFile:write([[
    default:;
    }
    lua_pushnil(state);
    return;
}
]])

sourceFile:close()

local compLocationFile = io.open("src/ECS/ComponentLocation/ComponentLocation.h", "w")
assert(compLocationFile, "Failed to open ComponentLocation.h for writing")

compLocationFile:write([[
#pragma once
struct ComponentLocation{
    int entityUuid;
    int componentIdx;
    enum CompType{
]])
for _, type in ipairs(types) do
	compLocationFile:write([[
        ]] .. string.upper(type) .. ",\n")
end
compLocationFile:write([[
        COMPTYPE_MAX
    };
    CompType type;
};
]])
compLocationFile:close()

-- code generation for component selector gui
local guiStrings = io.open("src/gui-typestrings.generated", "w")
assert(guiStrings, "Failed to open gui-typestrings.generated for writing")
for i, type in ipairs(types) do
	guiStrings:write('"' .. type .. '"')
	if i < #types then
		guiStrings:write(",")
	end
end
guiStrings:close()

local guiCases = io.open("src/gui-typecases.generated", "w")
assert(guiCases, "Failed to open gui-typecases.generated for writing")
for _, type in ipairs(types) do
	guiCases:write([[
case ComponentLocation::]] .. string.upper(type) .. [[:
    drawComponentSelector(loc.componentIdx,storage.vec]] .. type .. [[);
    break;
]])
end
guiCases:close()

-- update cache
cacheFile = io.open("src/ECS/ComponentStorage/codegen.cache", "w")
if cacheFile then
	cacheFile:write(datemodified)
	cacheFile:close()
end
print("Generated source files for ComponentStorage struct!")
