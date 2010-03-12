#ifndef __LOGGER__
#define __LOGGER__

#include <ostream>
#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string.h>

using namespace std;

class ErrorExit {};

extern "C" ErrorExit errorExit;

class NullStream : public ostream {
public:
  NullStream() : ostream(0) { }
};

extern "C" NullStream nullStream;

void operator << (void*, ErrorExit&);

#define inf cout
#define dbg cout
#define msg cout
#define err cerr
#define testDbg cout
#define deepDbg nullStream

#endif
