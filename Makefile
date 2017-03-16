VERSION = 0.0-8
SRCDIR = fvfutil
LIBDIR = fvlib
TESTSDIR = tests
#DASRCDIR = DatABEL/src
BINDIR = bin
TEXT2FVF = $(BINDIR)/text2fvf
FV2TEXT = $(BINDIR)/fv2text
MERGEVARS = $(BINDIR)/mergevars
#RSHLIB = $(BINDIR)/filevector.so

READSPEED = $(BINDIR)/readspeed
CORRECTNESSTEST = $(BINDIR)/correctnesstest
ACCESSMODETEST = $(BINDIR)/accessmodetest
MODIFTEST = $(BINDIR)/modificationtest
CONVERTTEST = $(BINDIR)/converttest
CONVERT = $(BINDIR)/convert
WRITE_VAR_SPEED = $(BINDIR)/writevarspeed
TRANSPOSE = $(BINDIR)/transpose
BUILDTESTDATA = $(BINDIR)/buildtestdata
BUILDSUBMATRIX = $(BINDIR)/buildsubmatrix

UNITTEST = $(BINDIR)/unittest
TRANSPOSETEST = $(BINDIR)/transposetest
RFHTEST = $(BINDIR)/rfhtest

# GNU compiler
CPP = g++
# use for Solaris
# CPP = CC

CFLAGS = -I $(LIBDIR) -I $(SRCDIR) -g -Wall -D_NOT_R_FILEVECTOR #-m64

# Get the gcc version and check if the minor version number is above
# 7. Before gcc 4.7 the option for -std=c++11 was named -std=c++0x.
GCCVERSIONGTEQ47 := $(shell expr `gcc -dumpversion | cut -f2 -d.` \>= 7)
ifeq "$(GCCVERSIONGTEQ47)" "1"
    CFLAGS += -std=c++11
else
    CFLAGS += -std=c++0x
endif

CPPUNITFLAGS = -lcppunit
EXECS = $(TEXT2FVF) $(MERGEVARS) ${CONVERT}

LIBFILES = $(LIBDIR)/*.cpp $(LIBDIR)/*.h
REGFILES = $(SRCDIR)/usage.cpp $(SRCDIR)/usage.h

TESTSFILES = $(TESTSDIR)/*.cpp $(TESTSDIR)/*.h

BINARIES = converter text2fvf fv2text transpose writespeed # $(RSHLIB)

all: $(BINARIES)

$(BINARIES): | $(BINDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

text2fvf: $(TEXT2FVF)
$(TEXT2FVF): $(LIBFILES) $(REGFILES) $(SRCDIR)/text2fvf.cpp $(SRCDIR)/text2fvf_main.cpp $(SRCDIR)/usage.cpp $(SRCDIR)/text2fvf.h
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/usage.cpp $(SRCDIR)/text2fvf.cpp $(SRCDIR)/text2fvf_main.cpp -o $(TEXT2FVF)

fv2text: $(FV2TEXT)
$(FV2TEXT): $(LIBFILES) $(REGFILES) $(SRCDIR)/fv2text.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/fv2text.cpp -o $(FV2TEXT)

$(MERGEVARS): $(LIBFILES) $(REGFILES) $(SRCDIR)/mergevars.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/usage.cpp $(SRCDIR)/text2fvf.cpp $(SRCDIR)/mergevars.cpp -o $(MERGEVARS)

fvfutildis: clean
	cd ..; tar cvf filevector_$(VERSION).tar filevector_$(VERSION); gzip filevector_$(VERSION).tar

DatABELdis: clean
	cd ..; tar chvf DatABEL_$(VERSION).tar DatABEL_$(VERSION); gzip DatABEL_$(VERSION).tar

clean: tests-clean
	rm -rf $(BINDIR)/* $(SRCDIR)/*.o  $(LIBDIR)/*.o


correctnesstest : $(CORRECTNESSTEST)
$(CORRECTNESSTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/*.cpp $(TESTSDIR)/*.h
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(TESTSDIR)/CorrectnessTest.cpp ${TESTSDIR}/TestUtil.cpp ${CPPUNITFLAGS} -o $(CORRECTNESSTEST); $(CORRECTNESSTEST)

readspeed : $(READSPEED)
$(READSPEED): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/ReadSpeed.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(TESTSDIR)/ReadSpeed.cpp  -o $(READSPEED)

modificationtest : $(MODIFTEST)
$(MODIFTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/FileModificationTest.*
	$(CPP) $(CFLAGS) $(SRCDIR)/text2fvf.cpp $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/FileModificationTest.cpp $(CPPUNITFLAGS) -o $(MODIFTEST); $(MODIFTEST)

unittest : $(UNITTEST)
$(UNITTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/TestUtil.* $(TESTSDIR)/UnitTest.*
	$(CPP) $(CFLAGS) $(SRCDIR)/text2fvf.cpp $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/UnitTest.cpp $(CPPUNITFLAGS) -o $(UNITTEST); $(UNITTEST)

transposetest: $(TRANSPOSETEST)
$(TRANSPOSETEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/TransposeTest.cpp
	$(CPP) $(CFLAGS) $(SRCDIR)/text2fvf.cpp $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/TransposeTest.cpp $(CPPUNITFLAGS) -o $(TRANSPOSETEST); $(TRANSPOSETEST)

rfhtest: $(RFHTEST)
$(RFHTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/ReusableFileHandleTest.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/ReusableFileHandleTest.cpp $(CPPUNITFLAGS) -o $(RFHTEST); $(RFHTEST)

converter : $(CONVERT)
$(CONVERT): $(LIBFILES) $(SRCDIR)/convert.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(SRCDIR)/convert.cpp $(CPPUNITFLAGS) -o $(CONVERT)

transpose :$(TRANSPOSE)
$(TRANSPOSE) :  $(LIBFILES) $(SRCDIR)/transpose_main.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp  $(SRCDIR)/text2fvf.cpp $(SRCDIR)/transpose_main.cpp -o $(TRANSPOSE)


writespeed : $(WRITE_VAR_SPEED)
$(WRITE_VAR_SPEED): $(LIBFILES) $(SRCDIR)/convert.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(TESTSDIR)/WriteVariablesSpeed.cpp $(CPPUNITFLAGS) -o $(WRITE_VAR_SPEED)

accessmodetest : $(ACCESSMODETEST)
$(ACCESSMODETEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/AccessModeTest.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/AccessModeTest.cpp $(CPPUNITFLAGS) -o $(ACCESSMODETEST); $(ACCESSMODETEST)

converttest : $(CONVERTTEST)
$(CONVERTTEST):$(LIBFILES) $(TESTFILES) $(TESTSDIR)/ConvertTest.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/text2fvf.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/ConvertTest.cpp $(CPPUNITFLAGS) -o $(CONVERTTEST); $(CONVERTTEST)

buildtestdata : $(BUILDTESTDATA)
$(BUILDTESTDATA):$(TESTSDIR)/BuildTestData.cpp $(LIBDIR)/*.cpp $(LIBDIR)/*.h
	$(CPP) $(CFLAGS) $(TESTSDIR)/BuildTestData.cpp $(LIBDIR)/*.cpp $(CPPUNITFLAGS) -o $(BUILDTESTDATA)

preparetestdata: buildtestdata
	$(BUILDTESTDATA) tests/data/correctnessTestData 33815 1473 100 120; $(BUILDTESTDATA) tests/data/2write/modify_me 1001 1003;

tests : preparetestdata correctnesstest readspeed modificationtest unittest writespeed accessmodetest converttest transposetest
runtests : tests

tests-clean:
	rm -rf tests/data/2write/* tests/data/modetests/* tests/data/correctnessTestData.*
