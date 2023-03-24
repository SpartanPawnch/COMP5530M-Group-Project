#pragma once
#include<string>
#include<vector>

namespace assetfolder {

    //Info struct for file browser
    struct AssetDescriptor {
        enum class EFileType {
            FOLDER,
            MODEL,
            TEXTURE,
            AUDIO,
            MISC,
            INVALID
        };

        //fully qualified path
        std::string path;

        //name of file
        std::string name;
        EFileType type;
    };

    //set active directory for asset manager
    void setActiveDirectory(std::string dir);

    //get entry pointing to root assets folder
    AssetDescriptor getRootDir();

    //list items in directory given. dir.type must be EFileType::FOLDER
    void listDir(const AssetDescriptor& dir, std::vector<AssetDescriptor>& res);

    //copy asset in path to folder specified
    void addAsset(const char* path, const AssetDescriptor& folder);

    //delete asset
    void delAsset(const AssetDescriptor& asset);
}