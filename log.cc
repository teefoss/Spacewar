#include "log.h"

FILE * Log::log = NULL;


bool Log::create(const char * path)
{
    if ( log ) {
        destroy();
    }
    log = fopen(path, "w");
    
    return log != NULL;
}


void Log::destroy(void)
{
    if ( log == NULL ) {
        return;
    }
    fclose(log);
    log = NULL;
}
