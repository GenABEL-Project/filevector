#include "Logger.h"

using namespace std;

ErrorExit errorExit;
NullStream nullStream;

void operator << (void*, ErrorExit&) {
#ifdef R_R_H
   throw 1;
#else
	exit(EXIT_FAILURE);
#endif
}


