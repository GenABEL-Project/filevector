#include <map>
#include <string>

using namespace std;

#include "frutil.h"
#include "CastUtils.h"

void parseStringToArbType(string s, int destType, void *destData) {
    map<int, string> fmt;

    fmt[UNSIGNED_SHORT_INT] = string("%hu");
    fmt[SHORT_INT] = string("%sd");
    fmt[UNSIGNED_INT] = string("%ud");
    fmt[INT] = string("%d");
    fmt[FLOAT] = string("%f");
    fmt[DOUBLE] = string("%lf");

    string format = fmt[destType];

    sscanf(s.c_str(), format.c_str(), destData);
}
