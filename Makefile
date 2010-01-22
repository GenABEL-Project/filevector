VERSION = 0.0-8
SRCDIR = fvfutil
LIBDIR = fvlib
TESTSDIR = tests
#DASRCDIR = DatABEL/src
BINDIR = bin
TEXT2FVF = $(BINDIR)/text2fvf
MERGEVARS = $(BINDIR)/mergevars
#RSHLIB = $(BINDIR)/filevector.so

READSPEED = $(BINDIR)/readspeed
CORRECTNESSTEST = $(BINDIR)/correctnesstest
ACCESSMODETEST = $(BINDIR)/accessmodetest 
MODIFTEST = $(BINDIR)/modificationtest
UNITTEST = $(BINDIR)/fvunittest
CONVERT = $(BINDIR)/convert
WRITE_VAR_SPEED = $(BINDIR)/writevarspeed


# GNU compiler
CPP = g++
# use for Solaris
# CPP = CC

CFLAGS = -I $(LIBDIR) -I $(SRCDIR) -g #-m64 
CPPUNITFLAGS = -lcppunit
EXECS = $(TEXT2FVF) $(MERGEVARS)

LIBFILES = $(LIBDIR)/*.cpp $(LIBDIR)/*.h
REGFILES = $(SRCDIR)/usage.cpp $(SRCDIR)/usage.h

TESTSFILES = $(TESTSDIR)/*.cpp $(TESTSDIR)/*.h

all: $(EXECS) # $(RSHLIB)

$(TEXT2FVF): $(LIBFILES) $(REGFILES) $(SRCDIR)/text2fvf.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/usage.cpp $(SRCDIR)/text2fvf.cpp -o $(TEXT2FVF)

$(MERGEVARS): $(LIBFILES) $(REGFILES) $(SRCDIR)/mergevars.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(SRCDIR)/usage.cpp $(SRCDIR)/mergevars.cpp -o $(MERGEVARS)

#$(RSHLIB): $(LIBFILES) $(SRCDIR)/Rstaff.cpp $(SRCDIR)/Rstaff.h
#	R CMD SHLIB $(SRCDIR)/Rstaff.cpp $(LIBDIR)/*.cpp $(CFLAGS) -o $(RSHLIB)

fvfutildis: clean
	cd ..; tar cvf filevector_$(VERSION).tar filevector_$(VERSION); gzip filevector_$(VERSION).tar

DatABELdis: clean
	cd ..; tar chvf DatABEL_$(VERSION).tar DatABEL_$(VERSION); gzip DatABEL_$(VERSION).tar

clean: 
	rm -rf $(BINDIR)/* $(SRCDIR)/*.o  $(LIBDIR)/*.o


correctnesstest : $(CORRECTNESSTEST)
$(CORRECTNESSTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/CorrectnessTest.cpp
	$(CPP) $(CFLAGS) ${CPPUNITFLAGS} $(LIBDIR)/*.cpp $(TESTSDIR)/CorrectnessTest.cpp ${TESTSDIR}/TestUtil.cpp -o $(CORRECTNESSTEST)

readspeed : $(READSPEED)
$(READSPEED): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/ReadSpeed.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/ReadSpeed.cpp  -o $(READSPEED)

modificationtest : $(MODIFTEST)
$(MODIFTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/FileModificationTest.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/FileModificationTest.cpp  -o $(MODIFTEST)

unittest : $(UNITTEST)
$(UNITTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/FVUnitTest.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/FVUnitTest.cpp  -o $(UNITTEST)

converter : $(CONVERT)
$(CONVERT): $(LIBFILES) $(SRCDIR)/convert.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp  $(SRCDIR)/convert.cpp -o $(CONVERT)

writespeed : $(WRITE_VAR_SPEED)
$(WRITE_VAR_SPEED): $(LIBFILES) $(SRCDIR)/convert.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/WriteVariablesSpeed.cpp -o $(WRITE_VAR_SPEED)

accessmodetest : $(ACCESSMODETEST)
$(ACCESSMODETEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/AccessModeTest.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/TestUtil.cpp $(TESTSDIR)/AccessModeTest.cpp -o $(ACCESSMODETEST)

tests : correctnesstest	readspeed modificationtest unittest writespeed accessmodetest
	