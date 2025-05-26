#ifndef _UTIL_STD_MOVE_H
#define _UTIL_STD_MOVE_H

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

std::string&& std_move(std::string&& _x) {
    return std::move(_x);
}

template<typename T> 
std::vector<T>&& std_move(std::vector<T>&& _x) {
    return _x;
}

template<typename T1, typename T2> 
std::unordered_map<T1, T2>&& std_move(std::unordered_map<T1, T2>&& _x) {
    return _x;
}

template<typename T> 
std::unique_ptr<T>&& std_move(std::unique_ptr<T>&& _x) {
    return _x;
}

template<typename T> 
std::shared_ptr<T>&& std_move(std::shared_ptr<T>&& _x) {
    return _x;
}

#define STD_MOVE(X) std_move(std::move(X))

#endif
