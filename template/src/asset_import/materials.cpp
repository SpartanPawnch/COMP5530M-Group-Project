#include "materials.h"


MaterialSystem* MaterialSystem::getInstance() {
    if (instance == nullptr) {
        instance = new MaterialSystem();
    }
    return instance;
}

bool MaterialSystem::createMaterial(std::string name) {
    if (materials.count(name) > 0) return false;

    Material newMat;
    newMat.name = name;
    materials[name] = newMat;
    return true;
}

std::string MaterialSystem::getSelectedMaterialName() {
    return materials[selectedMaterial].name;
}

Material* MaterialSystem::getMaterial(std::string name) {
    if (materials.count(name) > 0) {
        return &materials[name];
    }
    return nullptr;
}

Material* MaterialSystem::getSelectedMaterial() {
    return &materials[selectedMaterial];
}

void MaterialSystem::selectMaterial(std::string name) {
    selectedMaterial = name;
}

void MaterialSystem::setTextureFromPath(std::string path, std::shared_ptr<TextureDescriptor>& baseColorMap, std::string& uuid) {
    std::string localUuid = assetfolder::getRelativePath(path.c_str());
    auto desc = getTexture(localUuid);

    if (!desc) {
        // load file from disk
        desc = loadTexture(path.c_str(), localUuid);
    }

    std::swap(baseColorMap, desc);
    uuid = baseColorMap ? localUuid : "";
}

void MaterialSystem::createActiveMaterial(const std::string& name) {
    if (materials.count(name) > 0 && (activeMaterials.count(name) == 0 || activeMaterials[name].expired())) {
        Material mat = materials[name];
        ActiveMaterial activeMat;

        activeMat.name = mat.name;

        activeMat.baseColor = mat.baseColor;
        activeMat.emissiveColor = mat.emissiveColor;

        activeMat.roughness = mat.roughness;
        activeMat.metalness = mat.metalness;
        activeMat.occlusion = mat.occlusion;

        setTextureFromPath(mat.baseColorMap, activeMat.baseColorMap, activeMat.baseColorMapUuid);
        setTextureFromPath(mat.roughnessMap, activeMat.roughnessMap, activeMat.roughnessMapUuid);
        setTextureFromPath(mat.metalnessMap, activeMat.metalnessMap, activeMat.metalnessMapUuid);
        setTextureFromPath(mat.normalMap, activeMat.normalMap, activeMat.normalMapUuid);
        setTextureFromPath(mat.alphaMap, activeMat.alphaMap, activeMat.alphaMapUuid);
        setTextureFromPath(mat.emissiveMap, activeMat.emissiveMap, activeMat.emissiveMapUuid);
        setTextureFromPath(mat.occlusionMap, activeMat.occlusionMap, activeMat.occlusionMapUuid);

        activeMaterials[name] = std::make_shared<ActiveMaterial>(activeMat);
    }
}

std::shared_ptr<ActiveMaterial> MaterialSystem::getActiveMaterial(const std::string& name) {
    if (activeMaterials.count(name) == 0 || activeMaterials[name].expired()) {
        if (materials.count(name) > 0) {
            createActiveMaterial(name);
        }
        else {
            return nullptr;
        }
    }
    std::shared_ptr<ActiveMaterial> ret = activeMaterials[name].lock();
    return ret;

}

std::shared_ptr<ActiveMaterial> MaterialSystem::loadActiveMaterial(Material material) {
    if (materials.count(material.name) == 0) {
        materials[material.name] = material;
    }
    return getActiveMaterial(material.name);
}


