#include <map>

using namespace std;

#include "text2fvf.h"

unsigned short int dataTypeFromString(string type){
    if (type == "UNSIGNED_SHORT_INT") return 1;
    if (type == "SHORT_INT") return 2;
    if (type == "UNSIGNED_INT") return 3;
    if (type == "INT") return 4;
    if (type == "FLOAT") return 5;
    if (type == "DOUBLE") return 6;
    return 0;
}


#define REPORT_EVERY 10000
