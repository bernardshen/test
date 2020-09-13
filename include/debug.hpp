#ifndef DEBUG_HEADER
#define DEBUG_HEADER

#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

#define MAX_FORMAT_LEN 255
#define DEBUG true
#define TITLE true
#define TIMER true
#define CUR true

class Debug {
private:
    static long startTime;

public:
    static void debugTitle(const char *str);
    static void debugItem(const char *format, ...);
    static void debugCur(const char *format, ...);
    static void notifyInfo(const char *format, ...);
    static void notifyError(const char *format, ...);
    static void startTimer(const char *);
    static void endTimer(); 
};

#endif