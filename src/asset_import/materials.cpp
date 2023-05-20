#include "materials.h"

MaterialSystem* MaterialSystem::getInstance() {
    if (instance == nullptr) {
        instance = new MaterialSystem();
    }
    return instance;
}

bool MaterialSystem::createMaterial(std::string name) {
    if (materials.count(name) > 0)
        return false;

    MATSYS::Material newMat;
    newMat.name = name;
    materials[name] = newMat;
    return true;
}

bool MaterialSystem::createMaterialDirectly(MATSYS::Material mat) {
    if (materials.count(mat.name) > 0)
        return false;

    materials[mat.name] = mat;
    return true;
}

std::string MaterialSystem::getSelectedMaterialName() {
    return materials[selectedMaterial].name;
}

MATSYS::Material* MaterialSystem::getMaterial(std::string name) {
    if (materials.count(name) > 0) {
        return &materials[name];
    }
    return nullptr;
}

MATSYS::Material* MaterialSystem::getSelectedMaterial() {
    return &materials[selectedMaterial];
}

void MaterialSystem::selectMaterial(std::string name) {
    selectedMaterial = name;
}

void MaterialSystem::setTextureFromPath(std::string path,
    std::shared_ptr<TextureDescriptor>& baseColorMap, std::string& uuid) {
    std::string localUuid = assetfolder::getRelativePath(path.c_str());
    auto desc = getTexture(localUuid);

    if (!desc) {
        // load file from disk
        desc = loadTexture(path.c_str(), localUuid);
    }

    std::swap(baseColorMap, desc);
    uuid = baseColorMap ? localUuid : "";
}

std::shared_ptr<ActiveMaterial> MaterialSystem::createActiveMaterial(const std::string& name) {
    if (materials.count(name) > 0 &&
        (activeMaterials.count(name) == 0 || activeMaterials[name].expired())) {
        MATSYS::Material mat = materials[name];
        ActiveMaterial activeMat;

        activeMat.name = mat.name;

        activeMat.baseColor = mat.baseColor;
        activeMat.emissiveColor = mat.emissiveColor;

        activeMat.roughness = mat.roughness;
        activeMat.metalness = mat.metalness;
        activeMat.occlusion = mat.occlusion;
        if (!mat.baseColorMap.empty())
            setTextureFromPath(mat.baseColorMap, activeMat.baseColorMap,
                activeMat.baseColorMapUuid);
        if (!mat.roughnessMap.empty())
            setTextureFromPath(mat.roughnessMap, activeMat.roughnessMap,
                activeMat.roughnessMapUuid);
        if (!mat.metalnessMap.empty())
            setTextureFromPath(mat.metalnessMap, activeMat.metalnessMap,
                activeMat.metalnessMapUuid);
        if (!mat.normalMap.empty())
            setTextureFromPath(mat.normalMap, activeMat.normalMap, activeMat.normalMapUuid);
        if (!mat.alphaMap.empty())
            setTextureFromPath(mat.alphaMap, activeMat.alphaMap, activeMat.alphaMapUuid);
        if (!mat.emissiveMap.empty())
            setTextureFromPath(mat.emissiveMap, activeMat.emissiveMap, activeMat.emissiveMapUuid);
        if (!mat.occlusionMap.empty())
            setTextureFromPath(mat.occlusionMap, activeMat.occlusionMap,
                activeMat.occlusionMapUuid);

        std::shared_ptr<ActiveMaterial> activeMatPtr = std::make_shared<ActiveMaterial>(activeMat);
        activeMaterials[name] = activeMatPtr;
        return activeMatPtr;
    }
    return nullptr;
}

void MaterialSystem::reloadActiveMaterial(const std::string& name) {
    if (activeMaterials.count(name) > 0) {
        MATSYS::Material mat = materials[name];
        std::shared_ptr<ActiveMaterial> activeMatPtr = activeMaterials[name].lock();

        if (activeMatPtr) {
            activeMatPtr->baseColor = mat.baseColor;
            activeMatPtr->emissiveColor = mat.emissiveColor;

            activeMatPtr->roughness = mat.roughness;
            activeMatPtr->metalness = mat.metalness;
            activeMatPtr->occlusion = mat.occlusion;

            setTextureFromPath(mat.baseColorMap, activeMatPtr->baseColorMap,
                activeMatPtr->baseColorMapUuid);
            setTextureFromPath(mat.roughnessMap, activeMatPtr->roughnessMap,
                activeMatPtr->roughnessMapUuid);
            setTextureFromPath(mat.metalnessMap, activeMatPtr->metalnessMap,
                activeMatPtr->metalnessMapUuid);
            setTextureFromPath(mat.normalMap, activeMatPtr->normalMap, activeMatPtr->normalMapUuid);
            setTextureFromPath(mat.alphaMap, activeMatPtr->alphaMap, activeMatPtr->alphaMapUuid);
            setTextureFromPath(mat.emissiveMap, activeMatPtr->emissiveMap,
                activeMatPtr->emissiveMapUuid);
            setTextureFromPath(mat.occlusionMap, activeMatPtr->occlusionMap,
                activeMatPtr->occlusionMapUuid);
        }
    }
}

std::shared_ptr<ActiveMaterial> MaterialSystem::getActiveMaterial(const std::string& name) {
    if (activeMaterials.count(name) == 0 || activeMaterials[name].expired()) {
        if (materials.count(name) > 0) {
            return createActiveMaterial(name);
        }
    }
    return activeMaterials[name].lock();
}

std::shared_ptr<ActiveMaterial> MaterialSystem::loadActiveMaterial(MATSYS::Material material) {
    if (materials.count(material.name) == 0) {
        materials[material.name] = material;
    }
    return getActiveMaterial(material.name);
}
