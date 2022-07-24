#ifndef _STR_SPLIT_HPP_
#define _STR_SPLIT_HPP_

#include <string>
#include <sstream>
#include <math.h>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;
void _split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;

    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}
vector<string> split(const string &s, char delim) {
    vector<string> elems;
    _split(s, delim, elems);
    return elems;
}

#endif //_STR_SPLIT_HPP_
