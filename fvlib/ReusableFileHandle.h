#ifndef __ReusableFileHandle__
#define __ReusableFileHandle__

#include "RealHandlerWrapper.h"

#include <string>
#include <map>

using namespace std;

class ReusableFileHandle {
private:
    bool isOk;
    unsigned long curPos;
    string fileName;
    bool readOnly;
    
    RealHandlerWrapper *realHandlerWrapper;

    static map<string, RealHandlerWrapper*> openHandles;

public:
    ReusableFileHandle(RealHandlerWrapper *iRealHandlerWrapper, bool iIsOk,
        const string& iFileName, bool iReadOnly ):
        realHandlerWrapper(iRealHandlerWrapper), isOk(iIsOk),
        fileName(iFileName), readOnly(iReadOnly), curPos(0) {
    }

    ReusableFileHandle(const ReusableFileHandle &rfh) :
        realHandlerWrapper(rfh.realHandlerWrapper), isOk(rfh.isOk),
        fileName(rfh.fileName), readOnly(rfh.readOnly), curPos(rfh.curPos) {
    }

    void operator=(const ReusableFileHandle &rfh){
        isOk = rfh.isOk;
        curPos = rfh.curPos;
        fileName = rfh.fileName;
        readOnly = rfh.readOnly;
        realHandlerWrapper = rfh.realHandlerWrapper;
    }
    
    ReusableFileHandle() : isOk(false),
        fileName(), readOnly(false), curPos(0) {
    }

    operator bool(){
        return isOk;    
    }
    
    void blockWriteOrRead(unsigned long length, char* data, bool writeAction);
    void fseek(unsigned long pos);
    void flush();

    static ReusableFileHandle getHandle(string fileName, bool readOnly);
    static int testGetNumHandles();
    void close();
};

#endif