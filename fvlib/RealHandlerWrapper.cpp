#include "FileVector.h"
#include "RealHandlerWrapper.h"


// returns true on success
bool RealHandlerWrapper::open(const string &iFileName, bool iReadOnly) {
    fileName = iFileName;
    if (useCount > 0) {
        useCount++;    
        return true;
    }

	if (readOnly) {
		stream.open(fileName.c_str(), ios::in | ios::binary);
	} else {
		stream.open(fileName.c_str(), ios::out | ios::in | ios::binary);
	}

    stream.seekg(0,ios::end);
    length = stream.tellg();
    stream.seekg(0,ios::beg);

    readOnly = iReadOnly; 

	useCount = 1;

	return !!(stream);
}

void RealHandlerWrapper::close(){
    if (useCount > 1) {
        useCount--;
    } else if (useCount == 1) {
        useCount = 0;
        stream.close();
    } else; // false call, do nothing
}

void RealHandlerWrapper::blockWriteOrRead(unsigned long iLength, char* data, bool writeAction) {
    if (writeAction == false)
        memset(data, '!', iLength);
    ::blockWriteOrRead(stream, iLength, data, writeAction);
//    if (!stream) {
//        printf("useCount = %i", useCount);
 //       printf("BAD!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n\n");
//    }
}

void RealHandlerWrapper::fseek(unsigned long pos){
    lastPos = pos;
//    cout << "RealHandlerWrapper::fseek("<<fileName<<"," << pos << ")" << endl;
    stream.seekg(pos, ios::beg);
    stream.seekp(pos, ios::beg);
}

void RealHandlerWrapper::flush(){
    stream.flush();
}