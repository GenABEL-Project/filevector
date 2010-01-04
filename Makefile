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
READVARIABLE = $(BINDIR)/readvariable
MODIFTEST = $(BINDIR)/modificationtest

# GNU compiler
CPP = g++
# use for Solaris
# CPP = CC

CFLAGS = -I $(LIBDIR) -I $(SRCDIR) -O2 #-m64
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
	rm -f $(BINDIR)/* $(SRCDIR)/*.o  $(LIBDIR)/*.o


readvariable : $(READVARIABLE)

$(READVARIABLE): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/ReadVariableTest.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/ReadVariableTest.cpp -o $(READVARIABLE)

readspeed : $(READSPEED)
$(READSPEED): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/ReadSpeed.cpp
	$(CPP) $(CFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/ReadSpeed.cpp  -o $(READSPEED)

modificationtest : $(MODIFTEST)
$(MODIFTEST): $(LIBFILES) $(TESTFILES) $(TESTSDIR)/FileModificationTest.cpp
	$(CPP) $(CFLAGS) $(CPPUNITFLAGS) $(LIBDIR)/*.cpp $(TESTSDIR)/FileModificationTest.cpp  -o $(MODIFTEST)
