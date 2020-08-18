#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define COMMAND_LEN_MAX 1024
#define TOKEN_LEN_MAX COMMAND_LEN_MAX
#define TOKENS_MAX      256

/* https://gist.github.com/Fonger/98cc95ac39fbe1a7e4d9 */
size_t strlcat(char *dst, const char *src, size_t size) {
    size_t srclen;
    size_t dstlen;

    dstlen = strlen(dst);
    size -= dstlen + 1;

    if (!size)
        return (dstlen);

    srclen = strlen(src);

    if (srclen > size)
        srclen = size;

    memcpy(dst + dstlen, src, srclen);
    dst[dstlen + srclen] = '\0';

    return (dstlen + srclen);
}

size_t strlcpy(char *dst, const char *src, size_t size) {
    size_t srclen;

    size --;

    srclen = strlen(src);

    if (srclen > size)
        srclen = size;

    memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return (srclen);
}

void Prompt(void) {
    char username[LOGIN_NAME_MAX];
    char hostname[HOST_NAME_MAX];
    char workingDir[PATH_MAX];
    size_t promptMax = LOGIN_NAME_MAX + HOST_NAME_MAX + PATH_MAX + 4;
    char prompt[promptMax];

    getlogin_r(username, LOGIN_NAME_MAX);
    gethostname(hostname, HOST_NAME_MAX);
    getcwd(workingDir, PATH_MAX);

    strlcpy(prompt, username, promptMax);
    strlcat(prompt, "@", promptMax);
    strlcat(prompt, hostname, promptMax);
    strlcat(prompt, ":", promptMax);
    strlcat(prompt, workingDir, promptMax);
    strlcat(prompt, "$ ", promptMax);

    printf("%s", prompt);
}

void ReadCommand(char *command, size_t len) {
    fgets(command, len, stdin);
    command[strlen(command) - 1] = '\0';
}

void Tokenize(char *command, char **tokens, size_t *tokensCount,
 size_t tokensMax, size_t tokenLenMax) {
    char *pch;

    *tokensCount = 0;

    pch = strtok(command," ");
    while (pch != NULL) {
        strlcpy(tokens[*tokensCount], pch, tokenLenMax);
        pch = strtok(NULL, " ");
        (*tokensCount)++;
        if (*tokensCount > tokensMax)
            break;
    }
}

bool IsNumber(char *str, size_t len) {
    bool number = true;

    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\0') {
            if (i == 0) {
                number = false;
            }
            break;
        }
        if (!isdigit(str[i])) {
            number = false;
            break;
        }
    }

    return number;
}

void InitTokens(char **tokens, size_t len) {
    for (size_t i = 0; i < len; i++) {
        tokens[i] = NULL;
    }
}

void RefreshTokens(char **tokens, size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (tokens[i] == NULL)
            tokens[i] = malloc(sizeof(char) * len);
    }
}

void NullTerminateTokens(char **tokens, size_t len, size_t tokensCount) {
    for (size_t i = tokensCount; i < len; i++) {
        free(tokens[i]);
        tokens[i] = NULL;
    }
}

int main(int argc, char **argv) {
    char   command[COMMAND_LEN_MAX];
    char **tokens = malloc(sizeof(char *) * TOKENS_MAX);
    size_t tokensCount;
    int    status;
    int    exitStatus = 0;

    InitTokens(tokens, TOKENS_MAX);

    while (true) {
        RefreshTokens(tokens, TOKENS_MAX);

        Prompt();
        ReadCommand(command, COMMAND_LEN_MAX);
        Tokenize(command, tokens, &tokensCount, TOKENS_MAX, TOKEN_LEN_MAX);

        NullTerminateTokens(tokens, TOKENS_MAX, tokensCount);

        if (strncmp(tokens[0], "exit", TOKEN_LEN_MAX) == 0) {
            if (tokens[1] == NULL) {
                return exitStatus;
            } else {
                if (IsNumber(tokens[1], TOKEN_LEN_MAX))
                    return strtol(tokens[1], NULL, 10);
                else
                    return exitStatus;
            }
        }

        if (fork() != 0) {
            /* parent */
            waitpid(-1, &status, 0);
            exitStatus = WEXITSTATUS(status);
        } else {
            /* child */
            int ret = execvp(tokens[0], tokens);
            if (ret != 0) {
                perror("execve");
                return 0;
            }
        }
    }

    return 0;
}
