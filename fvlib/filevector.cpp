#include "frutil.h"
#include "const.h"

fr_type get_file_type(char * filename) {
	fr_type out;
	std::ifstream myfile(filename, std::ios::binary | std::ios::in);
	if (!myfile) {
		error("can not open file for reading\n");
	}
	myfile.read((char*)&out,sizeof(out));
	return(out);
}

void initialize_empty_file(std::string filename, unsigned long int nvariables, unsigned long int nobservations, unsigned short int type)
{
	fr_type metadata;
// data element size
	unsigned long int desize=0;
	switch (type) {
	  case UNSIGNED_SHORT_INT:
		desize = sizeof(unsigned short int);
		break;
	  case SHORT_INT:
		desize = sizeof(short int);
		break;
	  case UNSIGNED_INT:
		desize = sizeof(unsigned int);
		break;
	  case INT:
		desize = sizeof(int);
		break;
	  case FLOAT:
		desize = sizeof(float);
		break;
	  case DOUBLE:
		desize = sizeof(double);
		break;
	  default:
		error("file contains data of unknown type\n",filename.c_str());
	}
	metadata.type = type;
	metadata.nvariables = nvariables;
	metadata.nobservations = nobservations;
	metadata.nelements = nvariables*nobservations;
	metadata.bytes_per_record = desize;
	metadata.bits_per_record = desize*8;
	
	std::ofstream outfile(filename.c_str(), std::ios::binary | std::ios::out);
	outfile.write((char*)&metadata,sizeof(metadata));

	fixedchar obsname;
	for (unsigned long int i=0;i<nobservations;i++) {
		sprintf(obsname.name,"%lu",i+1);
		outfile.write((char*)&obsname.name,sizeof(obsname.name));
	}
	for (unsigned long int i=0;i<nvariables;i++) {
		sprintf(obsname.name,"%lu",i+1);
		outfile.write((char*)&obsname.name,sizeof(obsname.name));
	}

	unsigned long int estimated_size = (unsigned long int) metadata.bytes_per_record * 
																					(unsigned long int) metadata.nvariables * 
																					(unsigned long int) metadata.nobservations + 
																					(unsigned long int) sizeof(metadata) + 
																					(unsigned long int) sizeof(fixedchar) *
																					((unsigned long int) nobservations + 
																					 (unsigned long int) nvariables);

	outfile.seekp(estimated_size-1);
	outfile.put('E');
	outfile.close();
}

