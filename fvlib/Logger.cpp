#include "Logger.h"

                     
ErrorExit errorExit;

#define all false

Logger inf(MESSAGE_LEVEL,all);
Logger dbg(DEBUG_LEVEL,all);
Logger msg(MESSAGE_LEVEL,all);
Logger testDbg(DEBUG_LEVEL,all);
Logger deepDbg(DEBUG_LEVEL,false);
Logger errorLog(ERROR_LEVEL,all);





