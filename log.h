#ifndef log_h
#define log_h

#include <stdio.h>

#define LOG(msg...) fprintf(Log::log, msg); printf(msg);

struct Log
{
    static FILE * log;
    
    static void Create(const char * path);
    static void Destroy(void);
};

#endif /* log_h */
