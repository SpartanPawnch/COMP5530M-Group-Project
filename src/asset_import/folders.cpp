#include "folders.h"
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
// TODO Linux version?
#endif
#include <cstring>
#include <cassert>
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include "../logging.h"

// disable POSIX deprecation warning
#pragma warning(disable : 4996)

static std::string activeDirectory;

namespace assetfolder {
    // extract file name from path
    std::string getName(const char* path) {
        int i = strlen(path) - 1;
        while (i > 0 && path[i] != '/' && path[i] != '\\')
            i--;
        return std::string((path + i + (i != 0)));
    }

    // extract file extension from path
    static std::string getExtension(const char* path) {
        int i = strlen(path) - 1;
        while (i > 0 && path[i] != '.' && path[i] != '/' && path[i] != '\\')
            i--;
        if (path[i] == '.')
            return std::string((path + i));
        else
            return std::string("");
    }

    // find file type from path
    static AssetDescriptor::EFileType getType(const char* path) {
        std::string ext = getExtension(path);

        // models
        if (ext == ".obj" || ext == ".fbx" || ext == ".dae" || ext == ".gltf" || ext == ".iqm")
            return AssetDescriptor::EFileType::MODEL;

        // textures
        if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga" || ext == ".hdr")
            return AssetDescriptor::EFileType::TEXTURE;

        // audio
        if (ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".wav")
            return AssetDescriptor::EFileType::AUDIO;

        // script
        if (ext == ".lua")
            return AssetDescriptor::EFileType::SCRIPT;

        return AssetDescriptor::EFileType::MISC;
    }

    static AssetDescriptor::EFileType getType(const std::string& path) {
        std::string ext = getExtension(path.c_str());

        // models
        if (ext == ".obj" || ext == ".fbx" || ext == ".dae" || ext == ".gltf")
            return AssetDescriptor::EFileType::MODEL;

        // textures
        if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".tga" || ext == ".hdr")
            return AssetDescriptor::EFileType::TEXTURE;

        // audio
        if (ext == ".mp3" || ext == ".ogg" || ext == ".flac" || ext == ".wav")
            return AssetDescriptor::EFileType::AUDIO;

        return AssetDescriptor::EFileType::MISC;
    }

    void setActiveDirectory(std::string dir) {
        activeDirectory = dir;
    }

    AssetDescriptor getAssetsRootDir() {
        if (activeDirectory.empty())
            return AssetDescriptor{std::string(""), std::string(""),
                AssetDescriptor::EFileType::INVALID};

        return AssetDescriptor{std::string(activeDirectory) + "/assets", std::string("assets"),
            AssetDescriptor::EFileType::FOLDER};
    }

    AssetDescriptor getLevelsRootDir() {
        if (activeDirectory.empty())
            return AssetDescriptor{std::string(""), std::string(""),
                AssetDescriptor::EFileType::INVALID};

        return AssetDescriptor{std::string(activeDirectory) + "/ui-layouts", std::string("Uiname"),
            AssetDescriptor::EFileType::FOLDER};
    }

        AssetDescriptor getUIRootDir() {
        if (activeDirectory.empty())
            return AssetDescriptor{std::string(""), std::string(""),
                AssetDescriptor::EFileType::INVALID};

        return AssetDescriptor{std::string(activeDirectory) + "/levels", std::string("levels"),
            AssetDescriptor::EFileType::FOLDER};
    }

    void listDir(const AssetDescriptor& dir, std::vector<AssetDescriptor>& res) {
        assert(dir.type == AssetDescriptor::EFileType::FOLDER);
        res.resize(0);

#ifdef _WIN32
        WIN32_FIND_DATA findData;
        HANDLE hFind;
        hFind = FindFirstFileA((dir.path + "/*").c_str(), &findData);
        if (hFind != 0) {
            // add first result
            if (strcmp(findData.cFileName, ".") != 0 && strcmp(findData.cFileName, "..") != 0) {
                res.emplace_back(AssetDescriptor{std::string(dir.path + findData.cFileName),
                    std::string(findData.cFileName),
                    findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                        ? AssetDescriptor::EFileType::FOLDER
                        : getType(findData.cFileName)});
            }

            // add other results
            for (bool found = FindNextFileA(hFind, &findData); found;
                 found = FindNextFileA(hFind, &findData)) {
                // skip "." and ".."
                if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
                    continue;

                res.emplace_back(AssetDescriptor{std::string(dir.path + "/" + findData.cFileName),
                    std::string(findData.cFileName),
                    findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
                        ? AssetDescriptor::EFileType::FOLDER
                        : getType(findData.cFileName)});
            }
        }
#else
        // TODO - Linux Version?
#endif

        // bubble sort for folders first
        for (int i = res.size() - 1; i > 0; i--) {
            for (int j = 0; j < i; j++) {
                if (res[j].type != AssetDescriptor::EFileType::FOLDER &&
                    res[j + 1].type == AssetDescriptor::EFileType::FOLDER)
                    std::swap(res[j], res[j + 1]);
            }
        }
    }

    static void findAssetsByTypeInner(AssetDescriptor::EFileType type, const AssetDescriptor& root,
        std::vector<AssetDescriptor>& res) {
        std::vector<AssetDescriptor> contents;
        listDir(root, contents);
        for (unsigned int i = 0; i < contents.size(); i++) {
            // search recursively
            if (contents[i].type == AssetDescriptor::EFileType::FOLDER)
                findAssetsByTypeInner(type, contents[i], res);

            // add relevant results
            if (contents[i].type == type)
                res.push_back(contents[i]);
        }
    }

    void findAssetsByType(AssetDescriptor::EFileType type, std::vector<AssetDescriptor>& res) {
        res.resize(0);
        findAssetsByTypeInner(type, getAssetsRootDir(), res);
    }

    AssetDescriptor outerDir(const AssetDescriptor& dir) {
        int i = dir.path.length() - 1;
        while (i > 0 && dir.path[i] != '/' && dir.path[i] != '\\')
            i--;

        // reached topmost folder
        if (i == 0)
            return dir;

        std::string newPath = dir.path.substr(0, i);
        return AssetDescriptor{newPath, getName(newPath.c_str()),
            AssetDescriptor::EFileType::FOLDER};
    }

    static void addAssets_internal(const std::vector<AssetDescriptor>& assets,
        const AssetDescriptor& dir) {
        for (unsigned int i = 0; i < assets.size(); i++) {
            addAsset(assets[i].path, dir);
        }
    }

    void addAsset(const std::string& path, const AssetDescriptor& dir) {
        assert(dir.type == AssetDescriptor::EFileType::FOLDER);

        std::string name = getName(path.c_str());
        std::string newPath = dir.path + "/" + name;
        size_t bytesRead;

        // check if file is directory
        struct stat s;
        if (stat(path.c_str(), &s) != 0)
            return;

        if (s.st_mode & S_IFDIR) {
            // directory

            // create folder in destination
            _mkdir(newPath.c_str());
            AssetDescriptor dirDesc = {path, name, AssetDescriptor::EFileType::FOLDER};

            // get source contents
            std::vector<AssetDescriptor> contents;
            listDir(dirDesc, contents);

            // copy source contents
            dirDesc.path = newPath;
            addAssets_internal(contents, dirDesc);
        }
        else if (s.st_mode & S_IFREG) {
            // file - copy using POSIX descriptors
            char buf[BUFSIZ];
            int src = _open(path.c_str(), O_RDONLY | O_BINARY, 0);
            if (src == NULL) {
                logging::logErr("Failed to open file {} for reading\n", path.c_str());
                return;
            }

            int dst = _open(newPath.c_str(), O_WRONLY | O_CREAT | O_BINARY, _S_IWRITE | _S_IREAD);
            if (dst == NULL) {
                logging::logErr("Failed to open file {} for writing\n", newPath.c_str());
                return;
            }

            while ((bytesRead = _read(src, buf, BUFSIZ)) > 0)
                _write(dst, buf, BUFSIZ);

            _close(src);
            _close(dst);
        }
    }

    void addAssets(const std::vector<std::string>& paths, const AssetDescriptor& dir) {
        for (unsigned int i = 0; i < paths.size(); i++) {
            addAsset(paths[i], dir);
        }
    }

    void delAsset(const AssetDescriptor& asset) {
        // remove directory
        if (asset.type == AssetDescriptor::EFileType::FOLDER) {
            std::vector<AssetDescriptor> contents;
            listDir(asset, contents);
            delAssets(contents);
            _rmdir(asset.path.c_str());
            return;
        }

        // remove single file
        int res = std::remove(asset.path.c_str());
        // TODO log
        if (res != 0)
            logging::logErr("Failed to delete asset {}, got error {}\n", asset.path.c_str(), res);
    }

    void delAssets(const std::vector<AssetDescriptor>& assets) {
        for (unsigned int i = 0; i < assets.size(); i++)
            delAsset(assets[i]);
    }
    const char* typeToString(const AssetDescriptor::EFileType& type) {
        const char* literals[] = {"FOLDER", "MODEL", "TEXTURE", "AUDIO", "MISC", "INVALID"};
        return literals[int(type)];
    }

    std::string getProjectRoot() {
        return activeDirectory;
    }

    std::string getRelativePath(const char* path) {
        if (strlen(path) < activeDirectory.length() + 1) {
            // logging::logWarn("Path {} is outside project\n", path);
            return std::string(path);
        }
        return std::string(path + activeDirectory.length() + 1);
    }

    std::string resolveExternalDependency(const char* path, const char* baseAssetDir) {
        std::string assetpath(baseAssetDir);
        // check if path is relative
        if ((strlen(path) <= 1 || path[1] != ':') &&
            GetFileAttributesA((assetpath + path).c_str()) != INVALID_FILE_ATTRIBUTES)
            return assetpath + std::string(path);

        // check if path is local to folder
        if (assetpath.length() < strlen(path)) {
            bool local = true;
            for (size_t i = 0; i < assetpath.length() && local; i++) {
                if (assetpath[i] != path[i])
                    local = false;
            }
            if (local)
                return std::string(path);
        }

        // try to resolve folder structure
        for (int offset = strlen(path) - 1; offset >= 0; offset--) {
            if (path[offset] == '/' || path[offset] == '\\') {
                std::string subpath = std::string(path + offset);
                if (GetFileAttributesA((assetpath + subpath).c_str()) != INVALID_FILE_ATTRIBUTES) {
                    // logging::logInfo("Path {} is not local to project, resolved to {}\n", path,
                    // assetpath + subpath);
                    return (assetpath + subpath);
                }
            }
        }

        logging::logWarn("ASSET WARNING: Path {} is not found in base asset folder {} and"
                         " will not load on other machines. Consider importing\n",
            path, baseAssetDir);
        return std::string(path);
    }
}
