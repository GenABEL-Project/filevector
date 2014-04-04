#ifndef __ReusableFileHandleTest__
#define __ReusableFileHandleTest__

#include <cppunit/TestCase.h>
#include <cppunit/TestCaller.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/Asserter.h>

#include <string>
#include <cstring>

#include <stdio.h>

#include "../fvlib/AbstractMatrix.h"
#include "../fvlib/FileVector.h"
#include "../fvlib/Transposer.h"
#include "TestUtil.h"

using namespace std;

class ReusableFileHandleTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( ReusableFileHandleTest );
    CPPUNIT_TEST( testRandAccess );
    CPPUNIT_TEST_SUITE_END();

public:

    void testRandAccess();
};

#endif