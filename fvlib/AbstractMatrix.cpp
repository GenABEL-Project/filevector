#include "AbstractMatrix.h"

set<string> AbstractMatrix::fileNamesOpenForWriting;
void AbstractMatrix::checkOpenForWriting(const string fileName){
    testDbg << "checkOpenForWriting("<< fileName <<")" << endl;
    if (AbstractMatrix::fileNamesOpenForWriting.find(fileName)!=fileNamesOpenForWriting.end()) {
        throw 1; // file already opened for writing
    } else {
        AbstractMatrix::fileNamesOpenForWriting.insert(fileName);
    }
}
void AbstractMatrix::closeForWriting(const string fileName){
    testDbg << "closeForWriting("<< fileName <<")" << endl;
    AbstractMatrix::fileNamesOpenForWriting.erase(fileName);
}

