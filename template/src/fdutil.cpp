#include "fdutil.h"

#include<thread>

#include "../external/tinyfiledialogs/tinyfiledialogs.h"
namespace fdutil {

    std::string openFile(char const* const aTitle, char const* const aDefaultPathAndFile,
        int const aNumOfFilterPatterns, char const* const* const aFilterPatterns,
        char const* const aSingleFilterDescription) {

        const char* path = tinyfd_openFileDialog(aTitle, aDefaultPathAndFile, aNumOfFilterPatterns,
            aFilterPatterns, aSingleFilterDescription, 0);

        std::string str("");
        if (path) {
            str = std::string(path);
        }
        return str;
    }

    void openFileMulti(char const* const aTitle, char const* const aDefaultPathAndFile,
        int const aNumOfFilterPatterns, char const* const* const aFilterPatterns,
        char const* const aSingleFilterDescription, std::vector<std::string>& res
    ) {
        //get files
        const char* path = tinyfd_openFileDialog(aTitle, aDefaultPathAndFile, aNumOfFilterPatterns,
            aFilterPatterns, aSingleFilterDescription, 1);

        res.resize(0);
        if (path) {
            //tokenize string
            const char* begin = path;
            const char* p = path;
            size_t len = strlen(path);
            while (p < path + len) {
                if (*p == '|') {
                    res.emplace_back(std::string(begin, p));
                    p++;
                    begin = p;
                }
                p++;
            }
            res.emplace_back(std::string(begin, path + len));
        }
    }

    std::string selectFolder(
        char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPath) {
        const char* path = tinyfd_selectFolderDialog(aTitle, aDefaultPath);
        std::string str("");
        if (path)
            str = std::string(path);
        return str;
    }
}