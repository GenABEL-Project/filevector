#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

#include "../fvlib/Logger.h"
#include "../fvlib/FileVector.h"
#include "../fvlib/CastUtils.h"

void info() 
{
    cout << "fv2text converts .fv* files to a text file" << endl;
    cout << "Usage: fv2text inputfile outfile.txt [--RMatrix]" << endl;
}

/*string printByteSeq(char *data, int length){
    string ret;
    
    char s[20];
    for(int i=0;i<length;i++){
	sprintf(s,"%x ", ((unsigned char*)data)[i]);
	    ret += string(s);
    }    

    return ret;    
}*/



int main(int argc, char* argv[])
{
    argc--;
    if (argc<2) 
    {
        info();
	return 0;
    }
  
    string inputFileName = string(argv[1]);
    string outputFileName = string(argv[2]);
    
    bool isRMatrix;
    
    if (argc >= 3) {
	isRMatrix = (strcmp(argv[3], "--RMatrix") == 0);
    }
    
    cout << "Input file is '" << inputFileName << "'." << endl;
    cout << "Output file is '" << outputFileName << "'." << endl;
    cout << "isRMatrix = " << isRMatrix << endl;
    
    FileVector fv(inputFileName, 1);
    ofstream out(outputFileName.c_str());
    
    unsigned long row, col;
    
    if (!isRMatrix) {
	    out << "X ";
    }
    
    for (col=0; col<fv.getNumObservations(); col++){
	    out << fv.readObservationName(col).name << " ";
    }
    
    out << endl;
	
    for (row=0; row<fv.getNumVariables(); row++){
	    out << fv.readVariableName(row).name << " ";
        for (col=0; col<fv.getNumObservations(); col++){
    	    char data[200];
    	    fv.readElement(row, col, data);
    	    string elem = bufToString(fv.getElementType(), data);
	        out << elem << " ";
	    }
	    out << endl;
    }
}
