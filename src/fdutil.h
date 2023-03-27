#pragma once
#include<string>
// --- Utility wrapper for tinyfiledialogs ---
namespace fdutil {
    std::string openFile(char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPathAndFile, /*  NULL or "" */
        int const aNumOfFilterPatterns, /* 0 */
        char const* const* const aFilterPatterns, /* NULL or {"*.jpg","*.png"} */
        char const* const aSingleFilterDescription, /* NULL or "image files" */
        int const aAllowMultipleSelects) /* 0 or 1 */;

    std::string selectFolder(
        char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPath);
}