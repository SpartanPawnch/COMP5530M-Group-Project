#pragma once

//set all elements to specific value
template<typename T>
void setVec(std::vector<T>& vec, T val) {
    for (unsigned int i = 0;i < vec.size();i++) {
        vec[i] = val;
    }
}