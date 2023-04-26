#pragma once
#include<string>
#include<vector>
// --- Utility wrapper for tinyfiledialogs ---
namespace fdutil {
    std::string openFile(char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPathAndFile, /*  NULL or "" */
        int const aNumOfFilterPatterns, /* 0 */
        char const* const* const aFilterPatterns, /* NULL or {"*.jpg","*.png"} */
        char const* const aSingleFilterDescription /* NULL or "image files" */
    ) /* 0 or 1 */;

    void openFileMulti(char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPathAndFile, /*  NULL or "" */
        int const aNumOfFilterPatterns, /* 0 */
        char const* const* const aFilterPatterns, /* NULL or {"*.jpg","*.png"} */
        char const* const aSingleFilterDescription, /* NULL or "image files" */
        std::vector<std::string>& res
    );

    std::string selectFolder(
        char const* const aTitle, /*  NULL or "" */
        char const* const aDefaultPath
    );
}