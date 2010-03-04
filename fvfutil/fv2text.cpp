#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <map>

using namespace std;

#include "filevector.h"

void info() 
{
    cout << "fv2text converts .fv? files to a text file" << endl;
    cout << "Usage: fv2text inputfile outfile.txt --RMatrix" << endl;
}

string printByteSeq(char *data, int length){
    string ret;
    
    char s[20];
    for(int i=0;i<length;i++){
	sprintf(s,"%x ", ((unsigned char*)data)[i]);
	ret += string(s);
    }    

    return ret;    
}

string bufToString(short int dataType, char *data){
    char ret[20];
    switch(dataType){
	case UNSIGNED_SHORT_INT:
	    sprintf(ret, "%hu", *(unsigned short int*)data);
    	    break;
	case SHORT_INT:
	    sprintf(ret, "%sd", *(short int*)data);
    	    break;
    	case UNSIGNED_INT:
	    sprintf(ret, "%ud", *(unsigned int*)data);
	    break;
        case INT:
	    sprintf(ret, "%d", *(int*)data);
            break;
	case FLOAT:
	    sprintf(ret, "%f", *(float*)data);
	    break;
    	case DOUBLE:
	    sprintf(ret, "%lf", *(double*)data);
	    break;
    }
    
    return string(ret);                                     	
}

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
    
    filevector fv(inputFileName, 1);
    ofstream out(outputFileName.c_str());
    
    unsigned long row, col;
    
    if (!isRMatrix) {
	out << "X ";
    }
    
    for (col=0; col<fv.get_nobservations(); col++){
	out << fv.read_observation_name(col).name << " ";
    }
    
    out << endl;
	
    for (row=0; row<fv.get_nvariables(); row++){
	out << fv.read_variable_name(row).name << " ";
        for (col=0; col<fv.get_nobservations(); col++){
    	    char data[200];
    	    fv.read_element(row, col, data);
    	    string elem = bufToString(fv.getDataType(), data);    	        	    
	    out << elem << " ";
	}
	out << endl;
    }
}
