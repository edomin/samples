#define     SIR_FOPENMODE   "a"
#include "sir.h"

#define MESSAGE_LEN_MAX 8192

typedef enum {
    LL_DEBUG,
    LL_INFO,
    LL_NOTICE,
    LL_WARNING,
    LL_ERROR,
    LL_CRITICAL,
    LL_ALERT,
    LL_EMERGENCY,
    LL_MAX
} LogLevel;

typedef bool (*SirPrintfFunc)(const sirchar_t *format, ...);

SirPrintfFunc sirPrintfFuncs[LL_MAX] = {
    sir_debug,
    sir_info,
    sir_notice,
    sir_warn,
    sir_error,
    sir_crit,
    sir_alert,
    sir_emerg
};

void LogReportError(void) {
    sirchar_t message[SIR_MAXERROR] = {0};
    uint16_t  code = sir_geterror(message);

    fprintf(stderr, "libsir error: (%hu, %s)\n", code, message);
}

bool LogInit(const char *filename) {
    sirfileid_t fileid;
    sirinit     si = {0};

    si.d_stdout.levels = SIRL_DEBUG | SIRL_INFO | SIRL_NOTICE;
    si.d_stdout.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_stderr.levels = SIRL_WARN | SIRL_ERROR | SIRL_CRIT | SIRL_ALERT
     | SIRL_EMERG;
    si.d_stderr.opts = SIRO_NONAME | SIRO_NOPID | SIRO_NOTID;
    si.d_syslog.levels = 0;

    strcpy(si.processName, "example");

    if (!sir_init(&si)) {
        LogReportError();
        return false;
    }

    fileid = sir_addfile(filename, SIRL_ALL,
     SIRO_NONAME | SIRO_NOPID | SIRO_NOTID);

    if (fileid == NULL) {
        LogReportError();
        return false;
    }

    return true;
}

void LogFree(void) {
    sir_cleanup();
}

static inline SirPrintfFunc GetSirPrintfFunc(int level) {
    return sirPrintfFuncs[level];
}

void LogPrintf(int level, int padding, const char *format, ...) {
    SirPrintfFunc sirPrintfFunc= GetSirPrintfFunc(level);
    va_list       args;
    char          message[MESSAGE_LEN_MAX];
    char         *pmessage = message;

    for (int i = 0; i < padding; i++) {
        pmessage[0] = '-';
        pmessage[1] = ' ';
        pmessage += 2;
    }

    va_start(args, format);
    vsprintf (pmessage, format, args);
    va_end(args);

    sirPrintfFunc("%s", message);
}

int main(int argc, char **argv) {
    if (!LogInit("sample.log"))
        return 1;

    LogPrintf(LL_DEBUG, 0, "%s %i %f", "test", 1, 3.14f);
    LogPrintf(LL_DEBUG, 1, "%s %i %f", "test", 1, 3.14f);
    LogPrintf(LL_DEBUG, 2, "%s %i %f", "test", 1, 3.14f);
    LogPrintf(LL_ERROR, 0, "%s %i %f", "test", 1, 3.14f);
    LogPrintf(LL_ERROR, 1, "%s %i %f", "test", 1, 3.14f);
    LogPrintf(LL_ERROR, 2, "%s %i %f", "test", 1, 3.14f);

    LogFree();

    return 0;
}
