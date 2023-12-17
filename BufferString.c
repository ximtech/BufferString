#include "BufferString.h"

#define TERMINATE_STRING(s) (s)->value[(s)->length] = '\0'
#define STRING_END(s) ((s)->value + (s)->length)
#define UINT64_DIGITS_MAX_COUNT 20

#define BIT_READ(value, bit) (((value) >> (bit)) & 0x01)
#define BIT_SET(value, bit) ((value) |= (1UL << (bit)))
#define BIT_CLEAR(value, bit) ((value) &= ~(1UL << (bit)))

#define SET_FLAG(flags, flag) BIT_SET(flags, flag)
#define IS_FLAG_SET(flags, flag) (BIT_READ(flags, flag)) == true
#define IS_FLAG_NOT_SET(flags, flag) (BIT_READ(flags, flag)) == false

#define BIN_BASE 2
#define OCT_BASE 8
#define DEC_BASE 10
#define HEX_BASE 16

#define NO_SIGN 0
#define NO_RESULT (-1)
#define HEX_SIZE 2
#define NAN_LENGTH 3
#define INF_LENGTH 3
#define INF_WITH_SIGN_LENGTH (INF_LENGTH + 1)

#define LENGTH_FIELD_MAX_SIZE 2
#define SKIP_ONE_CHAR 1
#define SKIP_TWO_CHARS 2
#define FORMAT_NUMBER_BUFFER_SIZE 66
#define POINTER_DEFAULT_WIDTH (sizeof(void *) * 2)

#define IS_INT_8(length) ((length)[0] == '8')
#define IS_INT_16(length) ((length)[0] == '1' && (length)[1] == '6')
#define IS_INT_64(length) ((length)[0] == '6' && (length)[1] == '4')

#ifdef ENABLE_FLOAT_FORMATTING
#define FORMAT_FLOAT_BUFFER_SIZE 32
#define FORMAT_MAX_FLOAT_VALUE 1e9
#define FORMAT_DEFAULT_FLOAT_PRECISION 6
#define FORMAT_MAX_FLOAT_PRECISION 9

#define MANTISSA_BITS 52
#define EXPONENT_BITS 11
#define SIGN_BITS 1
#define DOUBLE_EXPONENT_ZERO_VALUE 1023 // The exponent field is an 11-bit unsigned integer from 0 to 2047, in biased form: an exponent value of 1023 represents the actual zero.

#define LOG_APPROXIMATION 0.289529654602168
#define LOG10_OF_2 0.301029995663981    // log10(2) constant
#define LOG2_OF_10 3.321928094887362    // log2(10) constant

// approximated constant term log 10( 1.5 ) = 0.176091259055681242... by 0.1760912590558
// to ensure kˆ ≥ k; it is easy to see from Taylor’s theorem that kˆ ≤ k + 1.
#define LOG10_OF_1_5 0.1760912590558

#define EXPONENT_WIDTH_DEFAULT_VALUE 4
#define NATURAL_LOG_OF_10 2.302585092994046     // ln(10)
#define NATURAL_LOG_OF_2 0.6931471805599453     // ln(2)

typedef union DoubleCast {
    double decimal;
    struct {
        uint64_t mantissa: MANTISSA_BITS;
        uint64_t exponent: EXPONENT_BITS;
        uint64_t sign: SIGN_BITS;
    } parts;
} DoubleCast;
#endif

typedef enum FormatFlagField {
    LEFT_ALIGN_FLAG,      // '-' -> Left-align the output of this placeholder. (The default is to right-align the output.)
    PLUS_FLAG,            // '+' -> Prepends a plus for positive signed-numeric types. positive = +, negative = -. (The default doesn't prepend anything in front of positive numbers.)
    SPACE_FLAG,           // ' ' -> Prepends a space for positive signed-numeric types. positive =  , negative = -. This flag is ignored if the + flag exists.
    ZEROES_PADDING_FLAG,  // '0' -> When the 'width' option is specified, prepends zeros for numeric types. (The default prepends spaces.)
    SPECIAL_FLAG,         // '#' -> For g and G types, trailing zeros are not removed. For f, F, e, E, g, G types, the output always contains a decimal point. For o, x, X types, the text 0, 0x, 0X, respectively, is prepended to non-zero numbers.
    LOWER_CASE_FLAG,      // 'x' -> unsigned int as a hexadecimal number. x uses lower-case letters and X uses upper-case. 'f' and 'F' only differs in how the strings for an infinite number or NaN are printed (inf, infinity and nan for f; INF, INFINITY and NAN for F).
    SIGNED_NUMBER_FLAG,   // unsigned/signed long flag
    ADAPTIVE_EXPONENT_FLAG,  // flag for: '%g' that represents the decimal format of the answer, depending upon whose length is smaller, comparing between %e and %f.
} FormatFlagField;

static uint32_t isDelimiterChar(char valueChar, const char *delimiters, uint32_t length);
static uint8_t parseFormatFlags(const char *format, uint8_t *flags);
static uint8_t parseFormatFieldWith(const char *format, va_list *vaList, int32_t *widthField, uint8_t *flags);
static uint8_t parseFormatPrecision(const char *format, va_list *vaList, int32_t *precision);
static uint8_t parseLengthField(char *lengthField, const char *format);

static BufferString *formatCharacter(BufferString *str, uint8_t flags, int32_t widthField, va_list *vaList);
static BufferString *formatChars(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, va_list *vaList);
static BufferString *formatString(BufferString *str, uint8_t flags, int32_t widthField, int64_t precision, va_list *vaList);
static BufferString *doFormatChars(BufferString *str, const char *valueStr, uint32_t length, uint8_t flags, int32_t widthField);
static BufferString *formatPointer(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint64_t pointerAddress);
static BufferString *formatNumber(BufferString *str, uint8_t flags, const char *lengthField, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);

#ifdef ENABLE_FLOAT_FORMATTING
static BufferString *formatFloat(BufferString *str, double decimalValue, uint8_t flags, int32_t widthField, int32_t precision);
static BufferString *formatExponential(BufferString *str, double decimalValue, uint8_t flags, int32_t widthField, int32_t precision);
#endif

static inline BufferString *formatByte(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);
static inline BufferString *formatShort(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);
static inline BufferString *formatLongLong(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);
static inline BufferString *formatLong(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);
static inline BufferString *formatInt(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList);

static BufferString *numberToString(BufferString *str, uint64_t number, char sign, uint8_t base, int32_t size, int32_t precision, uint8_t flags);
static uint8_t stringToNumber(const char *numberStr, int32_t *resultValue);
static BufferString *concatLeftPadding(BufferString *str, char *sign, uint8_t base, int32_t *size, uint8_t flags);
static inline BufferString *concatSpecialIfPresent(BufferString *str, uint8_t base, uint8_t flags);
static inline BufferString *concatSignIfPresent(BufferString *str, char sign);
static inline BufferString *concatRightPadding(BufferString *str, int32_t size);
static int32_t numberToStringByBase(uint64_t number, char *numberBuffer, uint8_t base, uint8_t flags);
static char resolveSign(int64_t *number, uint8_t flags, int32_t *widthField);

#ifdef ENABLE_FLOAT_FORMATTING
static bool isNanOrInfinity(BufferString *str, double decimalValue, int32_t widthField, uint8_t flags);
#endif


BufferString *newStringWithLength(BufferString *str, const void *initValue, uint32_t initLength, char *buffer, uint32_t bufferLength) {
    if (str == NULL || initLength >= bufferLength) return NULL;
    str->value = buffer;
    str->length = initLength;
    str->capacity = bufferLength;
    memcpy(str->value, initValue, initLength);
    TERMINATE_STRING(str);
    return str;
}

BufferString *newString(BufferString *str, const void *initValue, char *buffer, uint32_t bufferLength) {
    if (initValue == NULL || buffer == NULL) return NULL;
    return newStringWithLength(str, initValue, strnlen(initValue, bufferLength), buffer, bufferLength);
}

BufferString *dubString(BufferString *source, BufferString *dest, char *buffer, uint32_t bufferLength) {
    return newStringWithLength(dest, source->value, source->length, buffer, bufferLength);
}

BufferString *stringFormat(BufferString *str, const char *format, ...) {
    if (str == NULL || format == NULL) return NULL;
    clearString(str);
    va_list vaList;
    va_start(vaList, format);

    for (; str != NULL && *format != '\0'; format++) {
        if (*format != '%') {
            str = concatChar(str, *format);
            continue;
        }

        format++;   // skip also '%'
        uint8_t flags = 0;
        format += parseFormatFlags(format, &flags);

        int32_t widthField = NO_RESULT;
        format += parseFormatFieldWith(format, &vaList, &widthField, &flags);

        int32_t precisionField = NO_RESULT;
        format += parseFormatPrecision(format, &vaList, &precisionField);

        char lengthField[LENGTH_FIELD_MAX_SIZE] = {0};
        format += parseLengthField(lengthField, format);

        uint8_t base = DEC_BASE;    // default base
        switch (*format) {
            case 'c':
                str = formatCharacter(str, flags, widthField - 1, &vaList);
                continue;
            case 's':
                str = formatChars(str, flags, widthField, precisionField, &vaList);
                continue;
            case 'S':
                str = formatString(str, flags, widthField, precisionField, &vaList);
                continue;
            case 'p':
                str = formatPointer(str, flags, widthField, precisionField, (uintptr_t) va_arg(vaList, void *));
                continue;
            case 'n':   // Print nothing, but writes the number of characters written so far into an integer pointer parameter.
                str = concatChar(str, '\n');    // BufferString holds string length, so no need to count this. Just add new line like in Java 
                continue;
            case '%':
                str = concatChar(str, '%');
                continue;

            case 'o':
                base = OCT_BASE;
                break;
            case 'b':
                base = BIN_BASE;
                break;
            case 'x':
                SET_FLAG(flags, LOWER_CASE_FLAG);
                // fall through
            case 'X':
                base = HEX_BASE;
                break;

            case 'd':
            case 'i':
                SET_FLAG(flags, SIGNED_NUMBER_FLAG);
                break;
            case 'u':
                break;

            case 'I':
                SET_FLAG(flags, SIGNED_NUMBER_FLAG);
                format += IS_INT_8(lengthField) ? SKIP_ONE_CHAR : SKIP_TWO_CHARS;
                break;
            case 'U':
                format += IS_INT_8(lengthField) ? SKIP_ONE_CHAR : SKIP_TWO_CHARS;
                break;

                #ifdef ENABLE_FLOAT_FORMATTING
            case 'f':
                SET_FLAG(flags, LOWER_CASE_FLAG);
            case 'F':
                str = formatFloat(str, va_arg(vaList, double), flags, widthField, precisionField);
                continue;

            case 'e':
                SET_FLAG(flags, LOWER_CASE_FLAG);
            case 'E':
                str = formatExponential(str, va_arg(vaList, double), flags, widthField, precisionField);
                continue;

            case 'g':
                SET_FLAG(flags, LOWER_CASE_FLAG);
            case 'G':
                SET_FLAG(flags, ADAPTIVE_EXPONENT_FLAG);
                str = formatExponential(str, va_arg(vaList, double), flags, widthField, precisionField);
                continue;
                #endif

            default:    // unknown char, just concatenate as is
                str = concatChar(str, *format);
                continue;
        }

        str = formatNumber(str, flags, lengthField, widthField, precisionField, base, &vaList);
    }

    va_end(vaList);
    return str;
}

BufferString *concatCharsByLength(BufferString *str, const char *strToConcat, uint32_t length) {
    if (str == NULL || length >= (str->capacity - str->length)) return NULL;
    memcpy(STRING_END(str), strToConcat, length);
    str->length += length;
    TERMINATE_STRING(str);
    return str;
}

BufferString *concatChars(BufferString *str, const char *strToConcat) {
    return str != NULL && strToConcat != NULL ? concatCharsByLength(str, strToConcat, strnlen(strToConcat, str->capacity)) : NULL;
}

BufferString *concatString(BufferString *str, BufferString *strToConcat) {
    return str != NULL && strToConcat != NULL ? concatCharsByLength(str, strToConcat->value, strToConcat->length) : NULL;
}

BufferString *copyString(BufferString *str, const char *strToCopy) {
    return copyStringByLength(str, strToCopy, strlen(strToCopy));
}

BufferString *concatChar(BufferString *str, char charToConcat) {
    if (str == NULL || str->length >= (str->capacity - 1)) return NULL;
    *STRING_END(str) = charToConcat;
    str->length++;
    return str;
}

BufferString *copyStringByLength(BufferString *str, const char *strToCopy, uint32_t length) {
    if (str == NULL || length >= str->capacity) return NULL;
    memcpy(str->value, strToCopy, length);
    str->length = length;
    TERMINATE_STRING(str);
    return str;
}

BufferString *clearString(BufferString *str) {
    if (str == NULL || str->length == 0) return str;
    memset(str->value, 0, str->length);
    str->length = 0;
    return str;
}

BufferString *toLowerCase(BufferString *str) {
    for (uint32_t i = 0; i < str->length; i++) {
        str->value[i] = (char) tolower((int) str->value[i]);
    }
    return str;
}

BufferString *toUpperCase(BufferString *str) {
    for (uint32_t i = 0; i < str->length; i++) {
        str->value[i] = (char) toupper((int) str->value[i]);
    }
    return str;
}

BufferString *swapCase(BufferString *str) {
    for (uint32_t i = 0; i < str->length; i++) {
        char valueChar = str->value[i];
        str->value[i] = (char) (islower((int) valueChar) ? toupper((int) valueChar) : tolower((int) valueChar));
    }
    return str;
}

BufferString *replaceFirstOccurrence(BufferString *source, const char *target, const char *replacement) {
    char *sourcePointer = strstr(source->value, target);
    if (sourcePointer == NULL) return NULL;

    uint32_t targetLength = strlen(target);
    uint32_t replacementLength = strlen(replacement);
    uint32_t tailLength = strlen(sourcePointer + targetLength) + 1;

    if (targetLength >= source->capacity) return NULL;
    memmove(sourcePointer + replacementLength, sourcePointer + targetLength, tailLength);
    strncpy(sourcePointer, replacement, replacementLength);
    source->length = strlen(source->value);
    return source;
}

BufferString *replaceAllOccurrences(BufferString *source, const char *target, const char *replacement) {
    while (replaceFirstOccurrence(source, target, replacement) != NULL);
    return source;
}

BufferString *trimAll(BufferString *str) {
    if (str == NULL) return NULL;

    while (isspace((unsigned char) *str->value)) {// Trim leading space
        str->value++;
        str->length--;
    }

    if (*str->value == 0) { // All spaces?
        return str;
    }

    // Trim trailing space
    char *stringEnd = str->value + str->length - 1;
    while (stringEnd > str->value && isspace((unsigned char) *stringEnd)) {
        stringEnd--;
    }

    // Write new null terminator character
    stringEnd[1] = '\0';
    str->length = strlen(str->value);
    return str;
}

BufferString *reverseString(BufferString *str) {
    for (uint32_t i = 0; i < (str->length / 2); i++) {
        char headChar = str->value[i];
        char tailChar = str->value[str->length - i - 1];
        str->value[i] = tailChar;
        str->value[str->length - i - 1] = headChar;
    }
    return str;
}

BufferString *capitalize(BufferString *str, const char *delimiters, uint32_t length) {
    if (isBuffStrBlank(str)) return str;

    if (delimiters == NULL || length == 0) {
        delimiters = " ";
        length = 1;
    }

    bool capitalizeNext = true;
    for (uint32_t i = 0; i < stringLength(str); i++) {

        char valueChar = charAt(str, i);
        if (isDelimiterChar(valueChar, delimiters, length)) {
            capitalizeNext = true;
            continue;
        }

        if (capitalizeNext) {
            str->value[i] = (char) toupper((int) str->value[i]);
            capitalizeNext = false;
        }
    }

    return str;
}

BufferString *substringFrom(BufferString *source, BufferString *destination, uint32_t beginIndex) {
    return substringFromTo(source, destination, beginIndex, source->length);
}

BufferString *substringFromTo(BufferString *source, BufferString *destination, uint32_t beginIndex, uint32_t endIndex) {
    return substringCStrFromTo(source != NULL ? source->value : NULL, destination, beginIndex, endIndex);
}

BufferString *substringAfter(BufferString *source, BufferString *destination, const char *separator) {
    return substringCStrAfter(source != NULL ? source->value : NULL, destination, separator);
}

BufferString *substringAfterLast(BufferString *source, BufferString *destination, const char *separator) {
    return substringCStrAfterLast(source != NULL ? source->value : NULL, destination, separator);
}

BufferString *substringBefore(BufferString *source, BufferString *destination, const char *separator) {
    return substringCStrBefore(source != NULL ? source->value : NULL, destination, separator);
}

BufferString *substringBeforeLast(BufferString *source, BufferString *destination, const char *separator) {
    return substringCStrBeforeLast(source != NULL ? source->value : NULL, destination, separator);
}

BufferString *substringBetween(BufferString *source, BufferString *destination, const char *open, const char *close) {
    return substringCStrBetween(source != NULL ? source->value : NULL, destination, open, close);
}

BufferString *substringCStrFrom(char *source, BufferString *destination, uint32_t beginIndex) {
    return substringCStrFromTo(source, destination, beginIndex, strlen(source));
}

BufferString *substringCStrFromTo(char *source, BufferString *destination, uint32_t beginIndex, uint32_t endIndex) {
    if (source == NULL) return NULL;
    bool isStringNotInBounds = (beginIndex > endIndex || endIndex > strlen(source));
    if (isStringNotInBounds) return NULL;
    uint32_t subLen = (endIndex - beginIndex);
    memmove(destination->value, source + beginIndex, subLen);
    destination->length = subLen;
    TERMINATE_STRING(destination);
    return destination;
}

BufferString *substringCStrAfter(char *source, BufferString *destination, const char *separator) {
    char *substringPointer = strstr(source, separator);
    if (substringPointer == NULL) return destination;
    uint32_t separatorLength = strlen(separator);
    substringPointer += separatorLength;
    return copyStringByLength(destination, substringPointer, strlen(substringPointer));
}

BufferString *substringCStrAfterLast(char *source, BufferString *destination, const char *separator) {
    int32_t position = lastIndexOfCStr(source, separator);
    if (position == NO_RESULT) {
        return destination;
    }
    uint32_t separatorLength = strlen(separator);
    char *substringPointer = source + position + separatorLength;    // move to the last occurrence
    return copyStringByLength(destination, substringPointer, strlen(substringPointer));
}

BufferString *substringCStrBefore(char *source, BufferString *destination, const char *separator) {
    int32_t position = indexOfCStr(source, separator, 0);
    if (position == NO_RESULT) {
        return destination;
    }
    return copyStringByLength(destination, source, position);
}

BufferString *substringCStrBeforeLast(char *source, BufferString *destination, const char *separator) {
    int32_t position = lastIndexOfCStr(source, separator);
    if (position == NO_RESULT) {
        return destination;
    }
    return copyStringByLength(destination, source, position);
}

BufferString *substringCStrBetween(char *source, BufferString *destination, const char *open, const char *close) {
    if (source == NULL || destination == NULL) return NULL;
    char *startPointer = strstr(source, open);
    if (startPointer != NULL) {  // check that substring start is found
        startPointer += strlen(open);
        char *endPointer = strstr(startPointer, close);
        size_t substringLength = endPointer != NULL ? endPointer - startPointer : 0;
        if (substringLength > 0 && substringLength < destination->capacity) {  // check that substring end is found and dest have enough capacity
            strncat(destination->value, startPointer, substringLength);
            destination->length = substringLength;
            return destination;
        }
        return NULL;
    }
    return NULL;
}

StringIterator getStringSplitIterator(BufferString *str, const char *delimiter) {
    StringIterator iterator = {
            .str = str,
            .delimiter = delimiter,
            .delimiterLength = delimiter != NULL ? strlen(delimiter) : 0,
            .nextToken = str->value
    };
    return iterator;
}

bool hasNextSplitToken(StringIterator *iterator, BufferString *token) {
    if (iterator == NULL || iterator->str == NULL || token == NULL) return false;
    char *startPointer = iterator->nextToken;
    if (startPointer == NULL) {
        return false;
    }

    char *endPointer = strstr(startPointer, iterator->delimiter);
    if (endPointer == NULL) {   // check that delimiter exist
        if (iterator->nextToken != NULL &&
            iterator->nextToken != iterator->str->value) {// copy last part only when source string has at least one existing delimiter
            copyStringByLength(token, startPointer, strlen(iterator->nextToken));
            iterator->nextToken = NULL;
            return true;
        }
        return false;
    }

    uint32_t tokenLength = endPointer - startPointer;
    copyStringByLength(token, startPointer, tokenLength);
    startPointer += tokenLength;
    iterator->nextToken = startPointer + iterator->delimiterLength;
    return true;
}

BufferString *joinChars(BufferString *str, const char *delimiter, uint32_t argCount, ...) {
    va_list valist;
    va_start(valist, argCount);
    uint32_t delimiterLength = strlen(delimiter);

    bool isFailedToJoin = false;
    for (uint32_t i = 0; i < argCount; i++) {
        char *argValue = va_arg(valist, char *);
        if (concatChars(str, argValue) == NULL) {
            isFailedToJoin = true;
            break;
        }
        if (i != argCount - 1) {
            concatCharsByLength(str, delimiter, delimiterLength);
        }
    }

    if (isFailedToJoin) {
        TERMINATE_STRING(str);  // restore previous string ending by length
    }
    va_end(valist);
    return str;
}

BufferString *joinStringArray(BufferString *str, const char *delimiter, uint32_t argCount, char **tokens) {
    uint32_t delimiterLength = strlen(delimiter);
    bool isFailedToJoin = false;
    for (uint32_t i = 0; i < argCount; i++) {
        char *argValue = tokens[i];
        if (concatChars(str, argValue) == NULL) {
            isFailedToJoin = true;
            break;
        }
        if (i != argCount - 1) {
            concatCharsByLength(str, delimiter, delimiterLength);
        }
    }

    if (isFailedToJoin) {
        TERMINATE_STRING(str);  // restore previous string ending by length
    }
    return str;
}

BufferString *joinStrings(BufferString *str, const char *delimiter, uint32_t argCount, ...) {
    va_list valist;
    va_start(valist, argCount);
    uint32_t delimiterLength = strlen(delimiter);

    bool isFailedToJoin = false;
    for (uint32_t i = 0; i < argCount; i++) {
        BufferString *argValue = va_arg(valist, BufferString *);
        if (concatString(str, argValue) == NULL) {
            isFailedToJoin = true;
            break;
        }
        if (i != argCount - 1) {
            concatCharsByLength(str, delimiter, delimiterLength);
        }
    }

    if (isFailedToJoin) {
        TERMINATE_STRING(str);
    }
    va_end(valist);
    return str;
}

BufferString *repeatChar(BufferString *str, char repeatChar, uint32_t count) {
    while (str != NULL && count > 0) {
        str = concatChar(str, repeatChar);
        count--;
    }
    return str;
}

BufferString *repeatChars(BufferString *str, const char *repeatChars, uint32_t count) {
    while (str != NULL && count > 0) {
        str = concatChars(str, repeatChars);
        count--;
    }
    return str;
}

BufferString *int64ToString(BufferString *str, int64_t value) {
    char *bufferPointer = str->value;
    uint64_t convertedValue = (value < 0) ? -value : value;
    int32_t length = numberToStringByBase(convertedValue, bufferPointer, DEC_BASE, 0);

    if (value < 0) {
        if ((length + 1) >= str->capacity) return NULL;
        bufferPointer[length] = '-';
        length++;   // add minus sign to length
    }

    str->length = length;
    return reverseString(str);
}

BufferString *uInt64ToString(BufferString *str, uint64_t value) {
    char *bufferPointer = str->value;
    int32_t length = numberToStringByBase(value, bufferPointer, DEC_BASE, 0);
    str->length = length;
    return reverseString(str);
}

StringToI64Status stringToI64(BufferString *str, int64_t *out, int base) {
    return str != NULL ? cStrToInt64(str->value, out, base) : STR_TO_I64_INCONVERTIBLE;
}

StringToI64Status cStrToInt64(const char *str, int64_t *out, int base) {
    if (str == NULL || *str == '\0' || isspace((int) str[0])) {
        return STR_TO_I64_INCONVERTIBLE;
    }

    char *end;
    errno = 0;
    int64_t result = strtoll(str, &end, base);
    // Both checks are needed because LLONG_MAX == LLONG_MAX is possible.
    if (result > LLONG_MAX || (errno == ERANGE && result == LLONG_MAX)) {
        return STR_TO_I64_OVERFLOW;
    }

    if (result < LLONG_MIN || (errno == ERANGE && result == LLONG_MIN)) {
        return STR_TO_I64_UNDERFLOW;
    }

    if (*end != '\0') {
        return STR_TO_I64_INCONVERTIBLE;
    }

    *out = result;
    return STR_TO_I64_SUCCESS;
}

bool isBuffStrBlank(BufferString *str) {
    return str != NULL ? isCstrBlank(str->value) : true;
}

bool isCstrBlank(const char *str) {
    while (isCstrNotEmpty(str)) {
        if (!isspace((int) *str)) {
            return false;
        }
        str++;
    }
    return true;
}

bool isBuffStrEquals(BufferString *one, BufferString *two) {
    if (one == two) return true;

    if (one != NULL && two != NULL) {
        size_t oneLength = one->length;
        if (oneLength == two->length) {
            return strncmp(one->value, two->value, oneLength) == 0;
        }
    }
    return false;
}

bool isBuffStrEqualsCstr(BufferString *one, const char *two) {
    if (one != NULL && one->value == two) {
        return true;
    }

    if (one != NULL && two != NULL) {
        size_t oneLength = one->length;
        size_t twoLength = strnlen(two, oneLength + 1);
        if (oneLength == twoLength) {
            return strncmp(one->value, two, oneLength) == 0;
        }
    }
    return false;
}

bool isBuffStrEqualsIgnoreCase(BufferString *one, BufferString *two) {
    if (one == two) return true;

    if (one != NULL && two != NULL) {
        size_t oneLength = one->length;
        if (oneLength == two->length) {
            return strncasecmp(one->value, two->value, oneLength) == 0;
        }
    }
    return false;
}

int32_t indexOfChar(BufferString *str, char charToFind, uint32_t fromIndex) {
    if (str == NULL || fromIndex >= str->length) return NO_RESULT;
    for (int32_t i = (int32_t) fromIndex; i < str->length; i++) {
        if (str->value[i] == charToFind) {
            return i;
        }
    }
    return NO_RESULT;
}

int32_t indexOfString(BufferString *str, const char *stringToFind, uint32_t fromIndex) {
    return indexOfCStr(str != NULL ? str->value : NULL, stringToFind, fromIndex);
}

int32_t lastIndexOfString(BufferString *str, const char *stringToFind) {
    return lastIndexOfCStr(str != NULL ? str->value : NULL, stringToFind);
}

int32_t indexOfCStr(char *str, const char *stringToFind, uint32_t fromIndex) {
    if (str == NULL || stringToFind == NULL || fromIndex >= strlen(str)) return NO_RESULT;
    char *strPointer = strstr(str + fromIndex, stringToFind);
    return strPointer != NULL ? (strPointer - str) : NO_RESULT;
}

int32_t lastIndexOfCStr(char *str, const char *stringToFind) {
    if (str == NULL || stringToFind == NULL) return NO_RESULT;
    uint32_t substringLength = strlen(stringToFind);

    uint32_t i = strlen(str);
    while (true) {
        uint32_t step = (i > substringLength) ? substringLength : 1;
        if (i == 0) {
            break;
        }
        i -= step;

        char *substringPointer = strstr(str + i, stringToFind);
        if (substringPointer != NULL) {
            return (substringPointer - str);
        }
    }
    return NO_RESULT;
}

bool isStrStartsWith(BufferString *str, const char *prefix, uint32_t toOffset) {
    if (str == NULL || prefix == NULL) return false;
    uint32_t prefixLength = strlen(prefix);
    if (toOffset > (str->length - prefixLength)) return false;

    const char *valuePointer = str->value + toOffset;
    for (uint32_t i = 0; i < prefixLength; i++) {
        if (valuePointer[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

bool isStrStartsWithIgnoreCase(BufferString *str, const char *prefix, uint32_t toOffset) {
    if (str == NULL || prefix == NULL) return false;
    uint32_t prefixLength = strlen(prefix);
    if (toOffset > (str->length - prefixLength)) return false;

    const char *valuePointer = str->value + toOffset;
    for (uint32_t i = 0; i < prefixLength; i++) {
        if (tolower((int) valuePointer[i]) != tolower((int) prefix[i])) {
            return false;
        }
    }
    return true;
}

bool isStrEndsWith(BufferString *str, const char *suffix) {
    if (str == NULL || suffix == NULL) return false;
    uint32_t suffixLength = strlen(suffix);

    if (suffixLength > str->length) {
        return false;
    }

    uint32_t length = str->length;
    for (uint32_t i = suffixLength; i > 0; i--, length--) {
        if (str->value[length - 1] != suffix[i - 1]) {
            return false;
        }
    }
    return true;
}

bool isStrEndsWithIgnoreCase(BufferString *str, const char *suffix) {
    if (str == NULL || suffix == NULL) return false;
    uint32_t suffixLength = strlen(suffix);

    if (suffixLength > str->length) {
        return false;
    }

    uint32_t length = str->length;
    for (uint32_t i = suffixLength; i > 0; i--, length--) {
        if (tolower((int) str->value[length - 1]) != tolower((int) suffix[i - 1])) {
            return false;
        }
    }
    return true;
}

static uint32_t isDelimiterChar(char valueChar, const char *delimiters, uint32_t length) {
    for (int i = 0; i < length; i++) {
        if (delimiters[i] == valueChar) {
            return true;
        }
    }
    return false;
}

static uint8_t parseFormatFlags(const char *format, uint8_t *flags) {
    uint8_t flagsLength = 0;
    bool haveNextFlag = true;
    while (haveNextFlag) {
        char formatChar = *format;

        switch (formatChar) {
            case '-':
                SET_FLAG(*flags, LEFT_ALIGN_FLAG);
                break;
            case '+':
                SET_FLAG(*flags, PLUS_FLAG);
                break;
            case ' ':
                SET_FLAG(*flags, SPACE_FLAG);
                break;
            case '0':
                SET_FLAG(*flags, ZEROES_PADDING_FLAG);
                break;
            case '#':
                SET_FLAG(*flags, SPECIAL_FLAG);
                break;
            default:
                haveNextFlag = false;
                break;
        }

        if (haveNextFlag) {
            format++;
            flagsLength++;
        }
    }
    return flagsLength;
}

static uint8_t parseFormatFieldWith(const char *format, va_list *vaList, int32_t *widthField, uint8_t *flags) {
    if (isdigit((int) *format)) {
        *widthField = 0;
        return stringToNumber(format, widthField);

    } else if (*format == '*') {
        *widthField = va_arg(*vaList, int32_t);   // dynamic width field value must be provided
        if (*widthField < 0) {
            *widthField = -*widthField;
            SET_FLAG(*flags, LEFT_ALIGN_FLAG);
        }
        return SKIP_ONE_CHAR; // skip '*'
    }
    return 0;
}

static uint8_t parseFormatPrecision(const char *format, va_list *vaList, int32_t *precision) {
    if (*format == '.') {
        format++;   // skip '.'
        if (isdigit((int) *format)) {
            *precision = 0;
            return stringToNumber(format, precision) + SKIP_ONE_CHAR;   // also skip '.'

        } else if (*format == '*') {
            *precision = va_arg(*vaList, int32_t);   // dynamic precision field value must be provided
            return SKIP_TWO_CHARS; // skip '*' and '.'
        }

        if (*precision < 0) {
            *precision = 0;
        }
        return SKIP_ONE_CHAR;      // also skip '.'
    }
    return 0;
}

static uint8_t parseLengthField(char *lengthField, const char *format) {
    if (*format == 'h' || *format == 'l' || *format == 'L') {
        lengthField[0] = *format;
        format++;   // skip length field
        if (*format == 'h' || *format == 'l') {
            lengthField[1] = *format;
            return SKIP_TWO_CHARS;   // skip additional field
        }
        return SKIP_ONE_CHAR;

    } else if (*format == 'I' || *format == 'U') {  // custom fields as U8, I8, U16, I16, U32, I32, U64, I64
        format++;   // skip type
        if (*format == '8') {
            lengthField[0] = *format;  // byte
            return 0;
        }
        lengthField[0] = *format++;   // all other int types. Do not return any length, because designator reversed instead standard types
        lengthField[1] = *format;
    }
    return 0;
}

static BufferString *formatCharacter(BufferString *str, uint8_t flags, int32_t widthField, va_list *vaList) {
    if (IS_FLAG_NOT_SET(flags, LEFT_ALIGN_FLAG)) {
        while (widthField > 0) {
            str = concatChar(str, ' ');
            widthField--;
        }
    }

    char valueChar = (char) va_arg(*vaList, int);
    str = concatChar(str, valueChar);
    if(IS_FLAG_SET(flags, LEFT_ALIGN_FLAG)) {
        while (widthField > 0) {
            str = concatChar(str, ' ');
            widthField--;
        }
    }
    return str;
}

static BufferString *formatChars(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, va_list *vaList) {
    char *valueStr = va_arg(*vaList, char *);
    if (valueStr == NULL) return NULL;
    uint32_t maxLength = (precision < 0 || precision > str->capacity) ? (str->capacity - str->length) : precision;
    uint32_t length = strnlen(valueStr, maxLength);
    widthField = (widthField > 0) ? widthField : 0;
    return doFormatChars(str, valueStr, length, flags, widthField);
}

static BufferString *formatString(BufferString *str, uint8_t flags, int32_t widthField, int64_t precision, va_list *vaList) {
    BufferString *valueStr = va_arg(*vaList, BufferString *);
    if (valueStr == NULL || valueStr->length > (str->capacity - str->length)) return NULL;
    uint32_t maxLength = (precision < 0 || precision > str->capacity) ? valueStr->length + 1 : precision;
    uint32_t length = strnlen(valueStr->value, maxLength);
    widthField = (widthField > 0) ? widthField : 0;
    return doFormatChars(str, valueStr->value, length, flags, widthField);
}

static BufferString *doFormatChars(BufferString *str, const char *valueStr, uint32_t length, uint8_t flags, int32_t widthField) {
    if (IS_FLAG_NOT_SET(flags, LEFT_ALIGN_FLAG)) {
        while (length < widthField) {
            str = concatChar(str, ' ');
            widthField--;
        }
    }

    str = concatCharsByLength(str, valueStr, length);
    while (length < widthField) {
        str = concatChar(str, ' ');
        widthField--;
    }
    return str;
}

static BufferString *formatPointer(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint64_t pointerAddress) {
    if (widthField <= 0) {
        widthField = POINTER_DEFAULT_WIDTH;
        SET_FLAG(flags, ZEROES_PADDING_FLAG);
    }
    return numberToString(str, pointerAddress, NO_SIGN, HEX_BASE, widthField, precision, flags);
}

static BufferString *formatNumber(BufferString *str, uint8_t flags, const char *lengthField, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (lengthField[0] == 'h' || IS_INT_8(lengthField) || IS_INT_16(lengthField)) {    // 	Expect int-sized integer argument which was promoted from a short.
        if (lengthField[1] == 'h' || IS_INT_8(lengthField)) {    // Expect int-sized integer argument which was promoted from a char.
            return formatByte(str, flags, widthField, precision, base, vaList);
        }
        return formatShort(str, flags, widthField, precision, base, vaList);
    }

    if (lengthField[0] == 'l' || IS_INT_64(lengthField)) {
        if (lengthField[1] == 'l' || IS_INT_64(lengthField)) {
            return formatLongLong(str, flags, widthField, precision, base, vaList);
        }
        return formatLong(str, flags, widthField, precision, base, vaList);
    }

    return formatInt(str, flags, widthField, precision, base, vaList);
}

#ifdef ENABLE_FLOAT_FORMATTING
static BufferString *formatFloat(BufferString *str, double decimalValue, uint8_t flags, int32_t widthField, int32_t precision) {
    static const double FLOAT_POW_OF_10[] = {1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000};

    if (isNanOrInfinity(str, decimalValue, widthField, flags)) {
        return str;
    }

    if ((decimalValue > FORMAT_MAX_FLOAT_VALUE) || (decimalValue < -FORMAT_MAX_FLOAT_VALUE)) {
        return formatExponential(str, decimalValue, flags, widthField, precision);
    }

    if (precision < 0) {
        precision = FORMAT_DEFAULT_FLOAT_PRECISION;
    }

    int32_t bufferLength = 0;
    char tmpDecimalBuffer[FORMAT_FLOAT_BUFFER_SIZE] = {0};
    while (bufferLength < FORMAT_FLOAT_BUFFER_SIZE && precision > FORMAT_MAX_FLOAT_PRECISION) {// limit precision to 9, cause a precision > 9 can lead to overflow errors
        tmpDecimalBuffer[bufferLength] = '0';
        bufferLength++;
        precision--;
    }

    double power = FLOAT_POW_OF_10[precision];
    decimalValue *= power;
    decimalValue = (decimalValue >= 0) ? ((int64_t) (decimalValue + 0.5)) / power : ((int64_t) (decimalValue - 0.5)) / power;// round by precision

    int64_t wholePart = (int64_t) decimalValue;
    char sign = resolveSign(&wholePart, flags, &widthField);
    if (precision == 0) {
        return numberToString(str, wholePart, sign, DEC_BASE, widthField, 0, flags);  // return string if only whole part is needed
    }

    decimalValue = (sign == '-') ? -decimalValue : decimalValue;
    double tmpFractional = (decimalValue - (int32_t) wholePart) * power;  // convert fraction part as a whole
    tmpFractional = (((int32_t) (tmpFractional + 0.5)) / power) * power; // round conversion
    int32_t fractionalPart = (int32_t) tmpFractional;   // extract fraction

    int32_t decimalLength = numberToStringByBase(fractionalPart, (tmpDecimalBuffer + bufferLength), DEC_BASE, flags);
    bufferLength += decimalLength;
    int32_t precisionDigitCount = (precision - bufferLength);

    while (precisionDigitCount > 0 && bufferLength < FORMAT_FLOAT_BUFFER_SIZE) {   // add extra precision 0s
        tmpDecimalBuffer[bufferLength] = '0';
        precisionDigitCount--;
        bufferLength++;
    }

    if (bufferLength < FORMAT_FLOAT_BUFFER_SIZE) {  // add decimal point
        tmpDecimalBuffer[bufferLength] = '.';
        bufferLength++;
    }

    uint32_t startValueLength = str->length;
    int32_t wholeLength = IS_FLAG_SET(flags, LEFT_ALIGN_FLAG) ? 0 : (widthField - bufferLength);   // when '-' flag set, concat only number without padding
    str = numberToString(str, wholePart, sign, DEC_BASE, wholeLength, 1, flags); // concat whole part with padding minus length of decimal part

    while (str != NULL && bufferLength > 0) { // concat fraction and decimal point part
        char digitChar = tmpDecimalBuffer[--bufferLength];
        str = concatChar(str, digitChar);
    }

    if (IS_FLAG_SET(flags, LEFT_ALIGN_FLAG)) {
        uint32_t endValueLength = str->length - startValueLength;
        uint32_t paddingLength = (widthField >= endValueLength) ? widthField - endValueLength : 0;
        repeatChar(str, ' ', paddingLength);
    }
    return str;
}

static BufferString *formatExponential(BufferString *str, double decimalValue, uint8_t flags, int32_t widthField, int32_t precision) {
    if (isNanOrInfinity(str, decimalValue, widthField, flags)) {
        return str;
    }

    if (precision < 0) {
        precision = FORMAT_DEFAULT_FLOAT_PRECISION;
    }

    bool isNegative = decimalValue < 0;
    decimalValue = isNegative ? -decimalValue : decimalValue;

    // Determine the decimal exponent. Based on the algorithm by David Gay (https://github.com/jwiegley/gdtoa/blob/master/dtoa.c)
    DoubleCast converter = {.decimal = decimalValue};
    int32_t exponentOfTwo = (converter.parts.exponent - DOUBLE_EXPONENT_ZERO_VALUE);
    converter.parts.exponent = DOUBLE_EXPONENT_ZERO_VALUE;  // drop the exponent so converter.decimal is now in [1,2)

    // now approximate log10 from the log2 integer part and an expansion of ln around 1.5
    int32_t exponentValue = (int32_t) ((converter.decimal - 1.5) * LOG_APPROXIMATION + LOG10_OF_1_5 + exponentOfTwo * LOG10_OF_2);
    exponentOfTwo = (int32_t) (exponentValue * LOG2_OF_10 + 0.5);// now we want to compute 10^exponentValue, but we want to be sure it won't overflow

    double zValue = (exponentValue * NATURAL_LOG_OF_10) - (exponentOfTwo * NATURAL_LOG_OF_2);
    double zPowOfTwo = zValue * zValue;

    converter.parts.exponent += exponentOfTwo;    // return exp value
    converter.parts.mantissa = 0;   // drop mantissa

    // compute exp(z) using continued fractions, see https://en.wikipedia.org/wiki/Exponential_function#Continued_fractions_for_ex
    converter.decimal = converter.decimal * (1 + 2 * zValue / (2 - zValue + (zPowOfTwo / (6 + (zPowOfTwo / (10 + zPowOfTwo / 14))))));

    if (decimalValue < converter.decimal) {    // correct for rounding errors
        exponentValue--;
        converter.decimal /= 10;
    }

    // the exponent format is "%+03d" and largest value is "307", so set aside 4 characters
    int32_t exponentMinWidth = EXPONENT_WIDTH_DEFAULT_VALUE;
    if (IS_FLAG_SET(flags, ADAPTIVE_EXPONENT_FLAG)) {   // in "%g" mode, "precision" is the number of "significant figures" not decimals
        // do we want to fall back to "%f" mode?
        if (decimalValue >= 1e-4 && decimalValue < 1e6) {
            precision = (precision > exponentValue) ? (precision - exponentValue - 1) : 0;
            // no characters in exponent
            exponentValue = 0;
            exponentMinWidth = 0;

        } else {
            precision--;
        }
    }

    // check that everything fits
    int32_t decimalWith = (widthField > exponentMinWidth) ? (widthField - exponentMinWidth) : 0;
    bool isLeftPadFlagSet = IS_FLAG_SET(flags, LEFT_ALIGN_FLAG);
    if (isLeftPadFlagSet) {
        BIT_CLEAR(flags, LEFT_ALIGN_FLAG);    // if we're padding on the right, DON'T pad the floating part
        decimalWith = 0;
    }

    decimalValue = (exponentValue != 0) ? (decimalValue / converter.decimal) : decimalValue; // rescale the float value
    decimalValue = isNegative ? -decimalValue : decimalValue;
    decimalWith = (decimalWith > 0 && exponentMinWidth > 0) ? (decimalWith - 1) : decimalWith;

    uint32_t startValueLength = str->length;
    str = formatFloat(str, decimalValue, flags, decimalWith, precision);        // output the floating part

    if (exponentMinWidth > 0) {     // output the exponent part
        str = concatChar(str, IS_FLAG_SET(flags, LOWER_CASE_FLAG) ? 'e' : 'E');
        char sign = (exponentValue < 0) ? '-' : '+';
        exponentValue = (exponentValue < 0) ? -exponentValue : exponentValue;
        SET_FLAG(flags, ZEROES_PADDING_FLAG);    // set zeroes to exponent value
        str = numberToString(str, exponentValue, sign, DEC_BASE, 0, exponentMinWidth - 1, flags);
    }

    if (isLeftPadFlagSet) {
        uint32_t endValueLength = str->length - startValueLength;
        uint32_t paddingLength = (widthField >= endValueLength) ? widthField - endValueLength : 0;
        repeatChar(str, ' ', paddingLength);
    }
    return str;
}
#endif

static inline BufferString *formatByte(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (IS_FLAG_SET(flags, SIGNED_NUMBER_FLAG)) {    // "hhi"
        signed char signedInt = (signed char) va_arg(*vaList, signed int);
        int64_t number = (int64_t) signedInt;
        char sign = resolveSign(&number, flags, &widthField);
        return numberToString(str, number, sign, base, widthField, precision, flags);
    }
    int64_t number = (unsigned char) va_arg(*vaList, signed int);   // "hhu"
    return numberToString(str, number, NO_SIGN, base, widthField, precision, flags);
}

static inline BufferString *formatShort(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (IS_FLAG_SET(flags, SIGNED_NUMBER_FLAG)) {    // "hi"
        int64_t number = (short) va_arg(*vaList, signed int);
        char sign = resolveSign(&number, flags, &widthField);
        return numberToString(str, number, sign, base, widthField, precision, flags);
    }
    unsigned short number = (unsigned short) va_arg(*vaList, signed int);   // "hu"
    return numberToString(str, number, NO_SIGN, base, widthField, precision, flags);
}

static inline BufferString *formatLongLong(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (IS_FLAG_SET(flags, SIGNED_NUMBER_FLAG)) {    // "lld/i"
        int64_t number = va_arg(*vaList, signed long long);
        char sign = resolveSign(&number, flags, &widthField);
        return numberToString(str, number, sign, base, widthField, precision, flags);
    }
    unsigned long long number = va_arg(*vaList, unsigned long long);    // "llu"
    return numberToString(str, number, NO_SIGN, base, widthField, precision, flags);
}

static inline BufferString *formatLong(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (IS_FLAG_SET(flags, SIGNED_NUMBER_FLAG)) {    // "ld/i"
        int64_t number = va_arg(*vaList, signed long);
        char sign = resolveSign(&number, flags, &widthField);
        return numberToString(str, number, sign, base, widthField, precision, flags);
    }
    unsigned long number = va_arg(*vaList, unsigned long);  // "lu"
    return numberToString(str, number, NO_SIGN, base, widthField, precision, flags);
}

static inline BufferString *formatInt(BufferString *str, uint8_t flags, int32_t widthField, int32_t precision, uint8_t base, va_list *vaList) {
    if (IS_FLAG_SET(flags, SIGNED_NUMBER_FLAG)) {    // "d/i"
        int64_t number = va_arg(*vaList, signed int);
        char sign = resolveSign(&number, flags, &widthField);
        return numberToString(str, number, sign, base, widthField, precision, flags);
    }
    uint64_t number = va_arg(*vaList, unsigned int);    // "u"
    return numberToString(str, number, NO_SIGN, base, widthField, precision, flags);
}

static BufferString *numberToString(BufferString *str, uint64_t number, char sign, uint8_t base, int32_t size, int32_t precision, uint8_t flags) {
    if (number == 0) {
        BIT_CLEAR(flags, SPECIAL_FLAG);
        precision = (size < 0 && precision < 0) ? 1 : precision;    // when field width, precision not provided and number is 0, concat single '0' to string

        if (precision <= 0) {
            if (size >= 0) {
                BIT_CLEAR(flags, ZEROES_PADDING_FLAG);  // concat only ' '
                str = concatLeftPadding(str, &sign, base, &size, flags);
            }
            return concatSignIfPresent(str, sign);
        }
    }

    if (IS_FLAG_SET(flags, LEFT_ALIGN_FLAG)) {
        BIT_CLEAR(flags, ZEROES_PADDING_FLAG);
    }

    if (IS_FLAG_SET(flags, SPECIAL_FLAG)) {
        if (base == HEX_BASE || base == BIN_BASE) {
            size -= HEX_SIZE;

        } else if (base == OCT_BASE) {
            size--;
        }
    }

    char tmpNumberBuffer[FORMAT_NUMBER_BUFFER_SIZE] = {0};
    int32_t numberLength = numberToStringByBase(number, tmpNumberBuffer, base, flags);
    precision = (numberLength > precision) ? numberLength : precision;

    size -= precision;
    size = (size > 0) ? size : 0;

    str = concatLeftPadding(str, &sign, base, &size, flags);
    str = concatSignIfPresent(str, sign);

    while (numberLength < precision) {  // add precision before value
        str = concatChar(str, '0');
        precision--;
    }

    while (numberLength > 0) { // reverse concat number chars
        str = concatChar(str, tmpNumberBuffer[--numberLength]);
    }

    return concatRightPadding(str, size);
}

static uint8_t stringToNumber(const char *numberStr, int32_t *resultValue) {
    uint8_t numberLength = 0;

    while (isdigit((int) *numberStr) && numberLength <= UINT64_DIGITS_MAX_COUNT) {
        *resultValue = (*resultValue * 10) + (*numberStr - '0');
        numberLength++;
        numberStr++;
    }
    return numberLength;
}

static int32_t numberToStringByBase(uint64_t number, char *numberBuffer, uint8_t base, uint8_t flags) {
    /* called only with base 2, 8, 10 or 16, thus don't need more than "F..."  */
    static const char DIGITS[] = "0123456789ABCDEF";

    int32_t length = 0;
    bool isUpperCaseValue = IS_FLAG_SET(flags, LOWER_CASE_FLAG);
    do {
        uint64_t result = number % base;
        number = number / base;
        char digit = DIGITS[result];
        numberBuffer[length] = (char) (isUpperCaseValue ? tolower((int) digit) : digit);
        length++;
    } while (number > 0);

    return length;
}

static BufferString *concatLeftPadding(BufferString *str, char *sign, uint8_t base, int32_t *size, uint8_t flags) {
    if (IS_FLAG_NOT_SET(flags, LEFT_ALIGN_FLAG)) {
        char paddingChar = IS_FLAG_SET(flags, ZEROES_PADDING_FLAG) ? '0' : ' ';
        if (paddingChar == '0') {
            str = concatSignIfPresent(str, *sign);
            str = concatSpecialIfPresent(str, base, flags);
            str = repeatChar(str, paddingChar, *size);

        } else {
            str = repeatChar(str, paddingChar, *size);
            str = concatSignIfPresent(str, *sign);
            str = concatSpecialIfPresent(str, base, flags);
        }
        *size = 0;
        *sign = NO_SIGN;
    }
    return str;
}

static inline BufferString *concatSpecialIfPresent(BufferString *str, uint8_t base, uint8_t flags) {
    if (IS_FLAG_SET(flags, SPECIAL_FLAG)) {
        if (base == OCT_BASE) {
            str = concatChar(str, '0');

        } else if (base == HEX_BASE) {
            str = concatChar(str, '0');
            str = concatChar(str, IS_FLAG_SET(flags, LOWER_CASE_FLAG) ? 'x' : 'X');

        } else if (base == BIN_BASE) {
            str = concatCharsByLength(str, "0b", 2);
        }
    }
    return str;
}

static inline BufferString *concatSignIfPresent(BufferString *str, char sign) {
    return (sign != NO_SIGN) ? concatChar(str, sign) : str;
}

static inline BufferString *concatRightPadding(BufferString *str, int32_t size) {
    return (size > 0) ? repeatChar(str, ' ', size) : str;
}

static char resolveSign(int64_t *number, uint8_t flags, int32_t *widthField) {
    char sign = NO_SIGN;
    if (*number < 0) {
        sign = '-';
        *number = -*number;
        *widthField -= 1;

    } else if (IS_FLAG_SET(flags, PLUS_FLAG)) {
        sign = '+';
        *widthField -= 1;

    } else if (IS_FLAG_SET(flags, SPACE_FLAG)) {
        sign = ' ';
        *widthField -= 1;
    }
    return sign;
}

#ifdef ENABLE_FLOAT_FORMATTING
static bool isNanOrInfinity(BufferString *str, double decimalValue, int32_t widthField, uint8_t flags) {
    char sign = NO_SIGN;
    if (isnan(decimalValue)) {
        widthField -= NAN_LENGTH;
        concatLeftPadding(str, &sign, DEC_BASE, &widthField, flags);
        concatChars(str, IS_FLAG_SET(flags, LOWER_CASE_FLAG) ? "nan" : "NaN");
        concatRightPadding(str, widthField);
        return true;

    } else if (decimalValue < -DBL_MAX) {
        widthField -= INF_WITH_SIGN_LENGTH;
        concatLeftPadding(str, &sign, DEC_BASE, &widthField, flags);
        concatChars(str, IS_FLAG_SET(flags, LOWER_CASE_FLAG) ? "-inf" : "-INF");
        concatRightPadding(str, widthField);
        return true;

    } else if (decimalValue > DBL_MAX) {
        widthField = IS_FLAG_SET(flags, PLUS_FLAG) ? (widthField - INF_WITH_SIGN_LENGTH) : (widthField - INF_LENGTH);
        concatLeftPadding(str, &sign, DEC_BASE, &widthField, flags);
        if (IS_FLAG_SET(flags, PLUS_FLAG)) {
            concatChar(str, '+');
        }
        concatChars(str, IS_FLAG_SET(flags, LOWER_CASE_FLAG) ? "inf" : "INF");
        concatRightPadding(str, widthField);
        return true;
    }
    return false;
}
#endif