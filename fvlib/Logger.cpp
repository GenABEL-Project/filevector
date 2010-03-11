#include "Logger.h"

using namespace std;

ErrorExit errorExit;
NullStream nullStream;

void operator << (void*, ErrorExit&) {
   exit(EXIT_FAILURE);
} 


