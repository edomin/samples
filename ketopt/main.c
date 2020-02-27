#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ketopt.h"

#define LONGOPTS_GROW_SIZE 8
#define LONGOPT_NULL_TERM(LONG_OPTION) \
 LONG_OPTION.name = NULL; \
 LONG_OPTION.has_arg = 0; \
 LONG_OPTION.val = 0;
#define UNIQUE_KEY_FIRST   301
#define ARG_NO             ko_no_argument
#define ARG_OPTIONAL       ko_required_argument
#define ARG_REQUIRED       ko_optional_argument

typedef struct {
    /*
     * actual array size must be larger by one then longOptsMax because last
     * element is null terminator.
     * longOptsMax - maximum value of longOptsCount.
     * For example: if array size is 8 then longOptsMax is 7 and longOptsCount
     * can not be larger 7.
     */
    int           argc;
    char        **argv;
    char          shortOpts[1024];
    ko_longopt_t *longOpts;
    size_t        longOptsCount;
    size_t        longOptsMax;
    int           longOptsUniqueKey;
    ketopt_t      status;
    int           parseResult;
} Options;

bool OptionsGrow(Options *options) {
    ko_longopt_t *oldLongOpts;

    if (options == NULL)
        return false;

    oldLongOpts = options->longOpts;

    options->longOpts = realloc(options->longOpts,
     sizeof(ko_longopt_t) * (options->longOptsMax + LONGOPTS_GROW_SIZE + 1));
    if (options->longOpts == NULL) {
        options->longOpts = oldLongOpts;
        return false;
    }

    options->longOptsMax += LONGOPTS_GROW_SIZE;

    return true;
}

Options *OptionsInit(int argc, char **argv) {
    Options *options = malloc(sizeof(Options));

    if (options == NULL)
        return NULL;

    options->argc = argc;
    options->argv = argv;

    options->longOpts = NULL;
    options->longOptsCount = 0;
    options->longOptsMax = -1;

    if (!OptionsGrow(options)) {
        free(options);
        return NULL;
    }

    options->shortOpts[0] = '\0';
    LONGOPT_NULL_TERM(options->longOpts[options->longOptsCount]);
    options->longOptsUniqueKey = UNIQUE_KEY_FIRST;

    return options;
}

void OptionsFree(Options *options) {
    if (options == NULL)
        return;

    free(options->longOpts);
    free(options);
}

bool AddLongOption(Options *options, const char *optionName, int argument) {
    size_t        optionLen;
    ko_longopt_t *option;

    if (options == NULL || optionName == NULL)
        return false;

    if (options->longOptsCount == options->longOptsMax) {
        if (!OptionsGrow(options))
            return false;
    }

    optionLen = strlen(optionName);

    option = &options->longOpts[options->longOptsCount];
    option->name = malloc(optionLen + 1);
    if (option->name == NULL)
        return false;
    strncpy(option->name, optionName, optionLen);
    option->name[optionLen] = '\0';

    option->has_arg = argument;
    option->val = options->longOptsUniqueKey++;

    options->longOptsCount++;
    LONGOPT_NULL_TERM(options->longOpts[options->longOptsCount]);

    return true;
}

bool AddShortOption(Options *options, const char *optionName, int argument) {
    size_t shortOptsLen = strlen(options->shortOpts);

    if (options == NULL || optionName == NULL || argument == ARG_OPTIONAL)
        return false;

    options->shortOpts[shortOptsLen] = optionName[0];
    options->shortOpts[shortOptsLen + 1] = '\0';
    if (argument == ARG_REQUIRED) {
        options->shortOpts[shortOptsLen + 1] = ':';
        options->shortOpts[shortOptsLen + 2] = '\0';
    }

    return true;
}

void OptionsParseBegin(Options *options) {
    options->status = KETOPT_INIT;
}

bool OptionsParse(Options *options) {
    options->parseResult = ketopt(&options->status, options->argc,
     options->argv, 1, options->shortOpts, options->longOpts);

    if (options->parseResult == -1)
        return false;

    return true;
}

bool CheckShortOption(const Options *options, const char *shortOption) {
    if (options == NULL || shortOption == NULL)
        return false;

    if (options->parseResult == shortOption[0])
        return true;

    return false;
}

int GetLongOptionUniqueKey(const Options *options, const char *longOption) {
    if (options == NULL || longOption == NULL)
        return -1;

    for (size_t i = 0; i < options->longOptsCount; i++) {
        if (strcmp(options->longOpts[i].name, longOption) == 0)
            return options->longOpts[i].val;
    }

    return -1;
}

bool CheckLongOption(const Options *options, const char *longOption) {
    int longOptionUniqueKey;

    if (options == NULL || longOption == NULL)
        return false;

    longOptionUniqueKey = GetLongOptionUniqueKey(options, longOption);
    if (longOptionUniqueKey == -1)
        return false;

    if (options->parseResult == longOptionUniqueKey)
        return true;

    return false;
}

bool OptionHaveArg(const Options *options) {
    if (options == NULL)
        return false;

    return !!options->status.arg;
}

size_t GetOptionArg(const Options *options, char *result, size_t resultMaxLen)
{
    size_t argLen;
    size_t resultLen;

    if (options == NULL || result == NULL || resultMaxLen == 0)
        return 0;

    if (!OptionHaveArg(options))
        return 0;

    argLen = strlen(options->status.arg);
    resultLen = (argLen < resultMaxLen) ? argLen : resultMaxLen;

    strncpy(result, options->status.arg, resultLen);
    result[resultLen] = '\0';

    return resultLen;
}

int main(int argc, char **argv) {
    Options *options = OptionsInit(argc, argv);

    AddLongOption(options, "foo", ARG_NO);
    AddLongOption(options, "bar", ARG_REQUIRED);
    AddLongOption(options, "ex1", ARG_REQUIRED);
    AddLongOption(options, "ex2", ARG_REQUIRED);
    AddLongOption(options, "ex3", ARG_REQUIRED);
    AddLongOption(options, "ex4", ARG_REQUIRED);
    AddLongOption(options, "ex5", ARG_REQUIRED);
    AddLongOption(options, "ex6", ARG_REQUIRED);
    AddLongOption(options, "ex7", ARG_REQUIRED);
    AddLongOption(options, "ex8", ARG_REQUIRED);
    AddShortOption(options, "x", ARG_NO);
    AddShortOption(options, "y", ARG_REQUIRED);
    AddShortOption(options, "z", ARG_NO);

    OptionsParseBegin(options);

    while(OptionsParse(options)) {
        if (CheckLongOption(options, "foo")) {
            printf("%s\n", "foo");
        }
        if (CheckLongOption(options, "bar")) {
            printf("%s\n", "bar");
        }
        if (CheckShortOption(options, "x")) {
            printf("%s\n", "x");
        }
        if (CheckShortOption(options, "y")) {
            printf("%s\n", "y");
        }
        if (CheckShortOption(options, "z")) {
            printf("%s\n", "z");
        }
        if (OptionHaveArg(options)) {
            char arg[1024];

            GetOptionArg(options, arg, 1024);
            printf("arg: %s\n", arg);
        }
    }

    OptionsFree(options);

    return 0;
}
