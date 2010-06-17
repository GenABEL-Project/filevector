#include <fstream>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

#include "frutil.h"
#include "text2fvf.h"

double val(double r, double c){
    return sin(r)*r+cos(c);
}

void buildSubmatrix(string baseName, unsigned long numVariables, unsigned long numObservations, unsigned long subMatrixNumVars, unsigned long subMatrixNumObs){
    ofstream f((baseName+string(".fvf_submatrix")).c_str(),ios::out);
    ofstream varNames((baseName+string(".fvf_varnames")).c_str(),ios::out);
    ofstream obsNames((baseName+string(".fvf_obsnames")).c_str(),ios::out);

    unsigned long varIdx, obsIdx, subVarIdx, subObsIdx;
    cout << "Building test submatrix data." << endl;

    for(subObsIdx=0;subObsIdx<subMatrixNumObs;subObsIdx++){
        obsIdx = subObsIdx + numObservations/2;
        f << "obs" << obsIdx <<" ";
        obsNames << "obs" << obsIdx <<" ";
    }
    f<< endl;
    obsNames << endl;
    for(subVarIdx=0;subVarIdx<subMatrixNumVars;subVarIdx++){
        varIdx = subVarIdx + numVariables/2;
        f << "var" << varIdx << " ";
        varNames << "var" << varIdx << " ";
        for(subObsIdx=0;subObsIdx<subMatrixNumObs;subObsIdx++){T
            obsIdx = subObsIdx + numObservations/2;
            f << val(varIdx, obsIdx) << " ";
        }

        f << endl;
    }
    varNames << endl;
    f.close();
    cout << "Done." << endl;
}

void genSumFiles(string baseName, unsigned long numObservations, unsigned long numVariables){
    fstream obsSumFile((baseName+string(".fvf_obssum")).c_str(), ios::out),
            varSumFile((baseName+string(".fvf_varsum")).c_str(), ios::out);


    double *obsSums = new double[numObservations];


    unsigned long i,j;
    for(i=0;i<numObservations;i++){
        obsSums[i] = 0.;
    }
    
    for(j=0;j<numVariables;j++){

        double sum = 0;
        for(i=0;i<numObservations;i++){
            obsSums[i]+= val(j,i);
            sum += val(j,i);
        }

        varSumFile << sum << " ";
    }

    for(i=0;i<numObservations;i++){
        obsSumFile << obsSums[i] << " ";
    }
}

int main(int numArgs, char**args){
    if ((numArgs-1) != 3 && (numArgs-1) != 5) {
        cout << "Usage: BuildTestData fileName numVariables numObservations [subMatrixNumVars subMatrixNumObs]" << endl;
        return 0;
    }
    string baseName = string(args[1]);
    string textFileName = baseName + string(".txt");
    ofstream f(textFileName.c_str(),ios::out);

    unsigned long numVariables = atol(args[2]);
    unsigned long numObservations = atol(args[3]);

    unsigned long subMatrixNumVars = 0;
    unsigned long subMatrixNumObs = 0;

    if ((numArgs-1) == 5) {
        subMatrixNumVars = atol(args[4]);
        subMatrixNumObs = atol(args[5]);
    }

    unsigned long varIdx, obsIdx;

    if (headerOrDataExists(baseName)) {
        cout << "Test data already presents." << endl;
        return 0;
    }

    cout << "Building test data." << endl;

    for(obsIdx=0;obsIdx<numObservations;obsIdx++){
        f << "obs" << obsIdx <<" ";
    }
    f<< endl;
    for(varIdx=0;varIdx<numVariables;varIdx++){
        f << "var" << varIdx << " ";
        for(obsIdx=0;obsIdx<numObservations;obsIdx++){
            f << val(varIdx, obsIdx) << " ";
        }

        f << endl;
    }

    f.close();

    cout << "Building sum files." << endl;
    genSumFiles(baseName,numObservations, numVariables);
    if (subMatrixNumVars > 0)
        buildSubmatrix(baseName, numVariables, numObservations, subMatrixNumVars, subMatrixNumObs);

    cout << "Writing " << baseName << "..." << endl; 
    text2fvf("text2fvf", textFileName, baseName,"", "", 1, 1,0, 0, true, true, DOUBLE, true,"NAN");
    cout << "Done." << endl;
}