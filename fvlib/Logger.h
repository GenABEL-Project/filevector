#ifndef __LOGGER__
#define __LOGGER__

#include <iostream>

using namespace std;

class NewLine {};

class Logger {
    bool isEnabled;

public:
    Logger(bool enabled): isEnabled(enabled) {}

    Logger& operator << (const char * s) {
        if (isEnabled) cout << s;
        return *this;
    }

    Logger& operator << (string s) {
        if (isEnabled) cout << s.c_str();
        return *this;
    }

    Logger& operator << (unsigned long x) {
        if (isEnabled) cout << x;
        return *this;
    }

    Logger& operator << (unsigned int x) {
        if (isEnabled) cout << x;
        return *this;
    }

    Logger& operator << (int x) {
        if (isEnabled) cout << x;
        return *this;
    }

    Logger& operator << (float x) {
        if (isEnabled) cout << x;
        return *this;
    }

    Logger& operator << (NewLine){
        if (isEnabled) cout << endl;
        return *this;
    }
};

extern Logger dbg;
extern Logger inf;
extern Logger err;
extern NewLine nl;

#endif