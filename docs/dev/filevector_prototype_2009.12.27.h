// FILEVECTOR
//
// Priority marks:
// * HIGH = we need that to be implemented in most efficient manner
// * MEDIUM = needs to be implemented, but could be implemented in inefficient
//   manner for time-being
// * LOW = does not need to be implemented, but the rest should be designed
//   to keep the opportunity for implementation open. If keeping this open
//   contradicts with design/requirements specification, please discuss
//
// Data is a large matrix of DT type, with rows called "variables" and
// columns called "observations"
//
// New data types (LOW): 2-bits, half precision coding of the
// interval [0-2]
//
// Design decisions before we start:
// * currently, trying to access data out of boundaries calls "error"
//   meaning that the the calling application is responsible for
//   control of that, if failed, the application is terminated.
//   This is no good behavior, need to develop some solution
// * currently, retrieve-by-index mechanism is assumed
//   other mechanisms are retrieval by the-same-length vector of logical
//   and retrieval by the list of names. The latter is may be difficult
//   and expensive (need to keep the names and provide fast searching
//   mechanism. Do we need that?
// * need to re-think the data structure of the class for increased
//   portability (no idea how portable it is now -- but there were
//   some problems with "long long" as far as I can remember (Mac/Linux
//   differences)
// * dynamic cache resizing of all objects at the time when new one is
//   loaded -- is that realistic?
// * do we want to build in a possibility that data is kept in two copies
//   (transposed)? this will allow fast access of both variables and
//   observations, but complicates update and especially adding of the data.
//   Possible solution: if redundant data used, disable data-add
//   functions. Also, add transposed(-on-open) mechanism.
// * need to develop a mechanism to store NA (not available, missing) value
// * would be great if we can develop a mechanism for quick access to
//   flat text tables (read-only)


template <class DT> class filevector
{

// DATA structures

// constructor
// current:
	filevector(char * filename_toload, unsigned long int cachesizeMb);
//	required:
	filevector(char * filename_toload, unsigned long int desired_cache_size_Mb, int oFlag, int dbFormat);
//	HIGH:
//	oFlag (= BDB):
//		DB_CREATE: If the database does not currently exist, create it.
//			By default, the database open fails if the database does not already exist.
//		DB_EXCL: Exclusive database creation. Causes the database open to fail if the
//			database already exists. This flag is only meaningful when used with DB_CREATE.
//		DB_RDONLY: Open the database for read operations only. Causes any subsequent database
//			write operations to fail.
//	MEDIUM:
//		DB_TRUNCATE: Physically truncate (empty) the on-disk file that contains the database.
//			Causes DB to delete all databases physically contained in that file.
//	desired_cache_size_MB:
//	HIGH:
//		0 = minimal (1 vector at a time)
//		1 = maximal (need to check the system)
//	LOW:
//		2 = balanced maximal (dynamically changing cache size of other objects)
//	LOW:
//	dbFormat:
//		NATIVE: "native" filevector binary format
//      PLAIN: plain text file (read-only access); will need to pass more arguments?

// ...
// these need to be fixed given new constructor specification
// can read single variable
	void read_variable(unsigned long int nvar, DT * outvec);
// should only be used for reading single random elements!
	DT read_element(unsigned long int nvar, unsigned long int nobs);
// write single variable
// CURRENTLY CACHE IS NOT UPDATED!
	void write_variable(unsigned long int nvar, DT * datavec);
// write single element
// CURRENTLY CACHE IS NOT UPDATED!
	void write_element(unsigned long int nvar, unsigned long int nobs, DT data);
// working with names /do not like FIXEDCHAR!/
	fixedchar read_observation_name(unsigned long int nobs); // CHANGE NAME -- is now get_...
	fixedchar read_variable_name(unsigned long int nvar);    // CHANGE NAME -- is now get_...
// destroy object
	void free_filevector();

// These are new functions
// HIGH
// TAKE CARE BOTH FILE AND CACHE ARE UPDATED
    void write_variable_name(unsigned long int nvar,fixedchar newname);  // loooong future -- control that name is unique
    void write_observation_name(unsigned long int nobs,fixedchar newname);  // loooong future -- control that name is unique!
 // HIGH -- here I see the possibility to make these functions faster then "random" access functions
    void add_variable(DT * invec, fixedchar varname); // adds variable at the end = write_variable with nvar=NVARS?
                                                      // loooong future -- control that name is unique!
// ???? nado ????
	void read_first_variable(DT * outvec);
	void read_next_variable(DT * outvec);
// LOW
	void delete_variable(unsigned long int nvar);
// MEDIUM -- see notes on redundant data storage
	void read_observation(unsigned long int nobs, DT * outvec);
    void write_observation(unsigned long int nobs, DT * invec);
// ???? nado ????
	void read_first_observation(DT * outvec);
	void read_next_observation(DT * outvec);
// LOW
	void add_observation(DT * invec, fixedchar obsname); // loooong future -- control that name is unique!
	void delete_observation(unsigned long int nobs);

// MEDIUM
// same functions working through NAMES -- primitive

// LOW
// implement all read_* functions with an extra argument, "rFilter"
// rFilter:
// * vector of 0/1 or logical of NOBSERVATIONS/NVARIABLES, only values for
//   observations/variables for which rFilter is 1/TRUE are returned
// * arbitrary length vector containing observation/variable names
//   only values for observations/variables with these names are
//   returned (in the order of names!)






// NEW STAFF -- AFTER 27.12.2009
// these functions should create a copy of a file with (filtered) object
	void save(char * new_file_name);
	void save(char * new_file_name, unsigned long int nvars, unsigned long int * varindexes);
	void save(char * new_file_name, unsigned long int nobss, unsigned long int * obsindexes);
	void save(char * new_file_name, unsigned long int nvars, unsigned long int nobss, unsigned long int * varindexes, unsigned long int * obsindexes);
// changing cache size on the fly
	void set_cachesizeMb(unsigned long int cachesizeMb);
};
;
