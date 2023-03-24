#include "fdutil.h"

#include<thread>

#include "../external/tinyfiledialogs/tinyfiledialogs.h"
namespace fdutil {

    std::string openFile(char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPathAndFile, /*  NULL or "" */
        int const aNumOfFilterPatterns, /* 0 */
        char const* const* const aFilterPatterns, /* NULL or {"*.jpg","*.png"} */
        char const* const aSingleFilterDescription, /* NULL or "image files" */
        int const aAllowMultipleSelects) /* 0 or 1 */ {
        const char* path = tinyfd_openFileDialog(aTitle, aDefaultPathAndFile, aNumOfFilterPatterns,
            aFilterPatterns, aSingleFilterDescription, aAllowMultipleSelects);

        std::string str("");
        if (path) {
            str = std::string(path);
        }
        return str;
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