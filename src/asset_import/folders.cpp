#include "folders.h"
#ifdef _WIN32
#include<Windows.h>
#else
//TODO Linux version?
#endif
#include <cstring>
#include <cassert>

static const char* activeDirectory = NULL;

namespace assetfolder {
    //extract file name from path
    static std::string getName(const char* path) {
        int i = strlen(path) - 1;
        while (i > 0 && path[i] != '/' && path[i] != '\\')
            i--;
        return std::string((path + i + (i != 0)));
    }

    //extract file extension from path
    static std::string getExtension(const char* path) {
        int i = strlen(path) - 1;
        while (i > 0 && path[i] != '.' && path[i] != '/' && path[i] != '\\')
            i--;
        if (path[i] == '.')
            return std::string((path + i));
        else
            return std::string("");
    }

    //find file type from path
    static AssetDescriptor::EFileType getType(const char* path) {
        std::string ext = getExtension(path);

        //models
        if (ext == ".obj" || ext == ".fbx" || ext == ".dae" || ext == ".gltf")
            return AssetDescriptor::EFileType::MODEL;

        //textures
        if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga" || ext == ".hdr")
            return AssetDescriptor::EFileType::TEXTURE;

        //audio
        if (ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".wav")
            return AssetDescriptor::EFileType::AUDIO;

        return AssetDescriptor::EFileType::MISC;
    }

    static AssetDescriptor::EFileType getType(const std::string& path) {
        std::string ext = getExtension(path.c_str());

        //models
        if (ext == ".obj" || ext == ".fbx" || ext == ".dae" || ext == ".gltf")
            return AssetDescriptor::EFileType::MODEL;

        //textures
        if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga" || ext == ".hdr")
            return AssetDescriptor::EFileType::TEXTURE;

        //audio
        if (ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".wav")
            return AssetDescriptor::EFileType::AUDIO;

        return AssetDescriptor::EFileType::MISC;
    }

    void setActiveDirectory(const char* dir) {
        activeDirectory = dir;
    }

    AssetDescriptor getRootDir() {
        if (activeDirectory == NULL)
            return AssetDescriptor{
            std::string(""),
            std::string(""),
            AssetDescriptor::EFileType::INVALID
        };

        return AssetDescriptor{
            std::string(activeDirectory) + "/assets",
            std::string("assets"),
            AssetDescriptor::EFileType::FOLDER
        };
    }

    void listDir(const AssetDescriptor& dir, std::vector<AssetDescriptor>& res) {
        assert(dir.type == AssetDescriptor::EFileType::FOLDER);
        res.resize(0);

#ifdef _WIN32
        WIN32_FIND_DATA findData;
        HANDLE hFind;
        hFind = FindFirstFileA((dir.path + "/*").c_str(), &findData);
        if (hFind != 0) {
            //add first result
            res.emplace_back(AssetDescriptor{
                std::string(dir.path + findData.cFileName),
                std::string(findData.cFileName),
                findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ?
                    AssetDescriptor::EFileType::FOLDER : getType(findData.cFileName)
                });

            //add other results
            for (bool found = FindNextFileA(hFind, &findData);found;
                found = FindNextFileA(hFind, &findData)) {
                res.emplace_back(AssetDescriptor{
                std::string(dir.path + findData.cFileName),
                std::string(findData.cFileName),
                findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ?
                    AssetDescriptor::EFileType::FOLDER : getType(findData.cFileName)
                    });
            }
        }
#else
        //TODO - Linux Version?
#endif
    }

    void addAsset(const char* path, const AssetDescriptor& dir) {
        assert(dir.type == AssetDescriptor::EFileType::FOLDER);
    }

    void delAsset(const AssetDescriptor& asset) {

    }
}