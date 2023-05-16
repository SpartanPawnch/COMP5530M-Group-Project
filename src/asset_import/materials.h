#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include <glm/glm.hpp>
#include "images.h"
#include "folders.h"

struct Material {
    std::string name="";

    glm::vec3 baseColor = glm::vec3(0.2f);
    glm::vec3 emissiveColor = glm::vec3(0.0f);

    float roughness = 1.0f;
    float metalness = 0.0f;
    float occlusion = 0.5f;

    std::string baseColorMap= std::string("");
    std::string roughnessMap = std::string("");
    std::string metalnessMap = std::string("");
    std::string normalMap = std::string("");
    std::string alphaMap = std::string("");
    std::string emissiveMap = std::string("");
    std::string occlusionMap = std::string("");
};

struct ActiveMaterial {
    std::string name="";

    glm::vec3 baseColor = glm::vec3(0.2f);
    glm::vec3 emissiveColor = glm::vec3(0.0f);

    float roughness = 1.0f;
    float metalness = 0.0f;
    float occlusion = 0.5f;

    std::shared_ptr<TextureDescriptor> baseColorMap;
    std::string baseColorMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> roughnessMap;
    std::string roughnessMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> metalnessMap;
    std::string metalnessMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> normalMap;
    std::string normalMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> alphaMap;
    std::string alphaMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> emissiveMap;
    std::string emissiveMapUuid = std::string("");
    std::shared_ptr<TextureDescriptor> occlusionMap;
    std::string occlusionMapUuid = std::string("");
};

class MaterialSystem {
public:
    static MaterialSystem* getInstance();

    bool createMaterial(std::string name);
    bool createMaterialDirectly(Material mat);
    std::string getSelectedMaterialName();
    Material* getMaterial(std::string name);
    Material* getSelectedMaterial();
    void selectMaterial(std::string name);

    std::shared_ptr<ActiveMaterial> getActiveMaterial(const std::string& name);
    std::shared_ptr<ActiveMaterial> loadActiveMaterial(Material material);
    void setTextureFromPath(std::string path, std::shared_ptr<TextureDescriptor>& baseColorMap, std::string& uuid);
    std::shared_ptr<ActiveMaterial> createActiveMaterial(const std::string& name);
    void reloadActiveMaterial(const std::string& name);

    std::map<std::string, Material> materials;
    std::string selectedMaterial = "";

//private:
    static MaterialSystem* instance;
    MaterialSystem() {};
    MaterialSystem(const MaterialSystem&) = delete;
    MaterialSystem& operator=(const MaterialSystem&) = delete;

    std::map<std::string, std::weak_ptr<ActiveMaterial>> activeMaterials;
};