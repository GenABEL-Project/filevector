#include <stdlib.h>

#include "RealHandlerWrapper.h"
#include "ReusableFileHandle.h"

map<string, RealHandlerWrapper*> ReusableFileHandle::openHandles;

int ReusableFileHandle::testGetNumHandles(){
    return ReusableFileHandle::openHandles.size(); 
}

ReusableFileHandle ReusableFileHandle::getHandle(string fileName, bool readOnly){
/*    RealHandlerWrapper *newHandleWrapper = new RealHandlerWrapper();
    bool success = newHandleWrapper->open(fileName, readOnly);
    return ReusableFileHandle (newHandleWrapper, success, fileName, readOnly);*/

    cout << "getHandle("+fileName+", readonly=" << readOnly << ")" << endl;

    string key = string((readOnly ? "R" : "*")) + fileName;

    if (ReusableFileHandle::openHandles.find(key) != ReusableFileHandle::openHandles.end()) {
        cout << "Using existing handle." << endl;
        RealHandlerWrapper *rhw = ReusableFileHandle::openHandles[key];

        ReusableFileHandle ret(rhw, true, fileName, readOnly);

        return ret;
    } else {
        cout << "Creating new handle." << endl;
        RealHandlerWrapper *newHandleWrapper = new RealHandlerWrapper();
        bool success = newHandleWrapper->open(fileName, readOnly);

        if (success) {
            ReusableFileHandle::openHandles[key] = newHandleWrapper;
        }

        ReusableFileHandle ret(newHandleWrapper, success, fileName, readOnly);
        return ret;
    }
}

void ReusableFileHandle::close() {

    cout << "ReusableFileHandle::close("<<fileName<<")" <<endl; 

/*    if (isOk) {
        this->realHandlerWrapper->close();
        delete this->realHandlerWrapper;
    }*/

    string key = string((readOnly ? "R" : "*")) + fileName;

    if (ReusableFileHandle::openHandles.find(key) == ReusableFileHandle::openHandles.end())
        return;

    RealHandlerWrapper *rhw = ReusableFileHandle::openHandles[key];

    cout << "RealHandlerWrapper::close()" <<endl; 
    rhw->close();

    if (rhw->getUseCount() == 0) {
        cout << "rhw->getUseCount() == 0" <<endl; 
        ReusableFileHandle::openHandles.erase(key);
    }
}



void ReusableFileHandle::blockWriteOrRead(unsigned long length, char* data, bool writeAction){
//    cout << "ReusableFileHandle::blockWriteOrRead:"<<endl;
  //  cout << "fseek(" << curPos << ")" << endl;
    realHandlerWrapper->fseek(curPos);
//    cout << "read(" << length << ")" << endl;
    realHandlerWrapper->blockWriteOrRead(length, data, writeAction);
    curPos += length;
}

void ReusableFileHandle::flush(){
    realHandlerWrapper->flush();
}

void ReusableFileHandle::fseek(unsigned long pos){
    curPos = pos;
}

