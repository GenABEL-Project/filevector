#include <cppunit/TestCase.h>

class ReadVariableTest : public CppUnit::TestCase { 
public: 
  ReadVariableTest( ) : CppUnit::TestCase(  ) {}
    
  void runTest() {
    CPPUNIT_ASSERT( 1 == 2 );
  }
};
                