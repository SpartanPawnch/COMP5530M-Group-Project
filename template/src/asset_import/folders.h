#pragma once
#include <string>
#include <vector>

namespace assetfolder {

    // Info struct for file browser
    struct AssetDescriptor {
        enum class EFileType {
            FOLDER,
            MODEL,
            TEXTURE,
            AUDIO,
            SCRIPT,
            MISC,
            INVALID
        };

        // fully qualified path
        std::string path;

        // name of file
        std::string name;
        EFileType type;
    };

    // set active directory for asset manager
    void setActiveDirectory(std::string dir);

    // get entry pointing to root assets folder
    AssetDescriptor getAssetsRootDir();

    // get entry pointing to levels folder
    AssetDescriptor getLevelsRootDir();

    // get entry pointing to ui folder
    AssetDescriptor getUIRootDir();

    // list items in directory given. dir.type must be EFileType::FOLDER
    void listDir(const AssetDescriptor& dir, std::vector<AssetDescriptor>& res);

    // recursively search for specific file type in asset folder
    void findAssetsByType(AssetDescriptor::EFileType type, std::vector<AssetDescriptor>& res);

    // go to outer directory
    AssetDescriptor outerDir(const AssetDescriptor& dir);

    // copy asset in path to folder specified
    void addAsset(const std::string& path, const AssetDescriptor& dir);
    void addAssets(const std::vector<std::string>& paths, const AssetDescriptor& dir);

    // delete asset
    void delAsset(const AssetDescriptor& asset);
    void delAssets(const std::vector<AssetDescriptor>& assets);

    // get type as string
    const char* typeToString(const AssetDescriptor::EFileType&);

    // get name from path
    std::string getName(const char* path);

    // get current active directory
    std::string getProjectRoot();

    // get relative path based on current path
    std::string getRelativePath(const char* path);

    std::string resolveExternalDependency(const char* path, const char* baseAssetDir);
}
