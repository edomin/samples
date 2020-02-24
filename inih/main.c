#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "ini.h"

#define FILENAME             "config.ini"
#define ARRAY_GROW_SIZE      8
#define SECTION_NAME_LEN_MAX 64
#define KEY_LEN_MAX          64
#define VALUE_LEN_MAX        8192
#define TEST_BUF_LEN_MAX     8192

typedef FILE *(*FopenFunc)(const char *, const char *);
typedef int (*FcloseFunc)(FILE *);
typedef size_t (*FreadFunc)(void *, size_t, size_t, FILE *);
typedef size_t (*FwriteFunc)(const void *, size_t, size_t, FILE *);
typedef int (*FseekFunc)(FILE *, long, int);
typedef long (*FtellFunc)(FILE *);

typedef struct {
    char  *key;
    char  *value;
    size_t keyLen;
    size_t valueLen;
} Kv;

typedef struct {
    char  *name;
    size_t nameLen;
    Kv    *kvs;
    size_t kvsCount;
    size_t kvsMax;
} Section;

typedef struct {
    Section   *sections;
    size_t     sectionsCount;
    size_t     sectionsMax;
    bool       customFileIo;
    FopenFunc  fopenFunc;
    FcloseFunc fcloseFunc;
    FreadFunc  freadFunc;
    FwriteFunc fwriteFunc;
    FseekFunc  fseekFunc;
    FtellFunc  ftellFunc;
} Ini;

bool SectionGrow(Section *section) {
    size_t i;
    Kv    *oldKvs = section->kvs;

    section->kvs = realloc(section->kvs, sizeof(Kv) * (section->kvsMax + ARRAY_GROW_SIZE));
    if (section->kvs == NULL) {
        section->kvs = oldKvs;
        return false;
    }
    section->kvsMax += ARRAY_GROW_SIZE;

    for (i = section->kvsMax - ARRAY_GROW_SIZE; i < section->kvsMax; i++) {
        section->kvs[i].key = NULL;
        section->kvs[i].value = NULL;
    }

    return true;
}

bool IniGrow(Ini *ini) {
    size_t   i;
    Section *oldSections = ini->sections;

    ini->sections = realloc(ini->sections, sizeof(Section) * (ini->sectionsMax + ARRAY_GROW_SIZE));
    if (ini->sections == NULL) {
        ini->sections = oldSections;
        return false;
    }
    ini->sectionsMax += ARRAY_GROW_SIZE;

    for (i = ini->sectionsMax - ARRAY_GROW_SIZE; i < ini->sectionsMax; i++) {
        ini->sections[i].name = NULL;
        ini->sections[i].kvs = NULL;
        ini->sections[i].kvsCount = 0;
        ini->sections[i].kvsMax = 0;
        if (!SectionGrow(&ini->sections[i])) {
            ini->sectionsMax = i;
            return false;
        }
    }

    return true;
}

void FreeSection(Section *section) {
    size_t i;

    for (i = 0; i < section->kvsCount; i++) {
        if (section->kvs[i].key != NULL)
            free(section->kvs[i].key);
        if (section->kvs[i].value != NULL)
            free(section->kvs[i].value);
    }
    free(section->kvs);
}

void FreeIni(Ini *ini) {
    size_t i;

    for (i = 0; i < ini->sectionsCount; i++) {
        if (ini->sections[i].name != NULL)
            free(ini->sections[i].name);
        FreeSection(&ini->sections[i]);
    }
    free(ini->sections);
    free(ini);
}

Section *FindSection(const Ini *ini, const char *sectionName) {
    size_t i;

    for (i = 0; i < ini->sectionsCount; i++) {
        if (sectionName == NULL && ini->sections[i].name == NULL)
            return &ini->sections[i];
        if (sectionName == NULL || ini->sections[i].name == NULL)
            continue;
        if (strcmp(sectionName, ini->sections[i].name) == 0)
            return &ini->sections[i];
    }

    return NULL;
}

Kv *FindKv(const Section *section, const char *key) {
    size_t i;

    for (i = 0; i < section->kvsCount; i++) {
        if (strcmp(key, section->kvs[i].key) == 0)
            return &section->kvs[i];
    }

    return NULL;
}

bool AddSection(Ini *ini, const char *sectionName, Section **newSection) {
    Section *foundSection;

    if (sectionName != NULL) {
        if (strcmp(sectionName, "") == 0)
            sectionName = NULL;
    }

    foundSection = FindSection(ini, sectionName);

    if (foundSection != NULL) {
        if (newSection != NULL)
            *newSection = foundSection;
        return true;
    }
    if (ini->sectionsCount == ini->sectionsMax) {
        if (!IniGrow(ini))
            return false;
    }

    if (sectionName != NULL) {
        size_t   sectionNameLen = strlen(sectionName);
        Section *section = &ini->sections[ini->sectionsCount];

        if (sectionNameLen > SECTION_NAME_LEN_MAX)
            sectionNameLen = SECTION_NAME_LEN_MAX;

        section->name = malloc(sizeof(char) * (sectionNameLen + 1));
        if (section->name == NULL)
            return false;
        strncpy(section->name, sectionName, sectionNameLen);
        section->name[sectionNameLen] = '\0';
        section->nameLen = strlen(section->name);
    }

    if (newSection != NULL)
        *newSection = &ini->sections[ini->sectionsCount];
    ini->sectionsCount++;

    return true;
}

bool AddKv(Ini *ini, const char *sectionName, const char *key,
 const char *value) {
    Section *section;
    Kv      *kv;
    size_t   newValueLen = strlen(value);
    char    *oldValue;

    if (!AddSection(ini, sectionName, &section))
        return false;

    kv = FindKv(section, key);

    if (kv == NULL) {
        size_t keyLen;

        if (section->kvsCount == section->kvsMax) {
            if (!SectionGrow(section))
                return false;
        }
        keyLen = strlen(key);
        kv = &section->kvs[section->kvsCount];

        if (keyLen > KEY_LEN_MAX)
            keyLen = KEY_LEN_MAX;

        kv->key = malloc(sizeof(char) * (keyLen + 1));
        if (kv->key == NULL)
            return false;
        strncpy(kv->key, key, keyLen);
        kv->key[keyLen] = '\0';
        kv->keyLen = strlen(kv->key);
        section->kvsCount++;
    }

    oldValue = kv->value;

    kv->value = realloc(kv->value, sizeof(char) * (newValueLen + 1));
    if (kv->value == NULL) {
        if (oldValue == NULL) {
            free(kv->key);
            section->kvsCount--;
        }
        return false;
    }
    strncpy(kv->value, value, newValueLen);
    kv->value[newValueLen] = '\0';
    kv->valueLen = newValueLen;

    return true;
}

Ini *InitIni() {
    Ini   *ini = malloc(sizeof(Ini));
    size_t i;

    if (ini == NULL)
        return NULL;

    ini->sections = NULL;
    ini->sectionsCount = 0;
    ini->sectionsMax = 0;
    if (!IniGrow(ini)) {
        FreeIni(ini);
        return NULL;
    }

    if (!AddSection(ini, NULL, NULL)) {
        FreeIni(ini);
        return NULL;
    }

    ini->customFileIo = false;
    ini->fopenFunc = fopen;
    ini->fcloseFunc = fclose;
    ini->freadFunc = fread;
    ini->fwriteFunc = fwrite;
    ini->fseekFunc = fseek;
    ini->ftellFunc = ftell;

    return ini;
}

Ini *InitCustomIni(FopenFunc fopenFunc, FcloseFunc fcloseFunc,
 FreadFunc freadFunc, FwriteFunc fwriteFunc, FseekFunc fseekFunc,
 FtellFunc ftellFunc) {
    Ini *ini = InitIni();

    if (ini == NULL)
        return NULL;

    if (fopenFunc == NULL || fcloseFunc == NULL || freadFunc == NULL
     || fseekFunc == NULL || ftellFunc == NULL)
        return ini;

    ini->customFileIo = true;
    ini->fopenFunc = fopenFunc;
    ini->fcloseFunc = fcloseFunc;
    ini->freadFunc = freadFunc;
    ini->fwriteFunc = fwriteFunc;
    ini->fseekFunc = fseekFunc;
    ini->ftellFunc = ftellFunc;

    return ini;
}

size_t Min(size_t num1, size_t num2) {
    return (num1 < num2) ? num1 : num2;
}

size_t GetSectionsCount(Ini *ini) {
    return ini->sectionsCount;
}

size_t GetKeysCount(Ini *ini, char *sectionName) {
    Section *section = FindSection(ini, sectionName);

    if (section == NULL)
        return 0;

    return section->kvsCount;
}

size_t GetSection(Ini *ini, size_t index, char *sectionName,
 size_t sectionNameLenMax) {
    Section *section;

    if (ini->sectionsCount < index)
        return 0;

    section = &ini->sections[index];

    strncpy(sectionName, section->name,
     Min(sectionNameLenMax, section->nameLen));
    sectionName[Min(sectionNameLenMax, section->nameLen)] = '\0';

    return Min(sectionNameLenMax, section->nameLen);
}

size_t GetKey(Ini *ini, const char *sectionName, size_t index, char *key,
 size_t keyLenMax) {
    Section *section;
    Kv      *kv;

    section = FindSection(ini, sectionName);
    if (section == NULL)
        return 0;

    if (index > section->kvsCount)
        return 0;

    kv = &section->kvs[index];

    strncpy(key, kv->key, Min(keyLenMax, kv->keyLen));
    key[Min(keyLenMax, kv->keyLen)] = '\0';

    return Min(keyLenMax, kv->keyLen);
}

size_t GetValue(Ini *ini, const char *sectionName, const char *key,
 char *value, size_t valueLenMax) {
    Section *section;
    Kv      *kv;

    section = FindSection(ini, sectionName);
    if (section == NULL)
        return 0;

    kv = FindKv(section, key);
    if (kv == NULL)
        return 0;

    strncpy(value, kv->value, Min(valueLenMax, kv->valueLen));
    value[Min(valueLenMax, kv->valueLen)] = '\0';

    return Min(valueLenMax, kv->valueLen);
}

void MoveSectionToTail(Section *sections, size_t index, size_t sectionsCount) {
    size_t i;
    Section *section = &sections[index];

    for (i = index; i < sectionsCount - 1; i++)
        sections[i] = sections[i + 1];

    sections[sectionsCount - 1] = *section;
}

void MoveKvToTail(Kv *kvs, size_t index, size_t kvsCount) {
    size_t i;
    Kv    *kv = &kvs[index];

    for (i = index; i < kvsCount - 1; i++)
        kvs[i] = kvs[i + 1];

    kvs[kvsCount - 1] = *kv;
}

void DeleteSection(Ini *ini, const char *sectionName) {
    size_t   i;

    if (ini == NULL || sectionName == NULL)
        return;

    for (i = 1; i < ini->sectionsCount; i++) {
        if (strcmp(sectionName, ini->sections[i].name) == 0) {
            size_t   j;
            Section *section = &ini->sections[i];

            free(section->name);
            section->name = NULL;
            for (j = 0; j < section->kvsCount; j++) {
                if (section->kvs[j].key != NULL) {
                    free(section->kvs[j].key);
                    section->kvs[j].key = NULL;
                }
                if (section->kvs[j].value != NULL) {
                    free(section->kvs[j].value);
                    section->kvs[j].value = NULL;
                }
            }
            section->kvsCount = 0;
            MoveSectionToTail(ini->sections, i, ini->sectionsCount);
            ini->sectionsCount--;
        }
    }
}

void DeleteKey(Ini *ini, const char *sectionName, const char *key) {
    Section *section;
    size_t   i;

    if (ini == NULL || key == NULL)
        return;

    section = FindSection(ini, sectionName);
    if (section == NULL)
        return;

    for (i = 0; i < section->kvsCount; i++) {
        if (strcmp(key, section->kvs[i].key) == 0) {
            free(section->kvs[i].key);
            free(section->kvs[i].value);
            section->kvs[i].key = NULL;
            section->kvs[i].value = NULL;
            MoveKvToTail(section->kvs, i, section->kvsCount);
            section->kvsCount--;
        }
    }
}

void ClearSection(Ini *ini, const char *sectionName) {
    size_t   i;
    Section *section;

    if (ini == NULL)
        return;

    section = FindSection(ini, sectionName);
    if (section == NULL)
        return;

    for (i = 0; i < section->kvsCount; i++) {
        if (section->kvs[i].key != NULL) {
            free(section->kvs[i].key);
            section->kvs[i].key = NULL;
        }
        if (section->kvs[i].value != NULL) {
            free(section->kvs[i].value);
            section->kvs[i].value = NULL;
        }
    }
    section->kvsCount = 0;
}

size_t PrintIni(Ini *ini, char *output, size_t maxLen) {
    size_t sectionIndex;
    size_t remainingLen = maxLen;
    size_t printedLen;

    for (sectionIndex = 0; sectionIndex < ini->sectionsCount; sectionIndex++) {
        Section *section = &ini->sections[sectionIndex];
        size_t   kvIndex;

        if (sectionIndex > 0) {
            printedLen = snprintf(output, remainingLen, "[%s]\n", section->name);
            remainingLen -= printedLen;
            output += printedLen;
        }

        for (kvIndex = 0; kvIndex < section->kvsCount; kvIndex++) {
            Kv *kv = &section->kvs[kvIndex];

            printedLen = snprintf(output, remainingLen, "%s = %s\n", kv->key, kv->value);
            remainingLen -= printedLen;
            output += printedLen;
        }

        if (sectionIndex == ini->sectionsCount - 1)
            continue;

        if (sectionIndex == 0 && section->kvsCount == 0)
            continue;

        printedLen = snprintf(output, remainingLen, "%s", "\n");
        remainingLen -= printedLen;
        output += printedLen;
    }

    return maxLen - remainingLen;
}

size_t PrintIniGetSize(Ini *ini) {
    size_t sectionIndex;
    size_t resultLen = 0;

    for (sectionIndex = 0; sectionIndex < ini->sectionsCount; sectionIndex++) {
        Section *section = &ini->sections[sectionIndex];
        size_t   kvIndex;

        if (sectionIndex > 0)
            resultLen += strlen(section->name) + 3;

        for (kvIndex = 0; kvIndex < section->kvsCount; kvIndex++) {
            Kv *kv = &section->kvs[kvIndex];

            resultLen += strlen(kv->key) + strlen(kv->value) + 4;
        }

        if (sectionIndex == ini->sectionsCount - 1)
            continue;

        if (sectionIndex == 0 && section->kvsCount == 0)
            continue;

        resultLen += 1;
    }

    return resultLen;
}

int iniParseHandler(void* ini, const char* section, const char* key,
 const char* value) {
    bool success;

    if (key == NULL && value == NULL)
        success = AddSection(ini, section, NULL);
    else
        success = AddKv(ini, section, key, value);

    return success ? 1 : 0;
}

// int IniParseFile(Ini *ini, const char *filename) {
//     return ini_parse(filename, iniParseHandler, ini);
// }

int IniParseString(Ini *ini, const char *iniString) {
    return ini_parse_string(iniString, iniParseHandler, ini);
}

int IniParseCustom(Ini *ini, const char *filename) {
    void  *file;
    size_t fileSize;
    size_t readed;
    char  *buffer = NULL;
    int    parseResult;

    file = ini->fopenFunc(filename, "rb");
    if (file == NULL)
        return -1;

    ini->fseekFunc(file, 0L, SEEK_END);
    fileSize = ini->ftellFunc(file);
    ini->fseekFunc(file, 0L, SEEK_SET);

    buffer = malloc(sizeof(char) * fileSize);
    if (buffer == NULL) {
        ini->fcloseFunc(file);
        return -2;
    }

    readed = ini->freadFunc(buffer, sizeof(char), fileSize, file);
    if (readed != fileSize) {
        ini->fcloseFunc(file);
        free(buffer);
        return -1;
    }

    ini->fcloseFunc(file);

    parseResult = ini_parse_string(buffer, iniParseHandler, ini);

    free(buffer);

    return parseResult;
}

void IniSaveCustom(Ini *ini, const char *filename) {
    void  *file;
    size_t bufferSize = PrintIniGetSize(ini);
    char  *buffer = malloc(bufferSize);
    size_t writen;

    if (buffer == NULL || ini->fwriteFunc == NULL)
        return;

    PrintIni(ini, buffer, bufferSize);

    file = fopen(filename, "wb");
    if (file == NULL) {
        free(buffer);
        return;
    }

    writen = fwrite(buffer, sizeof(char), bufferSize, file);

    free(buffer);
    fclose(file);
}

int main(int argc, char **argv) {
    int    parseResult;
    Ini   *iniFromString = InitIni();
    Ini   *ini = InitCustomIni(fopen, fclose, fread, fwrite, fseek, ftell);
    size_t printedIniSize;

    char test[TEST_BUF_LEN_MAX];

    if (ini == NULL)
        return 1;

    parseResult = IniParseCustom(ini, FILENAME);
    printf("parseResult: %i\n", parseResult);

    GetSection(ini, 0, test, TEST_BUF_LEN_MAX);
    printf("Section with index 0: %s\n", test);
    GetSection(ini, 1, test, TEST_BUF_LEN_MAX);
    printf("Section with index 1: %s\n", test);
    GetSection(ini, 2, test, TEST_BUF_LEN_MAX);
    printf("Section with index 2: %s\n", test);

    GetKey(ini, NULL, 0, test, TEST_BUF_LEN_MAX);
    printf("Key with index 0 without section: %s\n", test);
    GetKey(ini, "section1", 0, test, TEST_BUF_LEN_MAX);
    printf("Key with index 0 in section 'section1': %s\n", test);
    GetKey(ini, "section1", 1, test, TEST_BUF_LEN_MAX);
    printf("Key with index 1 in section 'section1': %s\n", test);
    GetKey(ini, "section2", 0, test, TEST_BUF_LEN_MAX);
    printf("Key with index 0 in section 'section2': %s\n", test);
    GetKey(ini, "section2", 1, test, TEST_BUF_LEN_MAX);
    printf("Key with index 1 in section 'section2': %s\n", test);

    GetValue(ini, "section1", "bar", test, VALUE_LEN_MAX);
    printf("Value of key 'bar' in section 'section1': %s\n", test);

    printf("Sections count: %zu\n", GetSectionsCount(ini));

    printedIniSize = PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("Ini:\n%s", test);

    printf("Ini size: %zu\n", printedIniSize);
    printf("Ini strlen: %zu\n", strlen(test));

    DeleteKey(ini, "section1", "foo");
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After deleting key 'foo':\n%s", test);

    DeleteSection(ini, "section2");
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After deleting section 'section2':\n%s", test);

    DeleteSection(ini, NULL);
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After attempt to delete section without header (that with sectionless keys):\n%s", test);

    ClearSection(ini, "section1");
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After clearing section 'section1':\n%s", test);

    ClearSection(ini, NULL);
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After clearing section without header:\n%s", test);

    AddSection(ini, "sec1", NULL);
    AddSection(ini, "sec2", NULL);
    AddSection(ini, "sec3", NULL);
    AddSection(ini, "sec4", NULL);
    AddSection(ini, "sec5", NULL);
    AddSection(ini, "sec6", NULL);
    AddSection(ini, "sec7", NULL);
    AddSection(ini, "sec8", NULL);
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After adding 8 new sections:\n%s", test);

    AddKv(ini, "sec8", "k1", "v1");
    AddKv(ini, "sec8", "k2", "v2");
    AddKv(ini, "sec8", "k3", "v3");
    AddKv(ini, "sec8", "k4", "v4");
    AddKv(ini, "sec8", "k5", "v5");
    AddKv(ini, "sec8", "k6", "v6");
    AddKv(ini, "sec8", "k7", "v7");
    AddKv(ini, "sec8", "k8", "v8");
    PrintIni(ini, test, TEST_BUF_LEN_MAX);
    printf("After adding 8 new keys:\n%s", test);

    parseResult = IniParseString(iniFromString, "[section1]\nkey=value\n");
    printedIniSize = PrintIni(iniFromString, test, TEST_BUF_LEN_MAX);
    printf("Another ini loaded from string:\n%s", test);
    printf("Ini size: %zu\n", printedIniSize);
    printf("Ini strlen: %zu\n", strlen(test));

    IniSaveCustom(iniFromString, "from_string.ini");

    FreeIni(ini);
    FreeIni(iniFromString);

    return 0;
}
