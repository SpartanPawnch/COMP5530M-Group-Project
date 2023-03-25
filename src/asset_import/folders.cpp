#include "folders.h"
#ifdef _WIN32
#include<windows.h>
#include<direct.h>
#else
//TODO Linux version?
#endif
#include <cstring>
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

//disable POSIX deprecation warning
#pragma warning(disable:4996)

static std::string activeDirectory;

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

    void setActiveDirectory(std::string dir) {
        activeDirectory = dir;
    }

    AssetDescriptor getRootDir() {
        if (activeDirectory.empty())
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
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                res.emplace_back(AssetDescriptor{
                    std::string(dir.path + findData.cFileName),
                    std::string(findData.cFileName),
                    findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ?
                        AssetDescriptor::EFileType::FOLDER : getType(findData.cFileName)
                    });
            }

            //add other results
            for (bool found = FindNextFileA(hFind, &findData);found;
                found = FindNextFileA(hFind, &findData)) {
                if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                    res.emplace_back(AssetDescriptor{
                    std::string(dir.path + "/" + findData.cFileName),
                    std::string(findData.cFileName),
                    findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ?
                        AssetDescriptor::EFileType::FOLDER : getType(findData.cFileName)
                        });
                }
            }
        }
#else
        //TODO - Linux Version?
#endif
    }

    static void addAssets_internal(const std::vector<AssetDescriptor>& assets, const AssetDescriptor& dir) {
        for (unsigned int i = 0;i < assets.size();i++) {
            addAsset(assets[i].path, dir);
        }
    }

    void addAsset(const std::string& path, const AssetDescriptor& dir) {
        assert(dir.type == AssetDescriptor::EFileType::FOLDER);

        std::string name = getName(path.c_str());
        std::string newPath = dir.path + "/" + name;
        size_t bytesRead;

        //check if file is directory
        struct stat s;
        if (stat(path.c_str(), &s) != 0)
            return;

        if (s.st_mode & S_IFDIR) {
            //directory

            //create folder in destination
            _mkdir(newPath.c_str());
            AssetDescriptor dirDesc = {
                path,
                name,
                AssetDescriptor::EFileType::FOLDER
            };

            //get source contents
            std::vector<AssetDescriptor> contents;
            listDir(dirDesc, contents);

            //copy source contents
            dirDesc.path = newPath;
            addAssets_internal(contents, dirDesc);
        }
        else if (s.st_mode & S_IFREG) {
            //file - copy using POSIX descriptors
            char buf[BUFSIZ];
            int src = _open(path.c_str(), O_RDONLY, 0);
            if (src == NULL) {
                printf("Failed to open file %s for reading\n", path.c_str());
                return;
            }

            int dst = _open(newPath.c_str(), O_WRONLY | O_CREAT, 0644);
            if (dst == NULL) {
                printf("Failed to open file %s for writing\n", newPath.c_str());
                return;
            }

            while ((bytesRead = _read(src, buf, BUFSIZ)) > 0)
                _write(dst, buf, BUFSIZ);

            _close(src);
            _close(dst);
        }
    }

    void addAssets(const std::vector<std::string>& paths, const AssetDescriptor& dir) {
        for (unsigned int i = 0;i < paths.size();i++) {
            addAsset(paths[i], dir);
        }
    }

    void delAsset(const AssetDescriptor& asset) {
        int res = std::remove(asset.path.c_str());

        //TODO log
        if (res != 0)
            printf("Failed to delete asset %s, got error %i\n", asset.path.c_str(), res);
    }

    void delAssets(const std::vector<AssetDescriptor>& assets) {
        for (unsigned int i = 0;i < assets.size();i++)
            delAsset(assets[i]);
    }
    const char* typeToString(const AssetDescriptor::EFileType& type) {
        const char* literals[] = {
            "FOLDER",
            "MODEL",
            "TEXTURE",
            "AUDIO",
            "MISC",
            "INVALID"
        };
        return literals[int(type)];
    }
}