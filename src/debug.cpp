#include "debug.hpp"

void Debug::debugTitle(const char *str) {
    if (TITLE) 
        printf("\033[0;45;1m%s\033[0m\n", str);
}

void Debug::debugItem(const char *format, ...) {
    char newFormat[MAX_FORMAT_LEN];

    va_list args;
    va_start(args, format);

    if (DEBUG) {
        sprintf(newFormat, "\033[0;42;1m%s\033[0m\n", format); /* Wrap format in a style. */
        vprintf(newFormat, args);       /* Print string of debug item. */
    }

    va_end(args);
}

void Debug::debugCur(const char *format, ...) {
    char newFormat[MAX_FORMAT_LEN];

    va_list args;
    va_start(args, format);

    if (CUR == true) {
        sprintf(newFormat, "%s\n", format);
        vprintf(newFormat, args);
    }

    va_end(args);
}

void Debug::notifyInfo(const char *format, ...) {
    char newFormat[MAX_FORMAT_LEN];

    va_list args;
    va_start(args, format);
    sprintf(newFormat, "\033[4m%s\033[0m\n", format);
    vprintf(newFormat, args);
    va_end(args);
}

void Debug::notifyError(const char *format, ...) {
    char newFormat[MAX_FORMAT_LEN];

    va_list args;
    va_start(args, format);
    sprintf(newFormat, "\033[0;31m%s\033[0m\n", format);
    vprintf(newFormat, args);
    va_end(args);
}

void Debug::startTimer(const char *timerName) {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    if (TIMER == true) {
        Debug::startTime = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
    }
}

void Debug::endTimer() {
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    long endTime;
    if (TIMER == true) {
        endTime = tv.tv_sec * 1000 * 1000 + tv.tv_usec;
        printf("Timer is ended. Cost %ls us.\n", endTime - Debug::startTime);
    }
}

long Debug::startTime;