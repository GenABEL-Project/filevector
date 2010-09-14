#include <stdlib.h>

#include "RealHandlerWrapper.h"
#include "ReusableFileHandle.h"

void hexdump(void *pAddressIn, long  lSize)
{
 char szBuf[100];
 long lIndent = 1;
 long lOutLen, lIndex, lIndex2, lOutLen2;
 long lRelPos;
 struct { char *pData; unsigned long lSize; } buf;
 unsigned char *pTmp,ucTmp;
 unsigned char *pAddress = (unsigned char *)pAddressIn;

   buf.pData   = (char *)pAddress;
   buf.lSize   = lSize;

   while (buf.lSize > 0)
   {
      pTmp     = (unsigned char *)buf.pData;
      lOutLen  = (int)buf.lSize;
      if (lOutLen > 16)
          lOutLen = 16;

      // create a 64-character formatted output line:
      sprintf(szBuf, " >                            "
                     "                      "
                     "    %08lX", pTmp-pAddress);
      lOutLen2 = lOutLen;

      for(lIndex = 1+lIndent, lIndex2 = 53-15+lIndent, lRelPos = 0;
          lOutLen2;
          lOutLen2--, lIndex += 2, lIndex2++
         )
      {
         ucTmp = *pTmp++;

         sprintf(szBuf + lIndex, "%02X ", (unsigned short)ucTmp);
         if(!isprint(ucTmp))  ucTmp = '.'; // nonprintable char
         szBuf[lIndex2] = ucTmp;

         if (!(++lRelPos & 3))     // extra blank after 4 bytes
         {  lIndex++; szBuf[lIndex+2] = ' '; }
      }

      if (!(lRelPos & 3)) lIndex--;

      szBuf[lIndex  ]   = '<';
      szBuf[lIndex+1]   = ' ';

      printf("%s\n", szBuf);

      buf.pData   += lOutLen;
      buf.lSize   -= lOutLen;
   }
}


map<string, RealHandlerWrapper*> ReusableFileHandle::openHandles;

int ReusableFileHandle::testGetNumHandles(){
    return ReusableFileHandle::openHandles.size(); 
}

ReusableFileHandle ReusableFileHandle::getHandle(string fileName, bool readOnly){
//    RealHandlerWrapper *newHandleWrapper = new RealHandlerWrapper();
//    bool success = newHandleWrapper->open(fileName, readOnly);
//    return ReusableFileHandle (newHandleWrapper, success, fileName, readOnly);

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


/*    if (isOk) {
        cout << "ReusableFileHandle::close("<<fileName<<")" <<endl; 
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
    cout << "ReusableFileHandle::blockWriteOrRead from "<<curPos<<", length = "<< length <<", file = " << fileName << "data:"
    <<(writeAction ? "write":"read")<< endl;
  //  cout << "fseek(" << curPos << ")" << endl;
    realHandlerWrapper->fseek(curPos);
//    cout << "read(" << length << ")" << endl;
    realHandlerWrapper->blockWriteOrRead(length, data, writeAction);

    hexdump(data,length > 100?100:length );


    curPos += length;

    cout << endl;
}

void ReusableFileHandle::flush(){
    realHandlerWrapper->flush();
}

void ReusableFileHandle::fseek(unsigned long pos){
    curPos = pos;
}



