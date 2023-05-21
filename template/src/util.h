#pragma once
#include <string>
#include <vector>

// set all elements to specific value
template <typename T> inline void setVec(std::vector<T>& vec, T val) {
    for (unsigned int i = 0; i < vec.size(); i++) {
        vec[i] = val;
    }
}

// get folder from path to file
inline std::string getFolder(const std::string& path) {
    int i = int(path.length() - 1);
    while (i > 0 && path[i] != '/' && path[i] != '\\')
        i--;

    return path.substr(0, i);
}
