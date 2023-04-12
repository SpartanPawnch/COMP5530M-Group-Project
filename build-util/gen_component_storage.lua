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

-- end struct
headerFile:write([[
    
    //add component, type is inferred by compiler
    template<typename T>
    void addComponent(T&& component);

    //update all components of specific type
    template<typename T>
    void update(float dt);

    //calls update for all types
    void updateAll(float dt);
    
    //clear all components
    void clearAll();
};
]])
headerFile:close()

--create source file

local sourceFile = io.open("src/ECS/ComponentStorage/ComponentStorage.cpp", "w")
assert(sourceFile, "Failed to open ComponentStorage.cpp for writing")

-- add static includes
sourceFile:write([[
#include "ComponentStorage.h"

]])

-- specialize add methods
for _, type in ipairs(types) do
	sourceFile:write([[
template<>
void ComponentStorage::addComponent<]] .. type .. [[>(]] .. type .. [[&& component){
    vec]] .. type .. [[.emplace_back(component);
}

]])
end

-- specialize update methods
for _, type in ipairs(types) do
	sourceFile:write([[
template<>
void ComponentStorage::update<]] .. type .. [[>(float dt){
    for(unsigned int i=0;i<vec]] .. type .. [[.size();i++){
        vec]] .. type .. [[[i].update(dt);
    }
}

]])
end

-- create updateAll method
sourceFile:write([[
void ComponentStorage::updateAll(float dt){
]])

for _, type in ipairs(types) do
	sourceFile:write([[
    update<]] .. type .. [[>(dt);
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

sourceFile:close()
