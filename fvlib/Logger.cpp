#include "Logger.h"

Logger::Logger(int iErrorLevel)
{
           errorLevel = iErrorLevel;
           enabled = true;
}
                     

ErrorExit errorExit;
Logger inf(MESSAGE_LEVEL);
Logger dbg(DEBUG_LEVEL);
Logger msg(MESSAGE_LEVEL);
Logger testDbg(DEBUG_LEVEL);
Logger deepDbg(DEBUG_LEVEL);
Logger errorLog(ERROR_LEVEL);





