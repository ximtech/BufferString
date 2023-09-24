#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <stdlib.h>

#ifdef ENABLE_FLOAT_FORMATTING
#include <float.h>
#include <math.h>
#endif

typedef struct BufferString {
    char *value;
    uint32_t length;
    uint32_t capacity;
} BufferString;

typedef struct StringIterator {
    BufferString *str;
    const char *delimiter;
    uint32_t delimiterLength;
    char *nextToken;
} StringIterator;

typedef enum StringToI64Status {
    STR_TO_I64_SUCCESS,
    STR_TO_I64_OVERFLOW,
    STR_TO_I64_UNDERFLOW,
    STR_TO_I64_INCONVERTIBLE
} StringToI64Status;

// initialization
#define NEW_STRING(capacity, initValue) newString(&(BufferString){0}, initValue, (char[capacity]){0}, capacity)
#define NEW_STRING_LEN(capacity, initValue, length) newStringWithLength(&(BufferString){0}, initValue, length, (char[capacity]){0}, capacity)
#define NEW_STRING_BUFF(buffer, initValue) newString(&(BufferString){0}, initValue, buffer, sizeof(buffer) / sizeof((buffer)[0]))   // create string from existing local or static buffer
#define EMPTY_STRING(capacity) newString(&(BufferString){0}, "", (char[capacity]){0}, capacity)
#define DUP_STRING(capacity, source) dubString(source, &(BufferString){0}, (char[capacity]){0}, capacity)

#define STRING_FORMAT(capacity, format, args...) stringFormat(EMPTY_STRING(capacity), format, args)
#define SUBSTRING(capacity, source, beginIndex, endIndex) substringFromTo(source, EMPTY_STRING(capacity), beginIndex, endIndex)
#define SUBSTRING_AFTER(capacity, source, separator) substringAfter(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_AFTER_LAST(capacity, source, separator) substringAfterLast(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_BEFORE(capacity, source, separator) substringBefore(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_BEFORE_LAST(capacity, source, separator) substringBeforeLast(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_BETWEEN(capacity, source, open, close) substringBetween(source, EMPTY_STRING(capacity), open, close)

#define SUBSTRING_CSTR(capacity, source, beginIndex, endIndex) substringCStrFromTo(source, EMPTY_STRING(capacity), beginIndex, endIndex)
#define SUBSTRING_CSTR_AFTER(capacity, source, separator) substringCStrAfter(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_CSTR_AFTER_LAST(capacity, source, separator) substringCStrAfterLast(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_CSTR_BEFORE(capacity, source, separator) substringCStrBefore(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_CSTR_BEFORE_LAST(capacity, source, separator) substringCStrBeforeLast(source, EMPTY_STRING(capacity), separator)
#define SUBSTRING_CSTR_BETWEEN(capacity, source, open, close) substringCStrBetween(source, EMPTY_STRING(capacity), open, close)

#define INT64_TO_STRING(value) int64ToString(EMPTY_STRING(32), value)
#define UINT64_TO_STRING(value) uInt64ToString(EMPTY_STRING(32), value)

// useful inline creators
#define NEW_STRING_16(initValue)   NEW_STRING(16, initValue)
#define NEW_STRING_32(initValue)   NEW_STRING(32, initValue)
#define NEW_STRING_64(initValue)   NEW_STRING(64, initValue)
#define NEW_STRING_128(initValue)  NEW_STRING(128, initValue)
#define NEW_STRING_256(initValue)  NEW_STRING(256, initValue)
#define NEW_STRING_512(initValue)  NEW_STRING(512, initValue)
#define NEW_STRING_1024(initValue) NEW_STRING(1024, initValue)
#define NEW_STRING_2048(initValue) NEW_STRING(2048, initValue)

#define STRING_FORMAT_16(format, args...)   STRING_FORMAT(16, format, args)
#define STRING_FORMAT_32(format, args...)   STRING_FORMAT(32, format, args)
#define STRING_FORMAT_64(format, args...)   STRING_FORMAT(64, format, args)
#define STRING_FORMAT_128(format, args...)  STRING_FORMAT(128, format, args)
#define STRING_FORMAT_256(format, args...)  STRING_FORMAT(256, format, args)
#define STRING_FORMAT_512(format, args...)  STRING_FORMAT(512, format, args)
#define STRING_FORMAT_2048(format, args...) STRING_FORMAT(2048, format, args)

// create
BufferString *newStringWithLength(BufferString *str, const void *initValue, uint32_t initLength, char *buffer, uint32_t bufferLength);
BufferString *newString(BufferString *str, const void *initValue, char *buffer, uint32_t bufferLength);
BufferString *dubString(BufferString *source, BufferString *dest, char *buffer, uint32_t bufferLength);
BufferString *stringFormat(BufferString *str, const char *format, ...);

// fill
BufferString *concatCharsByLength(BufferString *str, const char *strToConcat, uint32_t length);
BufferString *concatChars(BufferString *str, const char *strToConcat);
BufferString *concatString(BufferString *str, BufferString *strToConcat);
BufferString *concatChar(BufferString *str, char charToConcat);
BufferString *copyString(BufferString *str, const char *strToCopy);
BufferString *copyStringByLength(BufferString *str, const char *strToCopy, uint32_t length);
BufferString *clearString(BufferString *str);

// modify
BufferString *toLowerCase(BufferString *str);
BufferString *toUpperCase(BufferString *str);
BufferString *swapCase(BufferString *str);
BufferString *replaceFirstOccurrence(BufferString *source, const char *target, const char *replacement);
BufferString *replaceAllOccurrences(BufferString *source, const char *target, const char *replacement);
BufferString *trimAll(BufferString *str);
BufferString *reverseString(BufferString *str);
BufferString *capitalize(BufferString *str, const char *delimiters, uint32_t length);

// substring
BufferString *substringFrom(BufferString *source, BufferString *destination, uint32_t beginIndex);
BufferString *substringFromTo(BufferString *source, BufferString *destination, uint32_t beginIndex, uint32_t endIndex);
BufferString *substringAfter(BufferString *source, BufferString *destination, const char *separator);
BufferString *substringAfterLast(BufferString *source, BufferString *destination, const char *separator);
BufferString *substringBefore(BufferString *source, BufferString *destination, const char *separator);
BufferString *substringBeforeLast(BufferString *source, BufferString *destination, const char *separator);
BufferString *substringBetween(BufferString *source, BufferString *destination, const char *open, const char *close);

// substring from 'char*'
BufferString *substringCStrFrom(char *source, BufferString *destination, uint32_t beginIndex);
BufferString *substringCStrFromTo(char *source, BufferString *destination, uint32_t beginIndex, uint32_t endIndex);
BufferString *substringCStrAfter(char *source, BufferString *destination, const char *separator);
BufferString *substringCStrAfterLast(char *source, BufferString *destination, const char *separator);
BufferString *substringCStrBefore(char *source, BufferString *destination, const char *separator);
BufferString *substringCStrBeforeLast(char *source, BufferString *destination, const char *separator);
BufferString *substringCStrBetween(char *source, BufferString *destination, const char *open, const char *close);

// split
StringIterator getStringSplitIterator(BufferString *str, const char *delimiter);
bool hasNextSplitToken(StringIterator *iterator, BufferString *token);

// join
BufferString *joinChars(BufferString *str, const char *delimiter, uint32_t argCount, ...);
BufferString *joinStringArray(BufferString *str, const char *delimiter, uint32_t argCount, char **tokens);
BufferString *joinStrings(BufferString *str, const char *delimiter, uint32_t argCount, ...);

// repeat
BufferString *repeatChar(BufferString *str, char repeatChar, uint32_t count);
BufferString *repeatChars(BufferString *str, const char *repeatChars, uint32_t count);

// convert
BufferString *int64ToString(BufferString *str, int64_t value);
BufferString *uInt64ToString(BufferString *str, uint64_t value);
StringToI64Status stringToI64(BufferString *str, int64_t *out, int base);
StringToI64Status cStrToInt64(const char *str, int64_t *out, int base);

// check
bool isBuffStrBlank(BufferString *str);
bool isCstrBlank(const char *str);
bool isBuffStrEquals(BufferString *one, BufferString *two);
bool isBuffStrEqualsCstr(BufferString *one, const char *two);
bool isBuffStrEqualsIgnoreCase(BufferString *one, BufferString *two);

// index
int32_t indexOfChar(BufferString *str, char charToFind, uint32_t fromIndex);
int32_t indexOfString(BufferString *str, const char *stringToFind, uint32_t fromIndex);
int32_t lastIndexOfString(BufferString *str, const char *stringToFind);

// index 'char*'
int32_t indexOfCStr(char *str, const char *stringToFind, uint32_t fromIndex);
int32_t lastIndexOfCStr(char *str, const char *stringToFind);

// starts with
bool isStrStartsWith(BufferString *str, const char *prefix, uint32_t toOffset);
bool isStrStartsWithIgnoreCase(BufferString *str, const char *prefix, uint32_t toOffset);

// ends with
bool isStrEndsWith(BufferString *str, const char *suffix);
bool isStrEndsWithIgnoreCase(BufferString *str, const char *suffix);

// additional helper functions
static inline char charAt(BufferString *str, uint32_t index) {
    return (char) ((str == NULL || index >= str->length) ? 0 : str->value[index]);
}

static inline bool containsStr(BufferString *str, const char *searchString) {
    return (str != NULL && searchString != NULL && strstr(str->value, searchString) != NULL);
}

static inline bool isCstrEmpty(const char *str) {
    return (str == NULL || str[0] == '\0');
}

static inline bool isCstrNotEmpty(const char *str) {
    return !isCstrEmpty(str);
}

static inline bool isBuffStringEmpty(BufferString *str) {
    return str == NULL || isCstrEmpty(str->value);
}

static inline bool isBuffStringNotEmpty(BufferString *str) {
    return !isBuffStringEmpty(str);
}

static inline bool isBuffStringNotBlank(BufferString *str) {
    return !isBuffStrBlank(str);
}

static inline bool isBuffStringNotEquals(BufferString *one, BufferString *two) {
    return !isBuffStrEquals(one, two);
}

// properties
static inline char *stringValue(BufferString *str) { return str != NULL ? str->value : NULL; }
static inline uint32_t stringLength(BufferString *str) { return str != NULL ? str->length : 0; }
static inline uint32_t stringCapacity(BufferString *str) { return str != NULL ? str->capacity : 0; }