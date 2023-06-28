# BufferString

[![tests](https://github.com/ximtech/BufferString/actions/workflows/cmake-ci.yml/badge.svg?branch=main)](https://github.com/ximtech/BufferString/actions/workflows/cmake-ci.yml)
[![codecov](https://codecov.io/gh/ximtech/BufferString/branch/main/graph/badge.svg?token=oWE7rYTzQS)](https://codecov.io/gh/ximtech/BufferString)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/cc8f50c5f20a4f7d94990f507071a6cf)](https://www.codacy.com/gh/ximtech/BufferString/dashboard)

**forked from:** [Simple Buffer String (SBS)](https://github.com/nathanrpage97/sbs)

**BufferString** is a string library for C designed to augment standard `<string.h>` handling functionalities. \
Most usefully functions partition to C from Java `java.lang.String` and Apache `org.apache.commons.lang3.StringUtils`
. \
Specifically designed for embedded applications

### Features

- No static or dynamic memory allocations
- No need to free resources
- Easy and safe string manipulation
- Lightweight design
- Advanced string formatting
- No external dependencies
- Single include
- Easy to use

### Tradeoffs

- Static string size
- Char buffer should be accessed via `stringValue()`
- Can't be passed directly to C like `printf` or `scanf`, without accessing a struct member

### Add as CPM project dependency

How to add CPM to the project, check the [link](https://github.com/cpm-cmake/CPM.cmake)

```cmake
CPMAddPackage(
        NAME BufferString
        GITHUB_REPOSITORY ximtech/BufferString
        GIT_TAG origin/main
        OPTIONS
        "ENABLE_FLOAT_FORMATTING ON"
)

target_link_libraries(${PROJECT_NAME} BufferString)
```

```cmake
add_executable(${PROJECT_NAME}.elf ${SOURCES} ${LINKER_SCRIPT})
# For Clion STM32 plugin generated Cmake use 
target_link_libraries(${PROJECT_NAME}.elf BufferString)
```

**NOTE:** By default floating point number format disabled

## Usage

### Single header include

```c
#include "BufferString.h"
```

### Base string creation

```c
BufferString *str1 = NEW_STRING(16, "Hello World!"); // the size must be a literal
BufferString *str2 = NEW_STRING_16("even shorter");
BufferString *str3 = NEW_STRING_2048("2048 char size");

printf("%s\n", stringValue(str1));   // use property function to get inner buffer

Output: Hello World!
```

**NOTE:** Check other format string sizes in `BufferString.h`

### Other ways to create `BufferString`

```c
char rawData[] = { 65, 65, 0x0, 0x2 };
BufferString *dataStr = NEW_STRING_LEN(32, rawData, sizeof(rawData));   // new string by length

BufferString *emptyStr = EMPTY_STRING(64);  // empty string with capacity

BufferString *sourceStr = NEW_STRING_32("some text");
BufferString *copyStr = DUP_STRING(64, sourceStr);  // copy of original string

BufferString *str = STRING_FORMAT_64("%s", "Text"); // from formatted string

static buffer[128] = {0};
BufferString *buffStr = NEW_STRING_BUFF(buffer, "abcd");    // from static or global buffer
```

The creation functions all return either `BufferString` pointer or `NULL` if there is a failure.
All function have `NULL` check and also return `NULL` if something goes wrong.

### String concatenation

```c
BufferString *str = NEW_STRING_64("The quick brown ");

str = concatCharsByLength(str, "fox and cat", 4); // concat only firs 4 chars
str = concatChars(str, "jumps over");   // expects a null terminated string
str = concatString(str, NEW_STRING_64(" the lazy dog"));    // from other BufferString
str = concatChar(str, '!');   // concat single char

printf("%s\n", stringValue(str));

Output: The quick brown fox jumps over the lazy dog!
```

### Clear string

```c
BufferString *str = NEW_STRING_32("Some text");
clearString(str);
printf("[%s]", stringValue(str));

Output:[]
```

### To lower case string

```c
BufferString *str = NEW_STRING_32("aBc");
toLowerCase(str);
printf("%s", stringValue(str));

Output: abc
```

### To upper case string

```c
BufferString *str = NEW_STRING_32("aBc");
toUpperCase(str);
printf("%s", stringValue(str));

Output: ABC
```

### Swap case

Swaps the case of a `BufferString` changing upper to lower case, and lower case to upper case

```c
BufferString *str = NEW_STRING_32("The dog has a BONE");
swapCase(str);
printf("%s", stringValue(str));

Output: tHE DOG HAS A bone
```

### Replace first occurrence

```c
BufferString *str = NEW_STRING_32("abc abc");
replaceFirstOccurrence(str, "abc", "cba");
printf("%s", stringValue(str));

Output: cba abc
```

### Replace all occurrences

```c
BufferString *str = NEW_STRING_32("abc abc");
replaceAllOccurrences(str, "abc", "cba");
printf("%s", stringValue(str));

Output: cba cba
```

### Trim string

Removes control characters (char <= 32) from both ends of provided `BufferString`

```c
BufferString *str = NEW_STRING_32("         my string\n\n  ");
trimAll(str);
printf("%s", stringValue(str));

Output: my string
```

### Reverse string

```c
BufferString *str = NEW_STRING_32("bat");
reverseString(str);
printf("%s", stringValue(str));

Output: tab
```

### Capitalize string

With default separator

```c
BufferString *str = NEW_STRING_32("test message");
capitalize(str, NULL, 0);   // default whitespace separator will be used
printf("%s", stringValue(str));

Output: Test Message
```

Custom separators

```c
BufferString *str = NEW_STRING_32("i aM.fine");
char delims[] = {'.'};
capitalize(str, delims, 1);
printf("%s", stringValue(str));

Output: I aM.Fine
```

## Substring

### Substring from index

```c
BufferString *source = NEW_STRING_32("abc");
BufferString *destination = EMPTY_STRING(32);
substringFrom(source, destination, 0);  // "abc"
substringFrom(source, destination, 2);  // "c"
substringFrom(source, destination, 4);  // ""
```

**NOTE:** For `char*` use `substringCStrFrom()`

### Substring in range

```c
BufferString *source = NEW_STRING_32("abc");
BufferString *destination = EMPTY_STRING(32);
substringFromTo(source, destination, 0, 2);  // "ab"
substringFromTo(source, destination, 2, 3);  // "c"
substringFromTo(source, destination, 4, 6);  // ""
```

#### Short version

```c
BufferString *source = NEW_STRING_32("abc");
BufferString *destination = SUBSTRING(32, source, 1, 2);    // "b"
```

**NOTE:** For `char*` use `substringCStrFromTo()` or `SUBSTRING_CSTR` macro

### Substring after

Return the substring after the first occurrence of a separator

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = EMPTY_STRING(32);
substringAfter(source, destination, "a");  // "bcba"
substringAfter(source, destination, "b");  // "cba"
substringAfter(source, destination, "");  // "abcba"
```

#### Short version

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = SUBSTRING_AFTER(32, source, "cb"); // "a"
```

**NOTE:** For `char*` use `substringCStrAfter()` or `SUBSTRING_CSTR_AFTER` macro

### Substring after last

Return the substring after the last occurrence of a separator

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = EMPTY_STRING(32);
substringAfterLast(source, destination, "a");  // ""
substringAfterLast(source, destination, "b");  // "a"
substringAfterLast(source, destination, "ab");  // "cba"
```

#### Short version

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = SUBSTRING_AFTER_LAST(32, source, "b"); // "a"
```

**NOTE:** For `char*` use `substringCStrAfterLast()` or `SUBSTRING_CSTR_AFTER_LAST` macro

### Substring before

Return the substring before the first occurrence of a separator

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = EMPTY_STRING(32);
substringBefore(source, destination, "a");  // ""
substringBefore(source, destination, "c");  // "ab"
substringBefore(source, destination, "ba");  // "abc"
```

#### Short version

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = SUBSTRING_BEFORE(32, source, "c"); // "ab"
```

**NOTE:** For `char*` use `substringCStrBefor()` or `SUBSTRING_CSTR_BEFORE` macro

### Substring before last

Return the substring before the last occurrence of a separator

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = EMPTY_STRING(32);
substringBeforeLast(source, destination, "b");  // "abc"
substringBeforeLast(source, destination, "a");  // "abcb"
substringBeforeLast(source, destination, "z");  // "abcba"
```

#### Short version

```c
BufferString *source = NEW_STRING_32("abcba");
BufferString *destination = SUBSTRING_BEFORE_LAST(32, source, "a"); // "abcb"
```

**NOTE:** For `char*` use `substringCStrBeforeLast()` or `SUBSTRING_CSTR_BEFORE_LAST` macro

### Substring between

Return the `BufferString` that is nested in between two strings

```c
BufferString *source = NEW_STRING_32("yabcz");
BufferString *destination = EMPTY_STRING(32);
substringBetween(source, destination, "", "");  // ""
substringBetween(source, destination, "y", "z");  // "abc"
```

#### Short version

```c
BufferString *source = NEW_STRING_32("yabczyabcz");
BufferString *destination = SUBSTRING_BETWEEN(32, source, "y", "z"); // "abc"
```

**NOTE:** For `char*` use `substringCStrBetween()` or `SUBSTRING_CSTR_BETWEEN` macro

## Split string

Splitting a larger string into smaller strings. \
**NOTE:** Original string is not changed

```c
BufferString *str = NEW_STRING_64("/api/test/json/product=1234/utm_source");
BufferString *token = EMPTY_STRING(15);

StringIterator iterator = getStringSplitIterator(str, "/");
while (hasNextSplitToken(&iterator, token)) {
    printf("[%s]\n", stringValue(token));
}
```

#### Output

```text
    []
    [api]
    [test]
    [json]
    [product=1234]
    [utm_source]
```

## Join string

Joins the elements of the provided array into a single `BufferString` containing the provided list of elements

### Join chars

```c
BufferString *str = EMPTY_STRING(64);
joinChars(str, "|", 3, "foo", "bar", "zap");
printf("%s", stringValue(str));

Output: foo|bar|zap
```

### Join string array

```c
BufferString *str = EMPTY_STRING(64);
char *tokens[3] = { "foo", "bar", "zap" };
joinStringArray(str, "|", 3, tokens);
printf("%s", stringValue(str));

Output: foo|bar|zap
```

### Join `BufferString`

```c
BufferString *str = EMPTY_STRING(64);
joinStrings(str, "|", 3, NEW_STRING_16("foo"), NEW_STRING_16("bar"), NEW_STRING_16("zap"));
printf("%s", stringValue(str));
    
Output: foo|bar|zap
```

## Repeat

### Repeat char

Returns padding using the specified delimiter repeated to a given length

```c
BufferString *str = EMPTY_STRING(64);
repeatChar(str, 'e', 0);    // ""
repeatChar(str, 'e', 3);    // "eee"
```

### Repeat string

```c
BufferString *str = EMPTY_STRING(64);
repeatChars(str, "", 0);    // ""
repeatChars(str, "", 2);    // ""
repeatChars(str, "a", 3);    // "aaa"
repeatChars(str, "ab", 2);    // "abab"
```

## Fast number to string conversion and vice versa

`int64_t` and `uint64_t` to `BufferString`

```c
BufferString *str = EMPTY_STRING(64);
int64ToString(str, -10000); // "-10000"
uInt64ToString(str, 10000); // "10000"
```

`BufferString` to `int64_t`

```c
BufferString *str = NEW_STRING_32("123456789");
int64_t result = 0;
StringToI64Status status = stringToI64(str, &result, 10);
if (status == STR_TO_I64_SUCCESS) {
    printf("%lld\n", result);   // 123456789
}
```

For `char *` use `cStrToInt64()` instead

## Check functions

Function `isBuffStrBlank()` checks if a `char` sequence is empty (""), null or whitespace only.

```c
isBuffStrBlank(NULL);                        // true
isBuffStrBlank(NEW_STRING_16(""));           // true
isBuffStrBlank(NEW_STRING_16("  "));         // true
isBuffStrBlank(NEW_STRING_16("bob"));        // false
isBuffStrBlank(NEW_STRING_16("  bob  "));    // false
```

Function `isBuffStrEquals()` compares two `char` sequences, returning true if they represent equal sequences of
characters.

```c
isBuffStrEquals(NULL, NULL); // true
isBuffStrEquals(NULL, NEW_STRING_16("abc")); // false
isBuffStrEquals(NEW_STRING_16("abc"), NULL); // false
isBuffStrEquals(NEW_STRING_16("abc"), NEW_STRING_16("abc")); // true
isBuffStrEquals(NEW_STRING_16("abc"), NEW_STRING_16("ABC")); // false
```

Function `` compares two `char` sequences, returning true if they represent equal sequences of characters, ignoring
case.

```c
isBuffStrEqualsIgnoreCase(NULL, NULL); // true
isBuffStrEqualsIgnoreCase(NULL, NEW_STRING_16("abc")); // false
isBuffStrEqualsIgnoreCase(NEW_STRING_16("abc"), NULL); // false
isBuffStrEqualsIgnoreCase(NEW_STRING_16("abc"), NEW_STRING_16("abc")); // true
isBuffStrEqualsIgnoreCase(NEW_STRING_16("abc"), NEW_STRING_16("ABC")); // true
```

## Index of char or string

Function `indexOfChar()` finds the first index of `char` within a `BufferString`, starting at the specified index.

```c
indexOfChar(NEW_STRING_16("aabaabaa"), 'a', 0);  // 0
indexOfChar(NEW_STRING_16("aabaabaa"), 'b', 0);  // 2
indexOfChar(NEW_STRING_16("aabaabaa"), 'b', 3);  // 5
indexOfChar(NEW_STRING_16("aabaabaa"), 'b', 9);  // -1
```

Same as `indexOfChar()`, function `indexOfString()` finds index of a string, handling `NULL`

```c
indexOfString(NULL, "*", 0);  // -1
indexOfString(NEW_STRING_16("aabaabaa"), NULL, 0);  // -1
indexOfString(NEW_STRING_16("aabaabaa"), "a", 0);  // 0
indexOfString(NEW_STRING_16("aabaabaa"), "b", 0);  // 2
indexOfString(NEW_STRING_16("aabaabaa"), "ab", 0);  // 1
indexOfString(NEW_STRING_16("aabaabaa"), "b", 3);  // 5
indexOfString(NEW_STRING_16("aabaabaa"), "b", 9);  // -1
indexOfString(NEW_STRING_16("aabaabaa"), "", 2);  // 2
indexOfString(NEW_STRING_16("aabaabaa"), "", 9);  // -1
```

**NOTE:** For `char*` use `indexOfCStr()`

Function `lastIndexOfString()` finds the last index within a `char` sequence, handling `NULL`

```c
lastIndexOfString(NULL, "*");  // -1
lastIndexOfString(NEW_STRING_16("aabaabaa"), NULL);  // -1
lastIndexOfString(NEW_STRING_16("aabaabaa"), "a");  // 7
lastIndexOfString(NEW_STRING_16("aabaabaa"), "b");  // 5
lastIndexOfString(NEW_STRING_16("aabaabaa"), "ab");  // 4
lastIndexOfString(NEW_STRING_16("aabaabaa"), "b");  // 5
lastIndexOfString(NEW_STRING_16("aabaabaa"), "");  // 8
```

**NOTE:** For `char*` use `lastIndexOfCStr()`

## BufferString starts with

Function `isStringStartsWith()` tests if the substring of `BufferString` beginning at the specified index starts with
the specified prefix string

```c
isStringStartsWith(NULL, NULL, 0);  // false
isStringStartsWith(NULL, "abc", 0);  // false
isStringStartsWith(NEW_STRING_16("aabaabaa"), NULL, 0); // false
isStringStartsWith(NEW_STRING_16("aabaabaa"), "aa", 0);  // true
isStringStartsWith(NEW_STRING_16("ABCDEF"), "a", 0);  // false
isStringStartsWith(NEW_STRING_16("aabaabaa"), "aa", 1); // false
```

Function `isStringStartsWithIgnoreCase()` same functionality as `isStringStartsWith()`, but case-insensitive

```c
isStringStartsWithIgnoreCase(NULL, NULL, 0);  // false
isStringStartsWithIgnoreCase(NULL, "abc", 0);  // false
isStringStartsWithIgnoreCase(NEW_STRING_16("aabaabaa"), NULL, 0); // false
isStringStartsWithIgnoreCase(NEW_STRING_16("aabaabaa"), "aa", 0);  // true
isStringStartsWithIgnoreCase(NEW_STRING_16("ABCDEF"), "a", 0);  // true
isStringStartsWithIgnoreCase(NEW_STRING_16("ABCDEF"), "def", 3);  // true
isStringStartsWithIgnoreCase(NEW_STRING_16("aabaabaa"), "aa", 1); // false
```

## Additional helper functions

Function `charAt()`. Returns the `char` value at the specified index. \
An index ranges from 0 to `stringLength() - 1`. The first char value of the sequence is at index 0, the next at index 1,
and so on, as for array indexing.

```c
charAt(NULL, 0); // '\0'
charAt(NEW_STRING_16("aabaabaa"), 0); // 'a'
charAt(NEW_STRING_16("aabaabaa"), 2); // 'b'
charAt(NEW_STRING_16("aabaabaa"), 9); // '\0'
```

Function `containsStr()`. Returns true if and only if `BufferString` contains the specified sequence of char values.

```c
containsStr(NULL, "*");                 // false
containsStr(NEW_STRING_16("*"), NULL);   // false
containsStr(NEW_STRING_16(""), "");   // true
containsStr(NEW_STRING_16("abc"), "");   // true
containsStr(NEW_STRING_16("abc"), "a");   // true
containsStr(NEW_STRING_16("abc"), "z");   // false
containsStr(NEW_STRING_16("ABCDEF"), "def");   // false
containsStr(NEW_STRING_16("ABCDEF"), "DEF");   // true
```

Function `isBuffStringEmpty()`. Checks if a `StringBuffer` is empty ("") or null

```c
isBuffStringEmpty(NULL);    // true
isBuffStringEmpty(NEW_STRING_16(""));    // true
isBuffStringEmpty(NEW_STRING_16(" "));    // false
isBuffStringEmpty(NEW_STRING_16("bob"));    // false
isBuffStringEmpty(NEW_STRING_16(" bob "));    // false
```

Function `isBuffStringNotEmpty()`. Checks if a `StringBuffer` is not empty ("") or null

```c
isBuffStringNotEmpty(NULL);    // false
isBuffStringNotEmpty(NEW_STRING_16(""));    // false
isBuffStringNotEmpty(NEW_STRING_16(" "));    // true
isBuffStringNotEmpty(NEW_STRING_16("bob"));    // true
isBuffStringNotEmpty(NEW_STRING_16(" bob "));    // true
```

Function `isBuffStringNotBlank()`. Checks if a `StringBuffer` is not empty (""), not null and not whitespace only.

```c
isBuffStringNotBlank(NULL);    // false
isBuffStringNotBlank(NEW_STRING_16(""));    // false
isBuffStringNotBlank(NEW_STRING_16(" "));    // false
isBuffStringNotBlank(NEW_STRING_16("bob"));    // true
isBuffStringNotBlank(NEW_STRING_16(" bob "));    // true
```

Function `isBuffStringNotEquals()`. Compares two `StringBuffer` strings, returning `false` if they represent equal
sequences of characters.

```c
isBuffStringNotEquals(NULL, NULL);    // false
isBuffStringNotEquals(NULL, NEW_STRING_16(""));    // true
isBuffStringNotEquals(NEW_STRING_16(""), NULL);    // true
isBuffStringNotEquals(NEW_STRING_16(""), NEW_STRING_16(""));    // false
isBuffStringNotEquals(NEW_STRING_16("a"), NEW_STRING_16("b"));    // true
```

### Properties

Although the structure is transparent, it is best practice to prefer the property functions.

1. `stringValue()`: returns the chars/bytes
2. `stringLength()`: returns the length of the chars/bytes
3. `stringCapacity()`: returns the size of the buffer


## BufferString Format

### Create new by format

```c
BufferString *str1 = STRING_FORMAT_16("%s", "small string");    // "small string"
BufferString *str2 = STRING_FORMAT_128("%s: %d", "Big with capacity", 128); // "Big with capacity: 128"
BufferString *str3 = STRING_FORMAT_2048("%s: %d", "Large buffer", 2048);    // "Large buffer: 2048"
```

**NOTE:** Check other format string sizes in `BufferString.h`

## Format Syntax

The syntax for a format placeholder is: 
```text
%[flags][width][.precision][length]type
```

### Supported Standard Types

The following format specifiers are supported:

| Type   | Output                                                    |
|--------|-----------------------------------------------------------|
| d or i | Signed decimal integer                                    |
| u      | Unsigned decimal integer                                  |
| b      | Unsigned binary                                           |
| o      | Unsigned octal                                            |
| x      | Unsigned hexadecimal integer (lowercase)                  |
| X      | Unsigned hexadecimal integer (uppercase)                  |
| f or F | Decimal floating point                                    |
| e or E | Scientific-notation (exponential) floating point          |
| g or G | Scientific or decimal floating point                      |
| c      | Single character                                          |
| s      | String of characters                                      |
| p      | Pointer address                                           |
| %      | A % followed by another % character will write a single % |

### Additional Custom Types

| Type | Output                             |
|------|------------------------------------|
| S    | `BufferString` pointer             |
| n    | New line or '\n'                   |
| I8   | Signed byte or `int8_t`            |
| U8   | Unsigned byte or `uint8_t`         |
| I16  | Signed half word of `int16_t`      |
| U16  | Unsigned half word of `uint16_t`   |
| I32  | Signed word or `int32_t`           |
| U32  | Unsigned word or `uint32_t`        |
| I64  | Signed double word or `int64_t`    |
| U64  | Unsigned double word or `uint64_t` |

### Supported Flags

| Flags   | Description                                                                                                                                                                                                                                                                                      |
|---------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| -       | Left-justify within the given field width; Right justification is the default.                                                                                                                                                                                                                   |
| +       | Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers.<br>By default, only negative numbers are preceded with a - sign.                                                                                                                                      |
| (space) | If no sign is going to be written, a blank space is inserted before the value.                                                                                                                                                                                                                   |
| #       | Used with o, b, x or X specifiers the value is preceded with 0, 0b, 0x or 0X respectively for values different than zero.<br>Used with f, F it forces the written output to contain a decimal point even if no more digits follow. By default, if no digits follow, no decimal point is written. |
| 0       | Left-pads the number with zeros (0) instead of spaces when padding is specified (see width sub-specifier).                                                                                                                                                                                       |


### Supported Width

| Width    | Description                                                                                                                                                                                          |
|----------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| (number) | Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces. The value is not truncated even if the result is larger. |
| *        | The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.                                                        |


### Supported Precision

| Precision	 | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    |
|------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| .number    | For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number, the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.<br>For f and F specifiers: this is the number of digits to be printed after the decimal point. **By default, this is 6, maximum is 9**.<br>For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.<br>If the period is specified without an explicit value for precision, 0 is assumed. |
| .*         | The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |


### Supported Length

The length sub-specifier modifies the length of the data type.

| Length | d i                    | u o x X                |
|--------|------------------------|------------------------|
| (none) | int                    | unsigned int           |
| hh     | char                   | unsigned char          |
| h      | short int              | unsigned short int     |
| l      | long int               | unsigned long int      |
| ll     | long long int          | unsigned long long int |
| I8     | signed char            | unsigned short int     |
| U8     | unsigned char          | unsigned short int     |
| I16    | signed short           | unsigned short int     |
| U16    | unsigned short         | unsigned short int     |
| I32    | signed int             | unsigned long int      |
| U32    | unsigned int           | unsigned long int      |
| I64    | signed long long int   | unsigned long long int |
| U64    | unsigned long long int | unsigned long long int |


## Library Defines/Defaults

| Name                           | Default value | Description                                                                                 |
|--------------------------------|---------------|---------------------------------------------------------------------------------------------|
| ENABLE_FLOAT_FORMATTING        | undefined     | Define this to enable floating point (%f) and exponential floating point (%e) support       |
| FORMAT_DEFAULT_FLOAT_PRECISION | 6             | Default floating point precision. Can't be changed                                          |
| FORMAT_MAX_FLOAT_VALUE         | 1e9           | Default the largest value for %f, before using exponential representation. Can't be changed |


## Some examples

```c
BufferString *str1 = STRING_FORMAT_128("[%s]", "Hello World");              // "Hello World"
BufferString *str2 = STRING_FORMAT_128("[%S]", NEW_STRING_16("Hello World"));   // "Hello World"
BufferString *str3 = STRING_FORMAT_128("[%7d %7d %7d]", 1234, 5678, 91011);     // [   1234    5678   91011]
BufferString *str4 = STRING_FORMAT_128("[%U8]", 123);               // [123]
BufferString *str5 = STRING_FORMAT_128("[%U32]", 123456789);     // [123456789]
BufferString *str6 = STRING_FORMAT_128("[%f]", 1234.56789);     // [1234.567890]
BufferString *str7 = STRING_FORMAT_128("[%e]", 1234.56789);     // [1.234568e+003]
```
