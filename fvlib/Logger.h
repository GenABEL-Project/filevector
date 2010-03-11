#ifndef __LOGGER__
#define __LOGGER__

#include <ostream>
#include <iostream>
#include <cstdlib>

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
#define err cerr
#define msg cout
#define testDbg cout
#define deepDbg nullStream

#endif
