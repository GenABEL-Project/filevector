## Additional flags to pass to aclocal when it is invoked automatically at
## make time. The ${ACLOCAL_FLAGS} variable is picked up from the environment
## to provide a way for the user to supply additional arguments.
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}


HEADERFILES = AbstractMatrix.h frversion.h CastUtils.h FileVector.h	\
 ReusableFileHandle.h const.h FilteredMatrix.h Logger.h Transposer.h	\
 convert_util.h frutil.h RealHandlerWrapper.h



## Define a libtool archive target "libexample-@EXAMPLE_API_VERSION@.la", with
## @EXAMPLE_API_VERSION@ substituted into the generated Makefile at configure
## time.
## The libtool archive file (.la) will be installed into the directory named
## by the predefined variable $(bindir), along with the actual shared library
## file (.so).
lib_LTLIBRARIES = libfilevector-@FV_API_VERSION@.la
libfilevector_@FV_API_VERSION@_la_SOURCES = ${HEADERFILES}		\
 AbstractMatrix.cpp CastUtils.cpp FileVector.cpp			\
 ReusableFileHandle.cpp FilteredMatrix.cpp Logger.cpp Transposer.cpp	\
 convert_util.cpp frutil.cpp RealHandlerWrapper.cpp

libfilevector_@FV_API_VERSION@_la_LDFLAGS = -version-info ${FV_SO_VERSION}



filevector_includedir=$(includedir)/filevector-${FV_API_VERSION}/filevector
filevector_include_HEADERS = ${HEADERFILES}

filevector_libincludedir = $(libdir)/filevector-${FV_API_VERSION}/include
nodist_filevector_libinclude_HEADERS = filevectorconfig.h

# The pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = filevector-${FV_API_VERSION}.pc
