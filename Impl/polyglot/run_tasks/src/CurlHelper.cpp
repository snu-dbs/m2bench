//
// Created by Kyoseung Koo on 2021/05/05.
//

#include "../include/CurlHelper.h"

string CurlHelper::spaceReplace(const string &input) {
    // curl with url having whitespace causes error..
    ostringstream res;

    string::const_iterator end = input.end();
    for (string::const_iterator i = input.begin(); i != end; i++) {
        string::value_type c = *i;
        if (c == ' ') res << "%20";
        else res << c;
    }

    return res.str();
}

string CurlHelper::replace(const string &input) {
    // curl with url having whitespace causes error..
    ostringstream res;

    string::const_iterator end = input.end();
    for (string::const_iterator i = input.begin(); i != end; i++) {
        string::value_type c = *i;
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            res << c;
            continue;
        }

        // Any other characters are percent-encoded
        res << uppercase << '%' << setw(2) << int((unsigned char) c) << nouppercase;
    }

    return res.str();
}
