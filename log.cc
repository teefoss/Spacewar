#include "log.h"
#include "defines.h"

FILE * Log::log = NULL;


void Log::create(const char * path)
{
    if ( log ) {
        destroy();
    }
    
    log = fopen(path, "w");
    
    if ( log == NULL ) {
        ERROR("Failed to create log");
    }
}


void Log::destroy(void)
{
    if ( log == NULL ) {
        return;
    }
    fclose(log);
    log = NULL;
}
