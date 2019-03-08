//
// Created by CmST0us on 2019/3/8.
//

#pragma once

#include <random>
#include <iostream>
namespace mavppm {

namespace utils {

inline std::string random_string() {
    std::random_device rd;
    std::mt19937 mt(rd());
    static std::string randomChar = "abcdefghijklmnopqrstuvwxyz1234567890";
    char randomStr[17] = {0};
    std::uniform_int_distribution<> dis(0, 35);
    for (int i = 0; i < 16; ++i) {
        int index = dis(mt);
        char c = randomChar[index];
        randomStr[i] = c;
    }
    return std::string(randomStr);
}

}

}