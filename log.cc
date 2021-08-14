#include "log.h"
#include "defines.h"

FILE * Log::log = NULL;


void Log::Create(const char * path)
{
    if ( log ) {
        Destroy();
    }
    
    log = fopen(path, "w");
    
    if ( log == NULL ) {
        ERROR("Failed to create log");
    }
}


void Log::Destroy(void)
{
    if ( log == NULL ) {
        return;
    }
    fclose(log);
    log = NULL;
}
