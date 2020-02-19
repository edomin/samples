#define _GNU_SOURCE
#include <dlfcn.h>
#include <err.h>
#include <execinfo.h>
#include <inttypes.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADDR2LINE_CMD_MAX_LEN 512
#define MAX_STACK_FRAMES 64
#define STR_BUF_LEN_MAX 256
#define OFFSET_STRING_LEN_MAX 15

static void *stack_traces[MAX_STACK_FRAMES];

int Addr2line(char const * const programName, const char *addr) {
    char addr2lineCmd[ADDR2LINE_CMD_MAX_LEN] = {0};

    sprintf(addr2lineCmd,"addr2line -i -f -p -e %.256s %s", programName,
     addr);

    return system(addr2lineCmd);
}

size_t GetProgramName(const char *message, char *programName,
 size_t programNameMaxLen) {
    char  *programNameLastChar;
    size_t i = 0;

    if (message == NULL || programName == NULL)
        return 0;

    programNameLastChar = strstr(message, "(");
    if (programNameLastChar == NULL)
        return 0;

    while (&message[i] != programNameLastChar && i != programNameMaxLen) {
        programName[i] = message[i];
        i++;
    }

    programName[i++] = '\0';

    return --i;
}

size_t GetSoFunctionName(const char *message, char *name, size_t nameLenMax) {
    char  *substringStart;
    char  *substringEnd;
    char  *currentChar;
    size_t i = 0;

    substringStart = strstr(message, "(");
    if (substringStart == NULL)
        return 0;

    substringEnd = strstr(substringStart, "+");

    currentChar = substringStart + 1;
    while (&currentChar[i] != substringEnd && i != nameLenMax) {
        name[i] = currentChar[i];
        i++;
    }

    name[i++] = '\0';

    return --i;
}

size_t GetRealSoFuncOffset(const char *soName, const char *funcName,
 uintmax_t relOffset, char *offsetString,
 size_t offsetStringMaxLen) {
    void   *handle = dlopen(soName, RTLD_LAZY);
    void   *func;
    char   *error = NULL;
    Dl_info funcInfo;
    uintmax_t funcAddr;
    int       snprintfResult;

    if (handle == NULL)
        return 0;

    func = dlsym(handle, funcName);

    error = dlerror();
    if (error != NULL) {
        dlclose(handle);
        return 0;
    }

    dladdr(func, &funcInfo);
    funcAddr = (uintmax_t)funcInfo.dli_saddr - (uintmax_t)funcInfo.dli_fbase;

    dlclose(handle);

    return snprintf(offsetString, offsetStringMaxLen, "0x%" PRIxMAX,
     funcAddr + relOffset);
}

size_t GetSoFunctionOffsetString(const char *message, char *offsetString,
 size_t offsetStringMaxLen) {
    char     *substringStart;
    char     *substringEnd;
    char     *currentChar;
    size_t    i = 0;
    char      soFuncName[STR_BUF_LEN_MAX];
    char      relOffsetString[STR_BUF_LEN_MAX];
    uintmax_t relOffset;
    char      soName[STR_BUF_LEN_MAX];

    GetSoFunctionName(message, soFuncName, STR_BUF_LEN_MAX);

    substringStart = strstr(message, "+0x");

    substringEnd = strstr(substringStart, ")");

    currentChar = substringStart + 1;
    while (&currentChar[i] != substringEnd && i != STR_BUF_LEN_MAX) {
        relOffsetString[i] = currentChar[i];
        i++;
    }

    relOffsetString[i++] = '\0';

    relOffset = strtoumax(relOffsetString, NULL, 16);
    GetProgramName(message, soName, STR_BUF_LEN_MAX);

    return GetRealSoFuncOffset(soName, soFuncName, relOffset, offsetString,
     offsetStringMaxLen);
}

size_t GetFunctionOffsetString(const char *message, char *offsetString,
 size_t offsetStringMaxLen) {
    char  *substringStart;
    char  *substringEnd;
    char  *currentChar;
    size_t i = 0;

    if (message == NULL || offsetString == NULL)
        return 0;

    substringStart = strstr(message, "(+0x");
    if (substringStart == NULL)
        return GetSoFunctionOffsetString(message, offsetString,
         offsetStringMaxLen);

    substringEnd = strstr(substringStart, ")");

    currentChar = substringStart + 2;
    while (&currentChar[i] != substringEnd && i != offsetStringMaxLen) {
        offsetString[i] = currentChar[i];
        i++;
    }

    offsetString[i++] = '\0';

    return --i;
}

void PrintStackTrace(void) {
    int i, trace_size = 0;
    char **messages = (char **)NULL;

    trace_size = backtrace(stack_traces, MAX_STACK_FRAMES);
    messages = backtrace_symbols(stack_traces, trace_size);

    for (i = 3; i < trace_size - 1; ++i) {
        char programName[STR_BUF_LEN_MAX];
        char offsetString[OFFSET_STRING_LEN_MAX];
        GetProgramName(messages[i], programName, STR_BUF_LEN_MAX);
        GetFunctionOffsetString(messages[i], offsetString,
         OFFSET_STRING_LEN_MAX);
        if (Addr2line(programName, offsetString) != 0) {
            fputs("  error determining line # for: ", stderr);
            fputs(messages[i], stderr);
            fputs("\n", stderr);
        }
    }
    if (messages) {
        free(messages);
    }
}

void SignalHandler(int signal, siginfo_t *siginfo, void *context) {
    switch(signal) {
        case SIGSEGV:
            fputs("Caught SIGSEGV: Segmentation Fault\n", stderr);
            break;
        case SIGINT:
            fputs(
             "Caught SIGINT: Interactive attention signal, (usually ctrl+c)\n",
             stderr);
            break;
        case SIGFPE:
            switch(siginfo->si_code) {
                case FPE_INTDIV:
                    fputs("Caught SIGFPE: (integer divide by zero)\n", stderr);
                    break;
                case FPE_INTOVF:
                    fputs("Caught SIGFPE: (integer overflow)\n", stderr);
                    break;
                case FPE_FLTDIV:
                    fputs("Caught SIGFPE: (floating-point divide by zero)\n",
                     stderr);
                    break;
                case FPE_FLTOVF:
                    fputs("Caught SIGFPE: (floating-point overflow)\n",
                     stderr);
                    break;
                case FPE_FLTUND:
                    fputs("Caught SIGFPE: (floating-point underflow)\n",
                     stderr);
                    break;
                case FPE_FLTRES:
                    fputs("Caught SIGFPE: (floating-point inexact result)\n",
                     stderr);
                    break;
                case FPE_FLTINV:
                    fputs(
                     "Caught SIGFPE: (floating-point invalid operation)\n",
                     stderr);
                    break;
                case FPE_FLTSUB:
                    fputs("Caught SIGFPE: (subscript out of range)\n", stderr);
                    break;
                default:
                    fputs("Caught SIGFPE: Arithmetic Exception\n", stderr);
                    break;
            }
            break;
        case SIGILL:
            switch(siginfo->si_code) {
                case ILL_ILLOPC:
                    fputs("Caught SIGILL: (illegal opcode)\n", stderr);
                    break;
                case ILL_ILLOPN:
                    fputs("Caught SIGILL: (illegal operand)\n", stderr);
                    break;
                case ILL_ILLADR:
                    fputs("Caught SIGILL: (illegal addressing mode)\n",
                     stderr);
                    break;
                case ILL_ILLTRP:
                    fputs("Caught SIGILL: (illegal trap)\n", stderr);
                    break;
                case ILL_PRVOPC:
                    fputs("Caught SIGILL: (privileged opcode)\n", stderr);
                    break;
                case ILL_PRVREG:
                    fputs("Caught SIGILL: (privileged register)\n", stderr);
                    break;
                case ILL_COPROC:
                    fputs("Caught SIGILL: (coprocessor error)\n", stderr);
                    break;
                case ILL_BADSTK:
                    fputs("Caught SIGILL: (internal stack error)\n", stderr);
                    break;
                default:
                    fputs("Caught SIGILL: Illegal Instruction\n", stderr);
                    break;
            }
            break;
        case SIGTERM:
            fputs(
             "Caught SIGTERM: a termination request was sent to the program\n",
             stderr);
            break;
        case SIGABRT:
            fputs("Caught SIGABRT: usually caused by an abort() or assert()\n",
             stderr);
            break;
        default:
            break;
    }
    PrintStackTrace();
    _Exit(1);
}

bool SetAlternateSignalStack(void) {
    stack_t ss;

    ss.ss_sp = malloc(SIGSTKSZ);
    if (ss.ss_sp == NULL) {
        fputs("Unable to allocate memory for alternate stack\n", stderr);
        return false;
    }

    ss.ss_size = SIGSTKSZ;
    ss.ss_flags = 0;

    if (sigaltstack(&ss, NULL) == -1) {
        fputs("Unable to allocate memory for alternate stack\n", stderr);
        return false;
    }

    return true;
}

void SetSigaction(int signal, const char *signalName,
 const struct sigaction *sigAction) {
    if (sigaction(signal, sigAction, NULL) == -1) {
        fputs("Unable to change action for signal: ", stderr);
        fputs(signalName, stderr);
        fputs("\n", stderr);
    }
}

void SetSignalHandler(void) {
    #ifdef __APPLE__
        bool useAlternateSignalStack = false;
    #else
        bool useAlternateSignalStack = SetAlternateSignalStack();
    #endif
    struct   sigaction sigAction = {};

    sigAction.sa_sigaction = SignalHandler;
    sigemptyset(&sigAction.sa_mask);

    if (useAlternateSignalStack)
        sigAction.sa_flags = SA_SIGINFO | SA_RESTART | SA_ONSTACK;
    else
        sigAction.sa_flags = SA_SIGINFO | SA_RESTART;

    SetSigaction(SIGABRT, "SIGABRT", &sigAction);
    SetSigaction(SIGFPE, "SIGFPE", &sigAction);
    SetSigaction(SIGILL, "SIGILL", &sigAction);
    SetSigaction(SIGINT, "SIGINT", &sigAction);
    SetSigaction(SIGSEGV, "SIGSEGV", &sigAction);
    SetSigaction(SIGTERM, "SIGTERM", &sigAction);
}

void qux(void) {
    void *handle;
    void (*baz)(void);
    char *error;

    handle = dlopen("./so.so", RTLD_LAZY);
    if (!handle) {
       fprintf(stderr, "%s\n", dlerror());
       exit(EXIT_FAILURE);
    }

    baz = (void (*)(void)) dlsym(handle, "baz");

    error = dlerror();
    if (error != NULL) {
        dlclose(handle);
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    baz();
    dlclose(handle);
}

void quux(void) {
    qux();
}

int main(int argc, char **argv) {
    SetSignalHandler();

    quux();

    return 0;
}
