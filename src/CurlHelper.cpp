//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "../include/CurlHelper.h"

std::string CurlHelper::spaceReplace(const std::string &input) {
    // curl with url having whitespace causes error..
    std::ostringstream res;

    std::string::const_iterator end = input.end();
    for (std::string::const_iterator i = input.begin(); i != end; i++) {
        std::string::value_type c = *i;
        if (c == ' ') {
            res << "%20";
            continue;
        }
        res << c;
    }

    return res.str();
}
