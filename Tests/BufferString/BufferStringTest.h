#pragma once

#include "BaseTestTemplate.h"
#include <BufferString.h>

// Example of ESP8266 command AT+CWLAP response
static const char *ESP_RESPONSE = "+CWLAP:(3,\"CVBJB\",-71,\"f8:e4:fb:5b:a9:5a\")\n"
                                  "+CWLAP:(3,\"HT_00d02d638ac3\",-90,\"04:f0:21:0f:1f:61\")\n"
                                  "+CWLAP:(3,\"CLDRM\",-69,\"22:c9:d0:1a:f6:54\")\n"
                                  "+CWLAP:(2,\"AllSaints\",-88,\"c4:01:7c:3b:08:48\")\n"
                                  "+CWLAP:(0,\"AllSaints-Guest\",-83,\"c4:01:7c:7b:08:48\")";

static const char TEST_STRING[] = "0,CONNECT\n 1,CONNECT\n\n +IPD,1,497:GET /api/test HTTP/1.1\nHost: 192.168.53.117\nConnection: keep-alive\n";

static void validateString(BufferString *str, const char *expectedText, uint32_t expectedLength, uint32_t expectedCapacity) {
    assert_not_null(str);
    assert_string_equal(str->value, expectedText);
    assert_uint32(str->length, ==, expectedLength);
    assert_uint32(str->capacity, ==, expectedCapacity);
}

static MunitResult testNewStringCreation(const MunitParameter params[], void *testData) {
    BufferString *str_1 = NEW_STRING(16, "abc");
    validateString(str_1, "abc", 3, 16);

    char buffer[] = {'a', 'b', 'c'};
    BufferString *str_2 = NEW_STRING_LEN(16, buffer, sizeof(buffer));
    validateString(str_2, "abc", 3, 16);

    BufferString *str_3 = EMPTY_STRING(32);
    validateString(str_3, "", 0, 32);

    BufferString *str_4 = NEW_STRING_16("abc");
    validateString(str_4, "abc", 3, 16);

    BufferString *str_5 = NEW_STRING_32("abc");
    validateString(str_5, "abc", 3, 32);

    BufferString *str_6 = NEW_STRING_64("abc");
    validateString(str_6, "abc", 3, 64);

    BufferString *str_7 = NEW_STRING_128("abc");
    validateString(str_7, "abc", 3, 128);

    BufferString *str_8 = NEW_STRING_256("abc");
    validateString(str_8, "abc", 3, 256);

    BufferString *str_9 = NEW_STRING_512("abc");
    validateString(str_9, "abc", 3, 512);

    BufferString *str_10 = NEW_STRING_1024("abc");
    validateString(str_10, "abc", 3, 1024);

    BufferString *str_11 = NEW_STRING_2048("abc");
    validateString(str_11, "abc", 3, 2048);

    BufferString *str_12 = NEW_STRING(3, "abcdef"); // too low capacity
    assert_null(str_12);

    BufferString *str_13 = NEW_STRING(16, NULL); // null value
    assert_null(str_13);

    BufferString *str_14 = NEW_STRING(1, "");
    validateString(str_14, "", 0, 1);

    static char staticBuff[128] = {0};
    BufferString *str_15 = NEW_STRING_BUFF(staticBuff, "abcd");
    validateString(str_15, "abcd", 4, 128);

    return MUNIT_OK;
}

static MunitResult testDupString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_16("abc");
    BufferString *dup = DUP_STRING(32, str);
    validateString(dup, "abc", 3, 32);
    return MUNIT_OK;
}

static MunitResult testStringFormat(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%.*s]", 3, "abcdef");
    assert_string_equal(str->value, "[abc]");

    stringFormat(str, "[%*d]", 5, 10);
    assert_string_equal(str->value, "[   10]");

    stringFormat(str, "[%04x]", 10);
    assert_string_equal(str->value, "[000a]");

    stringFormat(str, "[%4X]", 10);
    assert_string_equal(str->value, "[   A]");

    stringFormat(str, "[%#8X]", 171);
    assert_string_equal(str->value, "[    0XAB]");

    stringFormat(str, "[%I32], [%U32]", INT32_MIN, UINT32_MAX);
    assert_string_equal(str->value, "[-2147483648], [4294967295]");

    stringFormat(str, "[%#b]", 12345);
    assert_string_equal(str->value, "[0b11000000111001]");

    stringFormat(str, "[%b]", 12345);
    assert_string_equal(str->value, "[11000000111001]");

    stringFormat(str, "[%010d]", 12345);
    assert_string_equal(str->value, "[0000012345]");

    stringFormat(str, "[%-10d]", 12345);
    assert_string_equal(str->value, "[12345     ]");

    stringFormat(str, "[%+010d]", 12345);
    assert_string_equal(str->value, "[+000012345]");

    stringFormat(str, "[% 010d]", 12345);
    assert_string_equal(str->value, "[ 000012345]");

    stringFormat(str, "[% 010d]", -12345);
    assert_string_equal(str->value, "[-000012345]");

    stringFormat(str, "[%.8d]", 12345);
    assert_string_equal(str->value, "[00012345]");

    stringFormat(str, "[%.8d]", -12345);
    assert_string_equal(str->value, "[-00012345]");

    stringFormat(str, "[%o]", 12345);
    assert_string_equal(str->value, "[30071]");

    stringFormat(str, "[%o]", -12345);
    assert_string_equal(str->value, "[37777747707]");

    stringFormat(str, "[%#o]", 12345);
    assert_string_equal(str->value, "[030071]");

    stringFormat(str, "[%#x], [%#X]", 171, 171);
    assert_string_equal(str->value, "[0xab], [0XAB]");

    stringFormat(str, "[%f]", -145.2345676);
    assert_string_equal(str->value, "[-145.234568]");

    stringFormat(str, "[%f]", 145.2345676);
    assert_string_equal(str->value, "[145.234568]");

    stringFormat(str, "[%.6f]", 145.234);
    assert_string_equal(str->value, "[145.234000]");

    stringFormat(str, "[%.*f]", 2, 145.235);
    assert_string_equal(str->value, "[145.24]");

    stringFormat(str, "[%-10.8d]", 1234);
    assert_string_equal(str->value, "[00001234  ]");

    stringFormat(str, "[%10.8d]", 1234);
    assert_string_equal(str->value, "[  00001234]");

    stringFormat(str, "[%.0f]", 145.6355676);
    assert_string_equal(str->value, "[146]");

    stringFormat(str, "[%.0f]", -145.6355676);
    assert_string_equal(str->value, "[-146]");

    stringFormat(str, "[%e]", 125.348957);
    assert_string_equal(str->value, "[1.253490e+002]");

    stringFormat(str, "[%g]", 125.348957);
    assert_string_equal(str->value, "[125.349]");

    stringFormat(str, "[%e]", 125435345.348976547456757);
    assert_string_equal(str->value, "[1.254353e+008]");

    stringFormat(str, "[%g]", 125435345.348976547456757);
    assert_string_equal(str->value, "[1.25435e+008]");

    stringFormat(str, "[%e]", 1.54334E-34);
    assert_string_equal(str->value, "[1.543340e-034]");

    stringFormat(str, "[%s %s %d %d %u %u%%]", "normal", "test", 123, 456, 789, 987);
    validateString(str, "[normal test 123 456 789 987%]", 30, 128);

    stringFormat(str, "[%u%u%ctest%d %s]", 5, 3000, 'a', -20, "bit");
    assert_string_equal(str->value, "[53000atest-20 bit]");

    stringFormat(str, "[%.*f]", 2, 0.33333333);
    assert_string_equal(str->value, "[0.33]");

    stringFormat(str, "[%.*d]", -1, 1);
    assert_string_equal(str->value, "[1]");

    stringFormat(str, "[%.3s]", "foobar");
    assert_string_equal(str->value, "[foo]");

    stringFormat(str, "[% .0d]", 0);
    assert_string_equal(str->value, "[ ]");

    stringFormat(str, "[%10.5d]", 4);
    assert_string_equal(str->value, "[     00004]");

    stringFormat(str, "[%*sx]", -3, "hi");
    assert_string_equal(str->value, "[hi x]");

    stringFormat(str, "[%.*g]", 2, 0.33333333);
    assert_string_equal(str->value, "[0.33]");

    stringFormat(str, "[%.*e]", 2, 0.33333333);
    assert_string_equal(str->value, "[3.33e-001]");
    return MUNIT_OK;
}

static MunitResult testFormatSpaceFlag(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[% d]", 42);
    assert_string_equal(str->value, "[ 42]");

    stringFormat(str, "[% d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[% 5d]", 42);
    assert_string_equal(str->value, "[   42]");

    stringFormat(str, "[% 5d]", -42);
    assert_string_equal(str->value, "[  -42]");

    stringFormat(str, "[% 15d]", 42);
    assert_string_equal(str->value, "[             42]");

    stringFormat(str, "[% 15d]", -42);
    assert_string_equal(str->value, "[            -42]");

    stringFormat(str, "[% 15d]", -42);
    assert_string_equal(str->value, "[            -42]");

    stringFormat(str, "[% 15.3f]", -42.987);
    assert_string_equal(str->value, "[        -42.987]");

    stringFormat(str, "[% 15.3f]", 42.987);
    assert_string_equal(str->value, "[         42.987]");

    stringFormat(str, "[% s]", "Hello testing");
    assert_string_equal(str->value, "[Hello testing]");

    stringFormat(str, "[% d]", 1024);
    assert_string_equal(str->value, "[ 1024]");

    stringFormat(str, "[% d]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[% i]", 1024);
    assert_string_equal(str->value, "[ 1024]");

    stringFormat(str, "[% i]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[% u]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[% u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272]");

    stringFormat(str, "[% o]", 511);
    assert_string_equal(str->value, "[777]");

    stringFormat(str, "[% o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001]");

    stringFormat(str, "[% x]", 305441741);
    assert_string_equal(str->value, "[1234abcd]");

    stringFormat(str, "[% x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433]");

    stringFormat(str, "[% X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD]");

    stringFormat(str, "[% X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433]");

    stringFormat(str, "[% c]", 'x');
    assert_string_equal(str->value, "[x]");
    return MUNIT_OK;
}

static MunitResult testFormatPlusFlag(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%+d]", 42);
    assert_string_equal(str->value, "[+42]");

    stringFormat(str, "[%+d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[%+5d]", 42);
    assert_string_equal(str->value, "[  +42]");

    stringFormat(str, "[%+5d]", -42);
    assert_string_equal(str->value, "[  -42]");

    stringFormat(str, "[%+15d]", 42);
    assert_string_equal(str->value, "[            +42]");

    stringFormat(str, "[%+15d]", -42);
    assert_string_equal(str->value, "[            -42]");

    stringFormat(str, "[%+s]", "Hello testing");
    assert_string_equal(str->value, "[Hello testing]");

    stringFormat(str, "[%+d]", 1024);
    assert_string_equal(str->value, "[+1024]");

    stringFormat(str, "[%+d]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%+i]", 1024);
    assert_string_equal(str->value, "[+1024]");

    stringFormat(str, "[%+i]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%+u]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%+u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272]");

    stringFormat(str, "[%+o]", 511);
    assert_string_equal(str->value, "[777]");

    stringFormat(str, "[%+o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001]");

    stringFormat(str, "[%+x]", 305441741);
    assert_string_equal(str->value, "[1234abcd]");

    stringFormat(str, "[%+x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433]");

    stringFormat(str, "[%+X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD]");

    stringFormat(str, "[%+X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433]");

    stringFormat(str, "[%+c]", 'x');
    assert_string_equal(str->value, "[x]");

    stringFormat(str, "[%+.0d]", 0);
    assert_string_equal(str->value, "[+]");
    return MUNIT_OK;
}

static MunitResult testFormatZeroFlag(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%0d]", 42);
    assert_string_equal(str->value, "[42]");

    stringFormat(str, "[%0ld]", 42L);
    assert_string_equal(str->value, "[42]");

    stringFormat(str, "[%0d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[%05d]", 42);
    assert_string_equal(str->value, "[00042]");

    stringFormat(str, "[%05d]", -42);
    assert_string_equal(str->value, "[-0042]");

    stringFormat(str, "[%015d]", 42);
    assert_string_equal(str->value, "[000000000000042]");

    stringFormat(str, "[%015d]", -42);
    assert_string_equal(str->value, "[-00000000000042]");

    stringFormat(str, "[%015.2f]", 42.1234);
    assert_string_equal(str->value, "[000000000042.12]");

    stringFormat(str, "[%015.3f]", 42.9876);
    assert_string_equal(str->value, "[00000000042.988]");

    stringFormat(str, "[%015.5f]", -42.9876);
    assert_string_equal(str->value, "[-00000042.98760]");
    return MUNIT_OK;
}

static MunitResult testFormatMinusFlag(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%-d]", 42);
    assert_string_equal(str->value, "[42]");

    stringFormat(str, "[%-d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[%-5d]", 42);
    assert_string_equal(str->value, "[42   ]");

    stringFormat(str, "[%-5d]", -42);
    assert_string_equal(str->value, "[-42  ]");

    stringFormat(str, "[%-15d]", 42);
    assert_string_equal(str->value, "[42             ]");

    stringFormat(str, "[%-15d]", -42);
    assert_string_equal(str->value, "[-42            ]");

    stringFormat(str, "[%-0d]", 42);
    assert_string_equal(str->value, "[42]");

    stringFormat(str, "[%-0d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[%-05d]", 42);
    assert_string_equal(str->value, "[42   ]");

    stringFormat(str, "[%-05d]", -42);
    assert_string_equal(str->value, "[-42  ]");

    stringFormat(str, "[%-015d]", 42);
    assert_string_equal(str->value, "[42             ]");

    stringFormat(str, "[%-015d]", -42);
    assert_string_equal(str->value, "[-42            ]");

    stringFormat(str, "[%0-d]", 42);
    assert_string_equal(str->value, "[42]");

    stringFormat(str, "[%0-d]", -42);
    assert_string_equal(str->value, "[-42]");

    stringFormat(str, "[%0-5d]", 42);
    assert_string_equal(str->value, "[42   ]");

    stringFormat(str, "[%0-5d]", -42);
    assert_string_equal(str->value, "[-42  ]");

    stringFormat(str, "[%0-15d]", 42);
    assert_string_equal(str->value, "[42             ]");

    stringFormat(str, "[%0-15d]", -42);
    assert_string_equal(str->value, "[-42            ]");

    stringFormat(str, "[%0-15.3e]", -42.);
    assert_string_equal(str->value, "[-4.200e+001    ]");

    stringFormat(str, "[%0-15.3g]", -42.);
    assert_string_equal(str->value, "[-42.0          ]");
    return MUNIT_OK;
}

static MunitResult testFormatHashFlag(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%#.0x]", 0);
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%#.1x]", 0);
    assert_string_equal(str->value, "[0]");

    stringFormat(str, "[%#.0llx]", (long long) 0);
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%#.8x]", 0x614e);
    assert_string_equal(str->value, "[0x0000614e]");

    stringFormat(str, "[%#b]", 6);
    assert_string_equal(str->value, "[0b110]");

    return MUNIT_OK;
}

static MunitResult testFormatSpecifier(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[Hello testing]");
    assert_string_equal(str->value, "[Hello testing]");

    stringFormat(str, "[%c]", 'A');
    assert_string_equal(str->value, "[A]");

    stringFormat(str, "[%s]", "Hello testing");
    assert_string_equal(str->value, "[Hello testing]");

    stringFormat(str, "[%n]");
    assert_string_equal(str->value, "[\n]");

    stringFormat(str, "[%d]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%d]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%i]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%i]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%u]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272]");

    stringFormat(str, "[%o]", 511);
    assert_string_equal(str->value, "[777]");

    stringFormat(str, "[%o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001]");

    stringFormat(str, "[%x]", 305441741);
    assert_string_equal(str->value, "[1234abcd]");

    stringFormat(str, "[%x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433]");

    stringFormat(str, "[%X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD]");

    stringFormat(str, "[%X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433]");

    stringFormat(str, "[%%]");
    assert_string_equal(str->value, "[%]");

    // Special cases
    stringFormat(str, "[%S]", NEW_STRING_32("BufferString test"));
    assert_string_equal(str->value, "[BufferString test]");

    int8_t val_1 = -67;
    stringFormat(str, "[%I8]", val_1);
    assert_string_equal(str->value, "[-67]");

    uint8_t val_2 = 234;
    stringFormat(str, "[%U8]", val_2);
    assert_string_equal(str->value, "[234]");

    int16_t val_3 = -4567;
    stringFormat(str, "[%I16]", val_3);
    assert_string_equal(str->value, "[-4567]");

    uint16_t val_4 = 5678;
    stringFormat(str, "[%U16]", val_4);
    assert_string_equal(str->value, "[5678]");

    int32_t val_5 = -345466;
    stringFormat(str, "[%I32]", val_5);
    assert_string_equal(str->value, "[-345466]");

    uint32_t val_6 = 34534545;
    stringFormat(str, "[%U32]", val_6);
    assert_string_equal(str->value, "[34534545]");

    int64_t val_7 = -45635636565;
    stringFormat(str, "[%I64]", val_7);
    assert_string_equal(str->value, "[-45635636565]");

    uint64_t val_8 = 6452452452352345;
    stringFormat(str, "[%U64]", val_8);
    assert_string_equal(str->value, "[6452452452352345]");
    return MUNIT_OK;
}

static MunitResult testFormatWidth(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%1s]", "Hello testing");
    assert_string_equal(str->value, "[Hello testing]");

    stringFormat(str, "[%1d]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%1d]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%1i]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%1i]", -1024);
    assert_string_equal(str->value, "[-1024]");

    stringFormat(str, "[%1u]", 1024);
    assert_string_equal(str->value, "[1024]");

    stringFormat(str, "[%1u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272]");

    stringFormat(str, "[%1o]", 511);
    assert_string_equal(str->value, "[777]");

    stringFormat(str, "[%1o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001]");

    stringFormat(str, "[%1x]", 305441741);
    assert_string_equal(str->value, "[1234abcd]");

    stringFormat(str, "[%1x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433]");

    stringFormat(str, "[%1X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD]");

    stringFormat(str, "[%1X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433]");

    stringFormat(str, "[%1c]", 'x');
    assert_string_equal(str->value, "[x]");
    return MUNIT_OK;
}

static MunitResult testFormatWidth20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%20s]", "Hello");
    assert_string_equal(str->value, "[               Hello]");

    stringFormat(str, "[%20S]", NEW_STRING_32("Hello"));
    assert_string_equal(str->value, "[               Hello]");

    stringFormat(str, "[%20d]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20d]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%20i]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20i]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%20u]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20u]", 4294966272U);
    assert_string_equal(str->value, "[          4294966272]");

    stringFormat(str, "[%20o]", 511);
    assert_string_equal(str->value, "[                 777]");

    stringFormat(str, "[%20o]", 4294966785U);
    assert_string_equal(str->value, "[         37777777001]");

    stringFormat(str, "[%20x]", 305441741);
    assert_string_equal(str->value, "[            1234abcd]");

    stringFormat(str, "[%20x]", 3989525555U);
    assert_string_equal(str->value, "[            edcb5433]");

    stringFormat(str, "[%20X]", 305441741);
    assert_string_equal(str->value, "[            1234ABCD]");

    stringFormat(str, "[%20X]", 3989525555U);
    assert_string_equal(str->value, "[            EDCB5433]");

    stringFormat(str, "[%20c]", 'x');
    assert_string_equal(str->value, "[                   x]");
    return MUNIT_OK;
}

static MunitResult testFormatWidthStar20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%*s]", 20, "Hello");
    assert_string_equal(str->value, "[               Hello]");

    stringFormat(str, "[%*d]", 20, 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%*d]", 20, -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%*i]", 20, 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%*i]", 20, -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%*u]", 20, 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%*u]", 20, 4294966272U);
    assert_string_equal(str->value, "[          4294966272]");

    stringFormat(str, "[%*o]", 20, 511);
    assert_string_equal(str->value, "[                 777]");

    stringFormat(str, "[%*o]", 20, 4294966785U);
    assert_string_equal(str->value, "[         37777777001]");

    stringFormat(str, "[%*x]", 20, 305441741);
    assert_string_equal(str->value, "[            1234abcd]");

    stringFormat(str, "[%*x]", 20, 3989525555U);
    assert_string_equal(str->value, "[            edcb5433]");

    stringFormat(str, "[%*X]", 20, 305441741);
    assert_string_equal(str->value, "[            1234ABCD]");

    stringFormat(str, "[%*X]", 20, 3989525555U);
    assert_string_equal(str->value, "[            EDCB5433]");

    stringFormat(str, "[%*c]", 20,'x');
    assert_string_equal(str->value, "[                   x]");
    return MUNIT_OK;
}

static MunitResult testFormatWidthMinus20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%-20s]", "Hello");
    assert_string_equal(str->value, "[Hello               ]");

    stringFormat(str, "[%-20d]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%-20d]", -1024);
    assert_string_equal(str->value, "[-1024               ]");

    stringFormat(str, "[%-20i]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%-20i]", -1024);
    assert_string_equal(str->value, "[-1024               ]");

    stringFormat(str, "[%-20u]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%-20.4f]", 1024.1234);
    assert_string_equal(str->value, "[1024.1234           ]");

    stringFormat(str, "[%-20u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272          ]");

    stringFormat(str, "[%-20o]", 511);
    assert_string_equal(str->value, "[777                 ]");

    stringFormat(str, "[%-20o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001         ]");

    stringFormat(str, "[%-20x]", 305441741);
    assert_string_equal(str->value, "[1234abcd            ]");

    stringFormat(str, "[%-20x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433            ]");

    stringFormat(str, "[%-20X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD            ]");

    stringFormat(str, "[%-20X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433            ]");

    stringFormat(str, "[%-20c]", 'x');
    assert_string_equal(str->value, "[x                   ]");

    stringFormat(str, "[|%5d| |%-2d| |%5d|]", 9, 9, 9);
    assert_string_equal(str->value, "[|    9| |9 | |    9|]");

    stringFormat(str, "[|%5d| |%-2d| |%5d|]", 10, 10, 10);
    assert_string_equal(str->value, "[|   10| |10| |   10|]");

    stringFormat(str, "[|%5d| |%-12d| |%5d|]", 9, 9, 9);
    assert_string_equal(str->value, "[|    9| |9           | |    9|]");

    stringFormat(str, "[|%5d| |%-12d| |%5d|]", 10, 10, 10);
    assert_string_equal(str->value, "[|   10| |10          | |   10|]");
    return MUNIT_OK;
}

static MunitResult testFormatWidthFrom0To20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%0-20s]", "Hello");
    assert_string_equal(str->value, "[Hello               ]");

    stringFormat(str, "[%0-20d]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%0-20d]", -1024);
    assert_string_equal(str->value, "[-1024               ]");

    stringFormat(str, "[%0-20i]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%0-20i]", -1024);
    assert_string_equal(str->value, "[-1024               ]");

    stringFormat(str, "[%0-20u]", 1024);
    assert_string_equal(str->value, "[1024                ]");

    stringFormat(str, "[%0-20u]", 4294966272U);
    assert_string_equal(str->value, "[4294966272          ]");

    stringFormat(str, "[%0-20o]", 511);
    assert_string_equal(str->value, "[777                 ]");

    stringFormat(str, "[%0-20o]", 4294966785U);
    assert_string_equal(str->value, "[37777777001         ]");

    stringFormat(str, "[%0-20x]", 305441741);
    assert_string_equal(str->value, "[1234abcd            ]");

    stringFormat(str, "[%0-20x]", 3989525555U);
    assert_string_equal(str->value, "[edcb5433            ]");

    stringFormat(str, "[%0-20X]", 305441741);
    assert_string_equal(str->value, "[1234ABCD            ]");

    stringFormat(str, "[%0-20X]", 3989525555U);
    assert_string_equal(str->value, "[EDCB5433            ]");

    stringFormat(str, "[%0-20c]", 'x');
    assert_string_equal(str->value, "[x                   ]");
    return MUNIT_OK;
}

static MunitResult testFormatPadding20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%020d]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%020d]", -1024);
    assert_string_equal(str->value, "[-0000000000000001024]");

    stringFormat(str, "[%020i]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%020i]", -1024);
    assert_string_equal(str->value, "[-0000000000000001024]");

    stringFormat(str, "[%020u]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%020u]", 4294966272U);
    assert_string_equal(str->value, "[00000000004294966272]");

    stringFormat(str, "[%020o]", 511);
    assert_string_equal(str->value, "[00000000000000000777]");

    stringFormat(str, "[%020o]", 4294966785U);
    assert_string_equal(str->value, "[00000000037777777001]");

    stringFormat(str, "[%020x]", 305441741);
    assert_string_equal(str->value, "[0000000000001234abcd]");

    stringFormat(str, "[%020x]", 3989525555U);
    assert_string_equal(str->value, "[000000000000edcb5433]");

    stringFormat(str, "[%020X]", 305441741);
    assert_string_equal(str->value, "[0000000000001234ABCD]");

    stringFormat(str, "[%020X]", 3989525555U);
    assert_string_equal(str->value, "[000000000000EDCB5433]");
    return MUNIT_OK;
}

static MunitResult testFormatPaddingDot20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%.20d]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%.20d]", -1024);
    assert_string_equal(str->value, "[-00000000000000001024]");

    stringFormat(str, "[%.20i]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%.20i]", -1024);
    assert_string_equal(str->value, "[-00000000000000001024]");

    stringFormat(str, "[%.20u]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%.20u]", 4294966272U);
    assert_string_equal(str->value, "[00000000004294966272]");

    stringFormat(str, "[%.20o]", 511);
    assert_string_equal(str->value, "[00000000000000000777]");

    stringFormat(str, "[%.20o]", 4294966785U);
    assert_string_equal(str->value, "[00000000037777777001]");

    stringFormat(str, "[%.20x]", 305441741);
    assert_string_equal(str->value, "[0000000000001234abcd]");

    stringFormat(str, "[%.20x]", 3989525555U);
    assert_string_equal(str->value, "[000000000000edcb5433]");

    stringFormat(str, "[%.20X]", 305441741);
    assert_string_equal(str->value, "[0000000000001234ABCD]");

    stringFormat(str, "[%.20X]", 3989525555U);
    assert_string_equal(str->value, "[000000000000EDCB5433]");
    return MUNIT_OK;
}

static MunitResult testFormatZeroPaddingHash20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%#020d]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%#020d]", -1024);
    assert_string_equal(str->value, "[-0000000000000001024]");

    stringFormat(str, "[%#020i]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%#020i]", -1024);
    assert_string_equal(str->value, "[-0000000000000001024]");

    stringFormat(str, "[%#020u]", 1024);
    assert_string_equal(str->value, "[00000000000000001024]");

    stringFormat(str, "[%#020u]", 4294966272U);
    assert_string_equal(str->value, "[00000000004294966272]");

    stringFormat(str, "[%#020o]", 511);
    assert_string_equal(str->value, "[00000000000000000777]");

    stringFormat(str, "[%#020o]", 4294966785U);
    assert_string_equal(str->value, "[00000000037777777001]");

    stringFormat(str, "[%#020x]", 305441741);
    assert_string_equal(str->value, "[0x00000000001234abcd]");

    stringFormat(str, "[%#020x]", 3989525555U);
    assert_string_equal(str->value, "[0x0000000000edcb5433]");

    stringFormat(str, "[%#020X]", 305441741);
    assert_string_equal(str->value, "[0X00000000001234ABCD]");

    stringFormat(str, "[%#020X]", 3989525555U);
    assert_string_equal(str->value, "[0X0000000000EDCB5433]");
    return MUNIT_OK;
}

static MunitResult testFormatWhitespacePaddingHash20(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%#20d]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%#20d]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%#20i]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%#20i]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%#20u]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%#20u]", 4294966272U);
    assert_string_equal(str->value, "[          4294966272]");

    stringFormat(str, "[%#20o]", 511);
    assert_string_equal(str->value, "[                0777]");

    stringFormat(str, "[%#20o]", 4294966785U);
    assert_string_equal(str->value, "[        037777777001]");

    stringFormat(str, "[%#20x]", 305441741);
    assert_string_equal(str->value, "[          0x1234abcd]");

    stringFormat(str, "[%#20x]", 3989525555U);
    assert_string_equal(str->value, "[          0xedcb5433]");

    stringFormat(str, "[%#20X]", 305441741);
    assert_string_equal(str->value, "[          0X1234ABCD]");

    stringFormat(str, "[%#20X]", 3989525555U);
    assert_string_equal(str->value, "[          0XEDCB5433]");
    return MUNIT_OK;
}

static MunitResult testFormatWhitespacePadding20_5(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%20.5d]", 1024);
    assert_string_equal(str->value, "[               01024]");

    stringFormat(str, "[%20.5d]", -1024);
    assert_string_equal(str->value, "[              -01024]");

    stringFormat(str, "[%20.5i]", 1024);
    assert_string_equal(str->value, "[               01024]");

    stringFormat(str, "[%20.5i]", -1024);
    assert_string_equal(str->value, "[              -01024]");

    stringFormat(str, "[%20.5u]", 1024);
    assert_string_equal(str->value, "[               01024]");

    stringFormat(str, "[%20.5u]", 4294966272U);
    assert_string_equal(str->value, "[          4294966272]");

    stringFormat(str, "[%20.5o]", 511);
    assert_string_equal(str->value, "[               00777]");

    stringFormat(str, "[%20.5o]", 4294966785U);
    assert_string_equal(str->value, "[         37777777001]");

    stringFormat(str, "[%20.5x]", 305441741);
    assert_string_equal(str->value, "[            1234abcd]");

    stringFormat(str, "[%20.10x]", 3989525555U);
    assert_string_equal(str->value, "[          00edcb5433]");

    stringFormat(str, "[%20.5X]", 305441741);
    assert_string_equal(str->value, "[            1234ABCD]");

    stringFormat(str, "[%20.10X]", 3989525555U);
    assert_string_equal(str->value, "[          00EDCB5433]");
    return MUNIT_OK;
}

static MunitResult testFormatPaddingNegativeNumbers(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    // space padding
    stringFormat(str, "[% 1d]", -5);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[% 2d]", -5);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[% 3d]", -5);
    assert_string_equal(str->value, "[ -5]");

    stringFormat(str, "[% 4d]", -5);
    assert_string_equal(str->value, "[  -5]");

    // zero padding
    stringFormat(str, "[%01d]", -5);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[%02d]", -5);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[%03d]", -5);
    assert_string_equal(str->value, "[-05]");

    stringFormat(str, "[%04d]", -5);
    assert_string_equal(str->value, "[-005]");
    return MUNIT_OK;
}

static MunitResult testFormatPaddingNegativeFloats(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    // space padding
    stringFormat(str, "[% 3.1f]", -5.);
    assert_string_equal(str->value, "[-5.0]");

    stringFormat(str, "[% 4.1f]", -5.);
    assert_string_equal(str->value, "[-5.0]");

    stringFormat(str, "[% 5.1f]", -5.);
    assert_string_equal(str->value, "[ -5.0]");

    stringFormat(str, "[% 6.1g]", -5.);
    assert_string_equal(str->value, "[    -5]");

    stringFormat(str, "[% 6.1e]", -5.);
    assert_string_equal(str->value, "[-5.0e+000]");

    stringFormat(str, "[% 10.1e]", -5.);
    assert_string_equal(str->value, "[ -5.0e+000]");

    // zero padding
    stringFormat(str, "[%03.1f]", -5.);
    assert_string_equal(str->value, "[-5.0]");

    stringFormat(str, "[%04.1f]", -5.);
    assert_string_equal(str->value, "[-5.0]");

    stringFormat(str, "[%05.1f]", -5.);
    assert_string_equal(str->value, "[-05.0]");

    // zero padding no decimal point
    stringFormat(str, "[%01.0f]", -5.);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[%02.0f]", -5.);
    assert_string_equal(str->value, "[-5]");

    stringFormat(str, "[%03.0f]", -5.);
    assert_string_equal(str->value, "[-05]");

    stringFormat(str, "[%010.1e]", -5.);
    assert_string_equal(str->value, "[-05.0e+000]");

    stringFormat(str, "[%07.0E]", -5.);
    assert_string_equal(str->value, "[-5E+000]");

    stringFormat(str, "[%03.0g]", -5.);
    assert_string_equal(str->value, "[-05]");
    return MUNIT_OK;
}

static MunitResult testFormatLengthField(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%.0s]", "Hello testing");
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%20.0s]", "Hello testing");
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%.s]", "Hello testing");
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%20.s]", "Hello testing");
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.0d]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20.0d]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%20.d]", 0);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.0i]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20.i]", -1024);
    assert_string_equal(str->value, "[               -1024]");

    stringFormat(str, "[%20.i]", 0);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.u]", 1024);
    assert_string_equal(str->value, "[                1024]");

    stringFormat(str, "[%20.0u]", 4294966272U);
    assert_string_equal(str->value, "[          4294966272]");

    stringFormat(str, "[%20.u]", 0U);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.o]", 511);
    assert_string_equal(str->value, "[                 777]");

    stringFormat(str, "[%20.0o]", 4294966785U);
    assert_string_equal(str->value, "[         37777777001]");

    stringFormat(str, "[%20.o]", 0U);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.x]", 305441741);
    assert_string_equal(str->value, "[            1234abcd]");

    stringFormat(str, "[%50.x]", 305441741);
    assert_string_equal(str->value, "[                                          1234abcd]");

    stringFormat(str, "[%50.x%10.u]", 305441741, 12345);
    assert_string_equal(str->value, "[                                          1234abcd     12345]");

    stringFormat(str, "[%20.0x]", 3989525555U);
    assert_string_equal(str->value, "[            edcb5433]");

    stringFormat(str, "[%20.x]", 0U);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%20.X]", 305441741);
    assert_string_equal(str->value, "[            1234ABCD]");

    stringFormat(str, "[%20.0X]", 3989525555U);
    assert_string_equal(str->value, "[            EDCB5433]");

    stringFormat(str, "[%20.X]", 0U);
    assert_string_equal(str->value, "[                    ]");

    stringFormat(str, "[%02.0u]", 0U);
    assert_string_equal(str->value, "[  ]");

    stringFormat(str, "[%02.0d]", 0);
    assert_string_equal(str->value, "[  ]");
    return MUNIT_OK;
}

static MunitResult testFormatFloat(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    // test special-case floats using math.h macros
    stringFormat(str, "[%8f]", NAN);
    assert_string_equal(str->value, "[     nan]");

    stringFormat(str, "[%8f]", INFINITY);
    assert_string_equal(str->value, "[     inf]");

    stringFormat(str, "[%-8f]", -INFINITY);
    assert_string_equal(str->value, "[-inf    ]");

    stringFormat(str, "[%+8e]", INFINITY);
    assert_string_equal(str->value, "[    +inf]");

//    stringFormat(str, "[%.4f]", 3.1415354);
//    assert_string_equal(str->value, "[3.1415]");

    stringFormat(str, "[%.3f]", 30343.1415354);
    assert_string_equal(str->value, "[30343.142]");

    stringFormat(str, "[%.0f]", 34.1415354);
    assert_string_equal(str->value, "[34]");

    stringFormat(str, "[%.0f]", 1.3);
    assert_string_equal(str->value, "[1]");

    stringFormat(str, "[%.0f]", 1.55);
    assert_string_equal(str->value, "[2]");

    stringFormat(str, "[%.1f]", 1.64);
    assert_string_equal(str->value, "[1.6]");

    stringFormat(str, "[%.2f]", 42.8952);
    assert_string_equal(str->value, "[42.90]");

    stringFormat(str, "[%.9f]", 42.8952);
    assert_string_equal(str->value, "[42.895200000]");

    stringFormat(str, "[%.10f]", 42.895223);
    assert_string_equal(str->value, "[42.8952230000]");

    // this testcase checks, that the precision is truncated to 9 digits.
    // a perfect working float should return the whole number
    stringFormat(str, "[%.12f]", 42.89522312345678);
    assert_string_equal(str->value, "[42.895223123000]");

    // this testcase checks, that the precision is truncated AND rounded to 9 digits.
    // a perfect working float should return the whole number
    stringFormat(str, "[%.12f]", 42.89522387654321);
    assert_string_equal(str->value, "[42.895223877000]");

    stringFormat(str, "[%6.2f]", 42.8952);
    assert_string_equal(str->value, "[ 42.90]");

    stringFormat(str, "[%+6.2f]", 42.8952);
    assert_string_equal(str->value, "[+42.90]");

    stringFormat(str, "[%+5.1f]", 42.9252);
    assert_string_equal(str->value, "[+42.9]");

    stringFormat(str, "[%f]", 42.5);
    assert_string_equal(str->value, "[42.500000]");

    stringFormat(str, "[%.1f]", 42.5);
    assert_string_equal(str->value, "[42.5]");

    stringFormat(str, "[%f]", 42167.0);
    assert_string_equal(str->value, "[42167.000000]");

    stringFormat(str, "[%.9f]", -12345.987654321);
    assert_string_equal(str->value, "[-12345.987654321]");

    stringFormat(str, "[%.1f]", 3.999);
    assert_string_equal(str->value, "[4.0]");

    stringFormat(str, "[%.0f]", 3.5);
    assert_string_equal(str->value, "[4]");

    stringFormat(str, "[%.0f]", 4.5);
    assert_string_equal(str->value, "[5]");

    stringFormat(str, "[%.0f]", 3.49);
    assert_string_equal(str->value, "[3]");

    stringFormat(str, "[%.1f]", 3.49);
    assert_string_equal(str->value, "[3.5]");

    stringFormat(str, "[a%-5.1f]", 0.5);
    assert_string_equal(str->value, "[a0.5  ]");

    stringFormat(str, "[a%-5.1fend]", 0.5);
    assert_string_equal(str->value, "[a0.5  end]");

    stringFormat(str, "[%G]", 12345.678);
    assert_string_equal(str->value, "[12345.7]");

    stringFormat(str, "[%.7G]", 12345.678);
    assert_string_equal(str->value, "[12345.68]");

    stringFormat(str, "[%.5G]", 123456789.);
    assert_string_equal(str->value, "[1.2346E+008]");

    stringFormat(str, "[%.6G]", 12345.);
    assert_string_equal(str->value, "[12345.0]");

    stringFormat(str, "[%+12.4g]", 123456789.);
    assert_string_equal(str->value, "[ +1.235e+008]");

//    stringFormat(str, "[%.2G]", 0.001234);
//    assert_string_equal(str->value, "[0.0012]");

    stringFormat(str, "[%+10.4G]", 0.001234);
    assert_string_equal(str->value, "[ +0.001234]");

    stringFormat(str, "[%+012.4g]", 0.00001234);
    assert_string_equal(str->value, "[+01.234e-005]");

    stringFormat(str, "[%.3g]", -1.2345e-308);
    assert_string_equal(str->value, "[-1.23e-308]");

    stringFormat(str, "[%+.3E]", 1.23e+308);
    assert_string_equal(str->value, "[+1.230E+308]");

    // out of range for float: should switch to exp notation
    stringFormat(str, "[%.1f]", 1E20);
    assert_string_equal(str->value, "[1.0e+020]");
    return MUNIT_OK;
}

static MunitResult testFormatTypes(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%i]", 0);
    assert_string_equal(str->value, "[0]");

    stringFormat(str, "[%i]", 1234);
    assert_string_equal(str->value, "[1234]");

    stringFormat(str, "[%i]", 32767);
    assert_string_equal(str->value, "[32767]");

    stringFormat(str, "[%i]", -32767);
    assert_string_equal(str->value, "[-32767]");

    stringFormat(str, "[%li]", 30L);
    assert_string_equal(str->value, "[30]");

    stringFormat(str, "[%li]", -2147483647L);
    assert_string_equal(str->value, "[-2147483647]");

    stringFormat(str, "[%li]", 2147483647L);
    assert_string_equal(str->value, "[2147483647]");

    stringFormat(str, "[%lli]", 30LL);
    assert_string_equal(str->value, "[30]");

    stringFormat(str, "[%lli]", -9223372036854775807LL);
    assert_string_equal(str->value, "[-9223372036854775807]");

    stringFormat(str, "[%lli]", 9223372036854775807LL);
    assert_string_equal(str->value, "[9223372036854775807]");

    stringFormat(str, "[%lu]", 100000L);
    assert_string_equal(str->value, "[100000]");

    stringFormat(str, "[%lu]", 0xFFFFFFFFL);
    assert_string_equal(str->value, "[4294967295]");

    stringFormat(str, "[%llu]", 281474976710656LLU);
    assert_string_equal(str->value, "[281474976710656]");

    stringFormat(str, "[%llu]", 18446744073709551615LLU);
    assert_string_equal(str->value, "[18446744073709551615]");

    stringFormat(str, "[%b]", 60000);
    assert_string_equal(str->value, "[1110101001100000]");

    stringFormat(str, "[%lb]", 12345678L);
    assert_string_equal(str->value, "[101111000110000101001110]");

    stringFormat(str, "[%o]", 60000);
    assert_string_equal(str->value, "[165140]");

    stringFormat(str, "[%lo]", 12345678L);
    assert_string_equal(str->value, "[57060516]");

    stringFormat(str, "[%lx]", 0x12345678L);
    assert_string_equal(str->value, "[12345678]");

    stringFormat(str, "[%llx]", 0x1234567891234567LLU);
    assert_string_equal(str->value, "[1234567891234567]");

    stringFormat(str, "[%lx]", 0xabcdefabL);
    assert_string_equal(str->value, "[abcdefab]");

    stringFormat(str, "[%lX]", 0xabcdefabL);
    assert_string_equal(str->value, "[ABCDEFAB]");

    stringFormat(str, "[%c]", 'v');
    assert_string_equal(str->value, "[v]");

    stringFormat(str, "[%cv]", 'w');
    assert_string_equal(str->value, "[wv]");

    stringFormat(str, "[%s]", "A Test");
    assert_string_equal(str->value, "[A Test]");

    stringFormat(str, "[%hhu]", 0xFFFFUL);
    assert_string_equal(str->value, "[255]");

    stringFormat(str, "[%hu]", 0x123456UL);
    assert_string_equal(str->value, "[13398]");

    stringFormat(str, "[%s%hhi %hu]", "Test", 10000, 0xFFFFFFFF);
    assert_string_equal(str->value, "[Test16 65535]");
    return MUNIT_OK;
}

static MunitResult testFormatPointer(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%p]", (void *) 0x1234U);
    if (sizeof(void*) == 4U) {
        assert_string_equal(str->value, "[00001234]");
    }
    else {
        assert_string_equal(str->value, "[0000000000001234]");
    }

    stringFormat(str, "[%p]", (void *) 0x12345678U);
    if (sizeof(void*) == 4U) {
        assert_string_equal(str->value, "[12345678]");
    }
    else {
        assert_string_equal(str->value, "[0000000012345678]");
    }

    stringFormat(str, "[%p-%p]", (void *) 0x12345678U, (void *) 0x7EDCBA98U);
    if (sizeof(void*) == 4U) {
        assert_string_equal(str->value, "[12345678-7EDCBA98]");
    }
    else {
        assert_string_equal(str->value, "[0000000012345678-000000007EDCBA98]");
    }

    if (sizeof(uintptr_t) == sizeof(uint64_t)) {
        stringFormat(str, "[%p]", (void *) (uintptr_t) 0xFFFFFFFFU);
        assert_string_equal(str->value, "[00000000FFFFFFFF]");
    }
    else {
        stringFormat(str, "[%p]", (void *) (uintptr_t) 0xFFFFFFFFU);
        assert_string_equal(str->value, "[FFFFFFFF]");
    }
    return MUNIT_OK;
}

static MunitResult testFormatUnknownField(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%kmarco]", 42, 37);
    assert_string_equal(str->value, "[kmarco]");

    stringFormat(str, "[%q]", "test");
    assert_string_equal(str->value, "[q]");
    return MUNIT_OK;
}

static MunitResult testFormatStringLength(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%.4s]", "This is a test");
    assert_string_equal(str->value, "[This]");

    stringFormat(str, "[%.4s]", "test");
    assert_string_equal(str->value, "[test]");

    stringFormat(str, "[%.7s]", "123");
    assert_string_equal(str->value, "[123]");

    stringFormat(str, "[%.7s]", "");
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%.4s%.2s]", "123456", "abcdef");
    assert_string_equal(str->value, "[1234ab]");

    stringFormat(str, "[%.4.2s]", "123456");
    assert_string_equal(str->value, "[.2s]");

    stringFormat(str, "[%.*s]", 3, "123456");
    assert_string_equal(str->value, "[123]");
    return MUNIT_OK;
}

static MunitResult testFormatBufferSafety(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(16);

    BufferString *nullStr = stringFormat(NULL, "[%s]", "test");
    assert_null(nullStr);

    nullStr = stringFormat(str, NULL, "test");
    assert_null(nullStr);

    nullStr = stringFormat(str, "[%s]", "Test buffer overflow");
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[");

    nullStr = stringFormat(str, "[%s %d]", "Overflow int", 12345);
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[Overflow int 1");
    assert_uint32(str->length, ==, 15);

    nullStr = stringFormat(str, "[%-16d]", 123456); // right padding overflow
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[123456        ");
    assert_uint32(str->length, ==, 15);

    nullStr = stringFormat(str, "[%20d]", 123456); // left padding overflow
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[              ");
    assert_uint32(str->length, ==, 15);

    nullStr = stringFormat(str, "[%020d]", 123456); // zero padding overflow
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[00000000000000");
    assert_uint32(str->length, ==, 15);

    nullStr = stringFormat(str, "[%.9f]", 123456.1234567891); // float overflow
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[123456.1234567");
    assert_uint32(str->length, ==, 15);

    nullStr = stringFormat(str, "[%10.9f]", 123456.12345); // float padding overflow
    assert_null(nullStr);
    assert_not_null(str);
    assert_string_equal(str->value, "[123456.1234500");
    assert_uint32(str->length, ==, 15);
    return MUNIT_OK;
}

static MunitResult testFormatCustomType(const MunitParameter params[], void *testData) {
    BufferString *str = EMPTY_STRING(128);

    stringFormat(str, "[%S]", NEW_STRING_16("Hello Test"));
    assert_string_equal(str->value, "[Hello Test]");

    stringFormat(str, "[%S]", EMPTY_STRING(8));
    assert_string_equal(str->value, "[]");

    stringFormat(str, "[%S]", NULL);
    assert_string_equal(str->value, "[");

    stringFormat(str, "[%#b]", 60000);
    assert_string_equal(str->value, "[0b1110101001100000]");

    stringFormat(str, "[%.20U8]", 102);
    assert_string_equal(str->value, "[00000000000000000102]");

    stringFormat(str, "[%.20I8]", -102);
    assert_string_equal(str->value, "[-00000000000000000102]");

    stringFormat(str, "[%20.5U16]", 1024);
    assert_string_equal(str->value, "[               01024]");

    stringFormat(str, "[%20.5I16]", -1024);
    assert_string_equal(str->value, "[              -01024]");

    stringFormat(str, "[%20U32]", UINT32_MAX);
    assert_string_equal(str->value, "[          4294967295]");

    stringFormat(str, "[%-20I32]", INT32_MIN);
    assert_string_equal(str->value, "[-2147483648         ]");

    stringFormat(str, "[%030U64]", UINT64_MAX);
    assert_string_equal(str->value, "[000000000018446744073709551615]");

    stringFormat(str, "[%-030I64]", INT64_MIN);
    assert_string_equal(str->value, "[-9223372036854775808          ]");
    return MUNIT_OK;
}

static MunitResult testConcatString(const MunitParameter params[], void *testData) {
    BufferString *first = NEW_STRING_128("first");
    BufferString *second = concatChars(first, " second");
    BufferString *third = concatString(second, NEW_STRING_16(" third"));
    validateString(third, "first second third", 18, 128);

    BufferString *smallStr = EMPTY_STRING(12);
    BufferString *errorStr = concatChars(smallStr, "this text is way too long");
    assert_null(errorStr);

    BufferString *okStr = concatChars(smallStr, "ok");
    concatChars(okStr, " yes");
    validateString(okStr, "ok yes", 6, 12);
    return MUNIT_OK;
}

static MunitResult testCopyString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("test text to replace");
    copyString(str, "new test text");
    validateString(str, "new test text", 13, 64);

    BufferString *smallStr = EMPTY_STRING(4);
    BufferString *fail = copyString(smallStr, "too long text");
    assert_null(fail);
    copyStringByLength(smallStr, "really long long text", 3);   // should not fail by length
    validateString(smallStr, "rea", 3, 4);
    return MUNIT_OK;
}

static MunitResult testClearString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_32("abCdFg123%$*op[]}");
    clearString(str);
    assert_uint32(str->length, ==, 0);
    for (int i = 0; i < str->capacity; i++) {
        assert_char(str->value[i], ==, 0);
    }
    return MUNIT_OK;
}

static MunitResult testLowerCaseString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_32("abCdFg123%$*op[]} ");
    str = toLowerCase(str);
    assert_string_equal(stringValue(str), "abcdfg123%$*op[]} ");
    return MUNIT_OK;
}

static MunitResult testUpperCaseString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_32("abCdFg123%$*op[]} ");
    str = toUpperCase(str);
    assert_string_equal(stringValue(str), "ABCDFG123%$*OP[]} ");
    return MUNIT_OK;
}

static MunitResult testSwapCaseString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Test sTriNg sWAP cASe ok");
    str = swapCase(str);
    validateString(str, "tEST StRInG Swap CasE OK", 24, 64);
    return MUNIT_OK;
}

static MunitResult testReplaceFirstOccurrence(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Start test string abc ok abc end");
    replaceFirstOccurrence(str, "abc", "cba");
    validateString(str, "Start test string cba ok abc end", 32, 64);
    return MUNIT_OK;
}

static MunitResult testReplaceAllOccurrences(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Start test string abc ok abc end");
    replaceAllOccurrences(str, "abc", "");
    validateString(str, "Start test string  ok  end", 26, 64);
    return MUNIT_OK;
}

static MunitResult testTrimString(const MunitParameter params[], void *testData) {
    const char *stringWithSpaces = munit_parameters_get(params, "stringWithSpaces");
    BufferString *str = NEW_STRING_32(stringWithSpaces);
    trimAll(str);
    assert_string_equal(stringValue(str), "test");
    assert_int32(stringLength(str), ==, 4);

    BufferString *emptyStr_1 = NEW_STRING_32(" ");
    trimAll(emptyStr_1);
    assert_string_equal(stringValue(emptyStr_1), "");
    assert_int32(stringLength(emptyStr_1), ==, 0);

    BufferString *emptyStr_2 = NEW_STRING_32("       ");
    trimAll(emptyStr_2);
    assert_string_equal(stringValue(emptyStr_2), "");
    assert_int32(stringLength(emptyStr_2), ==, 0);
    return MUNIT_OK;
}

static MunitResult testReverseString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("ko eb dluohs gnirts tset desreveR");
    reverseString(str);
    validateString(str, "Reversed test string should be ok", 33, 64);
    return MUNIT_OK;
}

static MunitResult testCapitalizeString(const MunitParameter params[], void *testData) {
    BufferString *empty = NULL;
    capitalize(empty, NULL, 0);
    assert_null(empty);

    empty = EMPTY_STRING(32);
    capitalize(empty, NULL, 0);
    assert_not_null(empty);
    assert_true(isBuffStringEmpty(empty));

    BufferString *str = NEW_STRING_32("i am fine");
    capitalize(str, NULL, 0);
    assert_string_equal(stringValue(str), "I Am Fine");

    str = NEW_STRING_32("i aM.fine");
    capitalize(str, (char[]){'.'}, 1);
    assert_string_equal(stringValue(str), "I aM.Fine");

    str = NEW_STRING_64("some _test&with/different$#separators@t");
    capitalize(str, (char[]){' ', '_', '&', '/', '$', '#'}, 6);
    assert_string_equal(stringValue(str), "Some _Test&With/Different$#Separators@t");
    return MUNIT_OK;
}

static MunitResult testSubstringFrom(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc");
    BufferString *result = EMPTY_STRING(64);
    substringFrom(str, result, 0);
    validateString(result, "abc", 3, 64);

    substringFrom(str, result, 1);
    validateString(result, "bc", 2, 64);

    substringFrom(str, result, 2);
    validateString(result, "c", 1, 64);

    substringFrom(str, result, 3);
    validateString(result, "", 0, 64);

    substringFrom(str, result, 4);
    validateString(result, "", 0, 64);
    return MUNIT_OK;
}

static MunitResult testSubstringFromTo(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("hamburger");
    BufferString *result = SUBSTRING(64, str, 4, 8);
    validateString(result, "urge", 4, 64);

    BufferString *str_2 = NEW_STRING_64("smiles");
    result = SUBSTRING(64, str_2, 1, 5);
    validateString(result, "mile", 4, 64);

    result = SUBSTRING(64, str_2, 5, 6);
    validateString(result, "s", 1, 64);

    result = SUBSTRING(64, str_2, 6, 7);    // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSubstringAfter(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abcba");
    BufferString *result = SUBSTRING_AFTER(64, str, "a");
    validateString(result, "bcba", 4, 64);

    result = SUBSTRING_AFTER(64, str, "b");
    validateString(result, "cba", 3, 64);

    result = SUBSTRING_AFTER(64, str, "ba");
    validateString(result, "", 0, 64);

    result = SUBSTRING_AFTER(64, str, "d"); // not existing value
    validateString(result, "", 0, 64);

    result = SUBSTRING_AFTER(3, str, "a");  // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSubstringAfterLast(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abcba");
    BufferString *result = SUBSTRING_AFTER_LAST(64, str, "b");
    validateString(result, "a", 1, 64);

    result = SUBSTRING_AFTER_LAST(64, str, "a");
    validateString(result, "", 0, 64);

    result = SUBSTRING_AFTER_LAST(64, str, "d"); // not existing value
    validateString(result, "", 0, 64);

    result = SUBSTRING_AFTER_LAST(3, str, "ab");  // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSubstringBefore(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abcba");
    BufferString *result = SUBSTRING_BEFORE(64, str, "a");
    validateString(result, "", 0, 64);

    result = SUBSTRING_BEFORE(64, str, "b");
    validateString(result, "a", 1, 64);

    result = SUBSTRING_BEFORE(64, str, "c");
    validateString(result, "ab", 2, 64);

    result = SUBSTRING_BEFORE(64, str, "d"); // not existing value
    validateString(result, "", 0, 64);

    result = SUBSTRING_BEFORE(3, str, "ba");  // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSubstringBeforeLast(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abcba");
    BufferString *result = SUBSTRING_BEFORE_LAST(64, str, "b");
    validateString(result, "abc", 3, 64);

    result = SUBSTRING_BEFORE_LAST(64, str, "c");
    validateString(result, "ab", 2, 64);

    result = SUBSTRING_BEFORE_LAST(64, str, "a");
    validateString(result, "abcb", 4, 64);

    result = SUBSTRING_BEFORE_LAST(64, str, "d"); // not existing value
    validateString(result, "", 0, 64);

    result = SUBSTRING_BEFORE_LAST(3, str, "b");  // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSubstringBetween(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("tagabctag");
    BufferString *result = SUBSTRING_BETWEEN(64, str, "tag", "tag");
    validateString(result, "abc", 3, 64);

    BufferString *notExistingStartTag = SUBSTRING_BETWEEN(64, str, "tags", "tag");
    assert_null(notExistingStartTag);

    BufferString *notExistingEndTag = SUBSTRING_BETWEEN(64, str, "tag", "tags");
    assert_null(notExistingEndTag);

    BufferString *str_2 = NEW_STRING_64("yabczyabcz");
    result = SUBSTRING_BETWEEN(64, str_2, "y", "z");
    validateString(result, "abc", 3, 64);

    BufferString *str_3 = NEW_STRING_128(TEST_STRING);
    result = SUBSTRING_BETWEEN(64, str_3, "+IPD,", ":");
    validateString(result, "1,497", 5, 64);

    BufferString *str_4 = NEW_STRING_128(TEST_STRING);
    result = SUBSTRING_BETWEEN(3, str_4, "+IPD,", ":");   // overflow
    assert_null(result);
    return MUNIT_OK;
}

static MunitResult testSplitString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("/api/test/json/product=1234/utm_source");
    BufferString *token = EMPTY_STRING(64);

    StringIterator iterNotFoundToken = getStringSplitIterator(str, "&");
    bool hasToken = hasNextSplitToken(&iterNotFoundToken, token);
    assert_false(hasToken);
    assert_string_equal(stringValue(token), "");    // nothing should be copied to result

    StringIterator iterator = getStringSplitIterator(str, "/");
    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "api");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "test");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "json");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "product=1234");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_true(hasToken);
    assert_string_equal(stringValue(token), "utm_source");

    hasToken = hasNextSplitToken(&iterator, token);
    assert_false(hasToken); // end of string
    return MUNIT_OK;
}

static MunitResult testJoinChars(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Already have some content.");
    joinChars(str, "-", 3, " New", "content", "added");
    validateString(str, "Already have some content. New-content-added", 44, 64);

    BufferString *emptyStr = EMPTY_STRING(64);
    joinChars(emptyStr, "|||---|||", 3, "Some", "content", "added");
    validateString(emptyStr, "Some|||---|||content|||---|||added", 34, 64);

    BufferString *smallStr = NEW_STRING_16("aaa");
    BufferString *result = joinChars(smallStr, "-", 8, "Some", "content", "that", "is", "large", "than", "provided", "capacity");
    assert_null(result);
    validateString(smallStr, "aaa", 3, 16);

    BufferString *smallStr_2 = NEW_STRING_16("aaa");
    BufferString *result_2 = joinChars(smallStr_2, "-", 4, "aaaa", "bbbb", "bbb", "c");
    assert_null(result_2); // concatenate delimiter overflows buffer
    validateString(smallStr_2, "aaa", 3, 16);

    return MUNIT_OK;
}

static MunitResult testJoinArrayString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Already have some content.");
    char *tokens[3] = {" New", "content", "added"};
    joinStringArray(str, "-", 3, tokens);
    validateString(str, "Already have some content. New-content-added", 44, 64);

    BufferString *emptyStr = EMPTY_STRING(64);
    char *tokens_2[3] = {"Some", "content", "added"};
    joinStringArray(emptyStr, "|||---|||", 3, tokens_2);
    validateString(emptyStr, "Some|||---|||content|||---|||added", 34, 64);

    BufferString *smallStr = NEW_STRING_16("aaa");
    char *tokens_3[8] = {"Some", "content", "that", "is", "large", "than", "provided", "capacity"};

    BufferString *result = joinStringArray(smallStr, "-", 8, tokens_3);
    assert_null(result);
    validateString(smallStr, "aaa", 3, 16);

    BufferString *smallStr_2 = NEW_STRING_16("aaa");
    char *tokens_4[4] = {"aaaa", "bbbb", "bbb", "c"};
    BufferString *result_2 = joinStringArray(smallStr_2, "-", 4, tokens_4);
    assert_null(result_2); // concatenate delimiter overflows buffer
    validateString(smallStr_2, "aaa", 3, 16);
    return MUNIT_OK;
}

static MunitResult testJoinStrings(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("Already have some content.");
    BufferString *testStr1 = NEW_STRING_16("");
    BufferString *testStr2 = NEW_STRING_16("acvd");
    BufferString *testStr3 = NEW_STRING_16("  []  ");

    joinStrings(str, "-", 3, testStr1, testStr2, testStr3);
    validateString(str, "Already have some content.-acvd-  []  ", 38, 64);

    BufferString *emptyStr = EMPTY_STRING(64);
    joinStrings(emptyStr, "|", 3, NEW_STRING_16("test_1"), NEW_STRING_16("test_2"), NEW_STRING_16("test_3"));
    validateString(emptyStr, "test_1|test_2|test_3", 20, 64);

    BufferString *smallStr = NEW_STRING_16("aaa");
    BufferString *result = joinStrings(smallStr, "|", 3, NEW_STRING_16("test_1234"), NEW_STRING_16("test_25678"), NEW_STRING_16("test_3435345"));
    assert_null(result);
    validateString(smallStr, "aaa", 3, 16);

    BufferString *smallStr_2 = NEW_STRING_16("sdf");
    BufferString *result_2 = joinStrings(smallStr_2, "-", 4, NEW_STRING_16("aaaa"), NEW_STRING_16("bbbb"), NEW_STRING_16("bb"), NEW_STRING_16("c"));
    assert_null(result_2); // concatenate delimiter overflows buffer
    validateString(smallStr_2, "sdf", 3, 16);
    return MUNIT_OK;
}

static MunitResult testRepeatChar(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_16("Test ");
    str = repeatChar(str, 'a', 5);
    validateString(str, "Test aaaaa", 10, 16);

    str = repeatChar(str, ' ', 1);
    validateString(str, "Test aaaaa ", 11, 16);

    str = repeatChar(str, 'b', 4);
    validateString(str, "Test aaaaa bbbb", 15, 16); // string is full

    // check for overflow, should return NULL
    str = repeatChar(str, ' ', 1);
    assert_null(str);

    // NULL check
    str = repeatChar(str, 't', 1);
    assert_null(str);
    return MUNIT_OK;
}

static MunitResult testRepeatChars(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_16("Test ");
    str = repeatChars(str, "test", 1);
    validateString(str, "Test test", 9, 16);

    str = repeatChars(str, " ", 1);
    validateString(str, "Test test ", 10, 16);

    str = repeatChars(str, "bb", 2);
    validateString(str, "Test test bbbb", 14, 16);

    // check for overflow, should return NULL
    str = repeatChars(str, "cc", 2);
    assert_null(str);

    // NULL check
    str = repeatChars(str, "d", 2);
    assert_null(str);
    return MUNIT_OK;
}

static MunitResult testInt64ToString(const MunitParameter params[], void *testData) {
    BufferString *int64_1 = INT64_TO_STRING(12345);
    BufferString *int64_2 = INT64_TO_STRING(-12345);
    BufferString *int64_3 = INT64_TO_STRING(0);
    BufferString *int64_4 = INT64_TO_STRING(INT64_MAX);
    BufferString *int64_5 = INT64_TO_STRING(INT64_MIN);

    validateString(int64_1, "12345", 5, 32);
    validateString(int64_2, "-12345", 6, 32);
    validateString(int64_3, "0", 1, 32);
    validateString(int64_4, "9223372036854775807", 19, 32);
    validateString(int64_5, "-9223372036854775808", 20, 32);
    return MUNIT_OK;
}

static MunitResult testUint64ToString(const MunitParameter params[], void *testData) {
    BufferString *uint64_1 = UINT64_TO_STRING(12345);
    BufferString *uint64_2 = UINT64_TO_STRING(23);
    BufferString *uint64_3 = UINT64_TO_STRING(0);
    BufferString *uint64_4 = UINT64_TO_STRING(UINT64_MAX);

    validateString(uint64_1, "12345", 5, 32);
    validateString(uint64_2, "23", 2, 32);
    validateString(uint64_3, "0", 1, 32);
    validateString(uint64_4, "18446744073709551615", 20, 32);
    return MUNIT_OK;
}

static MunitResult testStringToI64(const MunitParameter params[], void *testData) {
    int64_t i;
    /* Lazy to calculate this size properly. */
    char s[128];

    /* Simple case. */
    assert_int(cStrToInt64("11", &i, 10), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, 11);

    /* Negative number . */
    assert_int(cStrToInt64("-11", &i, 10), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, -11);

    /* Different base. */
    assert_int(cStrToInt64("11", &i, 16), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, 17);

    /* 0 */
    assert_int(cStrToInt64("0", &i, 10), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, 0);

    /* INT_MAX. */
    sprintf(s, "%d", INT_MAX);
    assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, INT_MAX);

    /* INT_MIN. */
    sprintf(s, "%d", INT_MIN);
    assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_SUCCESS);
    assert_int(i, ==, INT_MIN);

    /* Leading and trailing space. */
    assert_int(cStrToInt64(" 1", &i, 10), ==, STR_TO_I64_INCONVERTIBLE);
    assert_int(cStrToInt64("1 ", &i, 10), ==, STR_TO_I64_INCONVERTIBLE);

    /* Trash characters. */
    assert_int(cStrToInt64("a10", &i, 10), ==, STR_TO_I64_INCONVERTIBLE);
    assert_int(cStrToInt64("10a", &i, 10), ==, STR_TO_I64_INCONVERTIBLE);

    /* i64_t overflow.
     *
     * `if` needed to avoid undefined behaviour
     * on `INT_MAX + 1` if INT_MAX == LONG_MAX.
     */
    if (LLONG_MAX < LLONG_MAX) {
        sprintf(s, "%lld", (long  long)LLONG_MAX + 1L);
        assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_OVERFLOW);
    }

    /* int underflow */
    if (LLONG_MIN < LLONG_MIN) {
        sprintf(s, "%lld", (long  long)LLONG_MIN - 1L);
        assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_UNDERFLOW);
    }

    /* long overflow */
    sprintf(s, "%lld0", LLONG_MAX);
    assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_OVERFLOW);

    /* long underflow */
    sprintf(s, "%lld0", LLONG_MIN);
    assert_int(cStrToInt64(s, &i, 10), ==, STR_TO_I64_UNDERFLOW);

    return MUNIT_OK;
}

static MunitResult testIsBuffStringBlank(const MunitParameter params[], void *testData) {
    BufferString *testStr1 = NEW_STRING_16("");
    BufferString *testStr2 = NEW_STRING_16("acvd");
    BufferString *testStr3 = NEW_STRING_16("     ");
    BufferString *testStr4 = NEW_STRING_16("   t  ");
    BufferString *testStr5 = NEW_STRING_16("\n\t\r");
    BufferString *testStr6 = NEW_STRING_16(NULL);

    assert_true(isBuffStrBlank(testStr1));
    assert_false(isBuffStrBlank(testStr2));
    assert_true(isBuffStrBlank(testStr3));
    assert_false(isBuffStrBlank(testStr4));
    assert_true(isBuffStrBlank(testStr5));
    assert_true(isBuffStrBlank(testStr6));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringEquals(const MunitParameter params[], void *testData) {
    assert_true(isBuffStrEquals(NEW_STRING_16(NULL), NEW_STRING_16(NULL)));
    assert_true(isBuffStrEquals(NEW_STRING_16("b"), NEW_STRING_16("b")));
    assert_true(isBuffStrEquals(NEW_STRING_16("123"), NEW_STRING_16("123")));
    assert_true(isBuffStrEquals(NEW_STRING_16(")"), NEW_STRING_16(")")));

    assert_false(isBuffStrEquals(NEW_STRING_16("a"), NEW_STRING_16(NULL)));
    assert_false(isBuffStrEquals(NEW_STRING_16(NULL), NEW_STRING_16("b")));
    assert_false(isBuffStrEquals(NEW_STRING_16("bB"), NEW_STRING_16("b")));
    assert_false(isBuffStrEquals(NEW_STRING_16("bB"), NEW_STRING_16("bb")));
    assert_false(isBuffStrEquals(NEW_STRING_16("#@"), NEW_STRING_16("@#")));

    assert_false(isBuffStrEqualsCstr(NEW_STRING_16("a"), NULL));
    assert_false(isBuffStrEqualsCstr(NEW_STRING_16(NULL), "b"));
    assert_false(isBuffStrEqualsCstr(NEW_STRING_16("bB"), "b"));
    assert_false(isBuffStrEqualsCstr(NEW_STRING_16("#@"), "@#"));
    assert_true(isBuffStrEqualsCstr(NEW_STRING_16("bb"), "bb"));
    assert_true(isBuffStrEqualsCstr(NEW_STRING_16("aaabcf"), "aaabcf"));
    assert_true(isBuffStrEqualsCstr(NEW_STRING_16("AAABCF"), "AAABCF"));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringEqualsIgnoreCase(const MunitParameter params[], void *testData) {
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16(NULL), NEW_STRING_16(NULL)));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16("b"), NEW_STRING_16("b")));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16("123"), NEW_STRING_16("123")));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16(")"), NEW_STRING_16(")")));

    assert_false(isBuffStrEqualsIgnoreCase(NEW_STRING_16("a"), NEW_STRING_16(NULL)));
    assert_false(isBuffStrEqualsIgnoreCase(NEW_STRING_16(NULL), NEW_STRING_16("b")));
    assert_false(isBuffStrEqualsIgnoreCase(NEW_STRING_16("bB"), NEW_STRING_16("b")));
    assert_false(isBuffStrEqualsIgnoreCase(NEW_STRING_16("#@"), NEW_STRING_16("@#")));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16("bB"), NEW_STRING_16("bb")));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16("aAaBcF"), NEW_STRING_16("aaabcf")));
    assert_true(isBuffStrEqualsIgnoreCase(NEW_STRING_16("aAaBcF"), NEW_STRING_16("AAABCF")));

    return MUNIT_OK;
}

static MunitResult testIndexOfChar(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_16("abcdefgh");
    assert_int32(indexOfChar(str, 'a', 0), ==, 0);
    assert_int32(indexOfChar(str, 'b', 0), ==, 1);
    assert_int32(indexOfChar(str, 'c', 0), ==, 2);
    assert_int32(indexOfChar(str, 'd', 0), ==, 3);
    assert_int32(indexOfChar(str, 'e', 0), ==, 4);
    assert_int32(indexOfChar(str, 'f', 0), ==, 5);
    assert_int32(indexOfChar(str, 'g', 0), ==, 6);
    assert_int32(indexOfChar(str, 'h', 0), ==, 7);
    assert_int32(indexOfChar(str, 'j', 0), ==, -1);
    return MUNIT_OK;
}

static MunitResult testIndexOfString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def ghj 123, t");
    assert_int32(indexOfString(str, "abc", 0), ==, 0);
    assert_int32(indexOfString(str, "def", 0), ==, 4);
    assert_int32(indexOfString(str, " gh", 0), ==, 7);
    assert_int32(indexOfString(str, "23", 0), ==, 13);
    assert_int32(indexOfString(str, "t", 0), ==, 17);
    assert_int32(indexOfString(str, "not in", 0), ==, -1);
    assert_int32(indexOfString(str, "ff", 0), ==, -1);
    assert_int32(indexOfString(str, "*", 0), ==, -1);

    assert_int32(indexOfString(NULL, "*", 0), ==, -1);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), NULL, 0), ==, -1);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "a", 0), ==, 0);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "b", 0), ==, 2);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "ab", 0), ==, 1);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "b", 3), ==, 5);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "b", 9), ==, -1);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "", 2), ==, 2);
    assert_int32(indexOfString(NEW_STRING_16("aabaabaa"), "", 9), ==, -1);
    return MUNIT_OK;
}

static MunitResult testLastIndexOfString(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def bc 123abc, te te");
    assert_int32(lastIndexOfString(str, "abc"), ==, 14);
    assert_int32(lastIndexOfString(str, "bc"), ==, 15);
    assert_int32(lastIndexOfString(str, "f"), ==, 6);
    assert_int32(lastIndexOfString(str, "te"), ==, 22);
    assert_int32(lastIndexOfString(str, "abc "), ==, 0);
    assert_int32(lastIndexOfString(str, "cde"), ==, -1);

    assert_int32(lastIndexOfString(NULL, "*"), ==, -1);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), NULL), ==, -1);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), "a"), ==, 7);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), "b"), ==, 5);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), "ab"), ==, 4);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), "b"), ==, 5);
    assert_int32(lastIndexOfString(NEW_STRING_16("aabaabaa"), ""), ==, 8);
    return MUNIT_OK;
}

static MunitResult testIsStringStartsWith(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def bca");
    assert_true(isStringStartsWith(str, "abc", 0));
    assert_true(isStringStartsWith(str, "ab", 0));
    assert_true(isStringStartsWith(str, "a", 0));
    assert_true(isStringStartsWith(str, " def", 3));
    assert_true(isStringStartsWith(str, " bc", 7));

    assert_false(isStringStartsWith(str, "b", 0));
    assert_false(isStringStartsWith(str, "cba", 0));
    assert_false(isStringStartsWith(str, " f", 3));
    assert_false(isStringStartsWith(str, "A", 10)); // check case
    return MUNIT_OK;
}

static MunitResult testIsStringStartsWithIgnoreCase(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("aBc DEf bCa");
    assert_true(isStringStartsWithIgnoreCase(str, "abc", 0));
    assert_true(isStringStartsWithIgnoreCase(str, "ab", 0));
    assert_true(isStringStartsWithIgnoreCase(str, "a", 0));
    assert_true(isStringStartsWithIgnoreCase(str, " def", 3));
    assert_true(isStringStartsWithIgnoreCase(str, " bc", 7));
    assert_true(isStringStartsWithIgnoreCase(str, "A", 10));

    assert_false(isStringStartsWithIgnoreCase(str, "b", 0));
    assert_false(isStringStartsWithIgnoreCase(str, "cba", 0));
    assert_false(isStringStartsWithIgnoreCase(str, " f", 3));
    return MUNIT_OK;
}

static MunitResult testCharAt(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def bcA");
    assert_char(charAt(str, 0), ==, 'a');
    assert_char(charAt(str, 1), ==, 'b');
    assert_char(charAt(str, 2), ==, 'c');
    assert_char(charAt(str, 3), ==, ' ');
    assert_char(charAt(str, 4), ==, 'd');
    assert_char(charAt(str, 5), ==, 'e');
    assert_char(charAt(str, 6), ==, 'f');
    assert_char(charAt(str, 7), ==, ' ');
    assert_char(charAt(str, 8), ==, 'b');
    assert_char(charAt(str, 9), ==, 'c');
    assert_char(charAt(str, 10), ==, 'A');
    assert_char(charAt(str, 11), ==, 0);
    assert_char(charAt(NULL, 0), ==, 0);
    assert_char(charAt(NULL, 2), ==, 0);
    return MUNIT_OK;
}

static MunitResult testContainsStr(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def bcA");
    assert_true(containsStr(str, "abc def bcA"));
    assert_true(containsStr(str, "abc"));
    assert_true(containsStr(str, "def"));
    assert_true(containsStr(str, " de"));
    assert_true(containsStr(str, "b"));
    assert_true(containsStr(str, "A"));

    assert_false(containsStr(str, "abC"));
    assert_false(containsStr(str, "ff"));
    assert_false(containsStr(str, "  "));
    assert_false(containsStr(str, "\n"));

    assert_false(containsStr(NULL, "*"));
    assert_false(containsStr(NEW_STRING_16("*"), NULL));
    assert_true(containsStr(NEW_STRING_16(""), ""));
    assert_true(containsStr(NEW_STRING_16("abc"), ""));
    assert_true(containsStr(NEW_STRING_16("abc"), "a"));
    assert_false(containsStr(NEW_STRING_16("abc"), "z"));
    assert_false(containsStr(NEW_STRING_16("ABCDEF"), "def"));
    assert_true(containsStr(NEW_STRING_16("ABCDEF"), "DEF"));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringEmpty(const MunitParameter params[], void *testData) {
    BufferString *testStr1 = NEW_STRING_16("");
    BufferString *testStr2 = NEW_STRING_16("acvd");
    BufferString *testStr3 = NEW_STRING_16("     ");
    BufferString *testStr4 = NEW_STRING_16("   t  ");
    BufferString *testStr5 = NEW_STRING_16("\n\t\r");
    BufferString *testStr6 = NEW_STRING_16(NULL);

    assert_true(isBuffStringEmpty(testStr1));
    assert_false(isBuffStringEmpty(testStr2));
    assert_false(isBuffStringEmpty(testStr3));
    assert_false(isBuffStringEmpty(testStr4));
    assert_false(isBuffStringEmpty(testStr5));
    assert_true(isBuffStringEmpty(testStr6));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringNotEmpty(const MunitParameter params[], void *testData) {
    assert_true(isBuffStringNotEmpty(NEW_STRING_16("a")));
    assert_true(isBuffStringNotEmpty(NEW_STRING_16("abc")));
    assert_true(isBuffStringNotEmpty(NEW_STRING_16(" ")));
    assert_true(isBuffStringNotEmpty(NEW_STRING_16("  ")));

    assert_false(isBuffStringNotEmpty(NEW_STRING_16("")));
    assert_false(isBuffStringNotEmpty(NEW_STRING_16(NULL)));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringNotBlank(const MunitParameter params[], void *testData) {
    assert_true(isBuffStringNotBlank(NEW_STRING_16("a")));
    assert_true(isBuffStringNotBlank(NEW_STRING_16("abc")));

    assert_false(isBuffStringNotBlank(NEW_STRING_16(" ")));
    assert_false(isBuffStringNotBlank(NEW_STRING_16("  ")));
    assert_false(isBuffStringNotBlank(NEW_STRING_16("")));
    assert_false(isBuffStringNotBlank(NEW_STRING_16(NULL)));
    return MUNIT_OK;
}

static MunitResult testIsBuffStringNotEquals(const MunitParameter params[], void *testData) {
    assert_true(isBuffStringNotEquals(NEW_STRING_16("abc"), NEW_STRING_16("abC")));
    assert_true(isBuffStringNotEquals(NEW_STRING_16("abc"), NEW_STRING_16("ab")));
    assert_true(isBuffStringNotEquals(NEW_STRING_16(" "), NEW_STRING_16("  ")));
    assert_true(isBuffStringNotEquals(NEW_STRING_16("\n"), NEW_STRING_16("")));

    assert_false(isBuffStringNotEquals(NEW_STRING_16("a"), NEW_STRING_16("a")));
    assert_false(isBuffStringNotEquals(NEW_STRING_16("ac"), NEW_STRING_16("ac")));
    assert_false(isBuffStringNotEquals(NEW_STRING_16("abc"), NEW_STRING_16("abc")));
    return MUNIT_OK;
}

static MunitResult testStringProperties(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_64("abc def bcA");
    assert_string_equal(stringValue(str), "abc def bcA");
    assert_uint32(stringLength(str), ==, 11);
    assert_uint32(stringCapacity(str), ==, 64);
    return MUNIT_OK;
}

static MunitResult testStringParsing(const MunitParameter params[], void *testData) {
    BufferString *str = NEW_STRING_1024(ESP_RESPONSE);
    BufferString *result = EMPTY_STRING(2048);

    BufferString *token = EMPTY_STRING(128);
    StringIterator iterator = getStringSplitIterator(str, "\n");    // get every line separated by new line character
    while (hasNextSplitToken(&iterator, token)) {
        BufferString *values = SUBSTRING_BETWEEN(64, token, "+CWLAP:(", ")");  // extract comma separated values
        replaceAllOccurrences(values, "\"", "");    // remove all escaped quotes from SSID
        BufferString *encryption = SUBSTRING(2, values, 0, indexOfChar(values, ',', 0));
        uint32_t nextValueIndex = (encryption->length + 1);
        BufferString *apName = SUBSTRING(64, values, nextValueIndex, indexOfChar(values, ',', nextValueIndex)); // extract SSID
        nextValueIndex += (apName->length + 1);
        BufferString *signalStrength = SUBSTRING(5, values, nextValueIndex, indexOfChar(values, ',', nextValueIndex));
        nextValueIndex += (signalStrength->length + 1);
        BufferString *macAddress = SUBSTRING(24, values, nextValueIndex, stringLength(values));
        BufferString *formatStr = STRING_FORMAT_256("Encryption: [%s], SSID: [%-15s], Strength: [%s], MAC: [%s]%n",
                                                    stringValue(encryption),stringValue(apName),stringValue(signalStrength),stringValue(macAddress));
        concatString(result, formatStr);
    }

    assert_string_equal(result->value, "Encryption: [3], SSID: [CVBJB          ], Strength: [-71], MAC: [f8:e4:fb:5b:a9:5a]\n"
                                       "Encryption: [3], SSID: [HT_00d02d638ac3], Strength: [-90], MAC: [04:f0:21:0f:1f:61]\n"
                                       "Encryption: [3], SSID: [CLDRM          ], Strength: [-69], MAC: [22:c9:d0:1a:f6:54]\n"
                                       "Encryption: [2], SSID: [AllSaints      ], Strength: [-88], MAC: [c4:01:7c:3b:08:48]\n"
                                       "Encryption: [0], SSID: [AllSaints-Guest], Strength: [-83], MAC: [c4:01:7c:7b:08:48]\n");
    return MUNIT_OK;
}

static char *stringWithTrailingSpaces[] = {
        "   test   ",
        "test       ",
        "       test",
        "\ttest\t",
        "test\t",
        "\ttest",
        " test ",
        NULL
};

static MunitParameterEnum stringTestParameters1[] = {
        {.name = "stringWithSpaces", .values = stringWithTrailingSpaces},
        END_OF_PARAMETERS
};

static MunitTest bufferStringTests[] = {
        {.name =  "Test newString() - should correctly create new string", .test = testNewStringCreation},
        {.name =  "Test dubString() - should correctly duplicate existing string", .test = testDupString},

        {.name =  "Test stringFormat() - happy path", .test = testStringFormat},
        {.name =  "Test stringFormat() - test format \"space\" flag", .test = testFormatSpaceFlag},
        {.name =  "Test stringFormat() - test format \"plus\" flag", .test = testFormatPlusFlag},
        {.name =  "Test stringFormat() - test format \"zero\" flag", .test = testFormatZeroFlag},
        {.name =  "Test stringFormat() - test format \"minus\" flag", .test = testFormatMinusFlag},
        {.name =  "Test stringFormat() - test format \"hash\" flag", .test = testFormatHashFlag},
        {.name =  "Test stringFormat() - test format \"specifier\"", .test = testFormatSpecifier},
        {.name =  "Test stringFormat() - test format \"width\"", .test = testFormatWidth},
        {.name =  "Test stringFormat() - test format \"width 20\"", .test = testFormatWidth20},
        {.name =  "Test stringFormat() - test format \"width *20\"", .test = testFormatWidthStar20},
        {.name =  "Test stringFormat() - test format \"width -20\"", .test = testFormatWidthMinus20},
        {.name =  "Test stringFormat() - test format \"width from 0 to 20\"", .test = testFormatWidthFrom0To20},
        {.name =  "Test stringFormat() - test format \"padding 20\"", .test = testFormatPadding20},
        {.name =  "Test stringFormat() - test format \"padding .20\"", .test = testFormatPaddingDot20},
        {.name =  "Test stringFormat() - test format \"padding #20 with 0s\"", .test = testFormatZeroPaddingHash20},
        {.name =  "Test stringFormat() - test format \"padding #20 with whitespaces\"", .test = testFormatWhitespacePaddingHash20},
        {.name =  "Test stringFormat() - test format \"padding with precision 20.5\"", .test = testFormatWhitespacePadding20_5},
        {.name =  "Test stringFormat() - test format \"padding negative numbers\"", .test = testFormatPaddingNegativeNumbers},
        {.name =  "Test stringFormat() - test format \"padding negative floats\"", .test = testFormatPaddingNegativeFloats},
        {.name =  "Test stringFormat() - test format \"lenght\"", .test = testFormatLengthField},
        {.name =  "Test stringFormat() - test format \"float\"", .test = testFormatFloat},
        {.name =  "Test stringFormat() - test format \"types\"", .test = testFormatTypes},
        {.name =  "Test stringFormat() - test format \"pointer\"", .test = testFormatPointer},
        {.name =  "Test stringFormat() - test format \"unknown field\"", .test = testFormatUnknownField},
        {.name =  "Test stringFormat() - test format \"string length\"", .test = testFormatStringLength},
        {.name =  "Test stringFormat() - test format safety check", .test = testFormatBufferSafety},
        {.name =  "Test stringFormat() - test format custom type", .test = testFormatCustomType},

        {.name =  "Test concatChars() - should correctly concat chars to string", .test = testConcatString},
        {.name =  "Test copyString() - should correctly copy chars to string", .test = testCopyString},
        {.name =  "Test swapCase() - should correctly change string char case", .test = testSwapCaseString},

        {.name =  "Test clearString() - should correctly set all string values to zero", .test = testClearString},
        {.name =  "Test toLowerCaseString() - should set all chars to lower case", .test = testLowerCaseString},
        {.name =  "Test toUpperCaseString() - should set all chars to upper case", .test = testUpperCaseString},

        {.name =  "Test replaceFirstOccurrence() - should replace first target string", .test = testReplaceFirstOccurrence},
        {.name =  "Test replaceAllOccurrences() - should replace all target strings", .test = testReplaceAllOccurrences},

        {.name =  "Test trimAll() - should correctly remove trailing whitespaces", .test = testTrimString, .parameters = stringTestParameters1},
        {.name =  "Test reverseString() - should correctly reverse string characters", .test = testReverseString},
        {.name =  "Test capitalize() - should correctly set uppercase to words by delimiter", .test = testCapitalizeString},

        {.name =  "Test substringFrom() - should correctly substring from index", .test = testSubstringFrom},
        {.name =  "Test substringFromTo() - should correctly substring from index and end index", .test = testSubstringFromTo},
        {.name =  "Test substringAfter() - should correctly substring after the first occurrence of a separator", .test = testSubstringAfter},
        {.name =  "Test substringAfterLast() - should correctly substring after the last occurrence of a separator", .test = testSubstringAfterLast},
        {.name =  "Test substringBefore() - should correctly substring before the first occurrence of a separator", .test = testSubstringBefore},
        {.name =  "Test substringBeforeLast() - should correctly substring before the last occurrence of a separator", .test = testSubstringBeforeLast},
        {.name =  "Test substringBetween() - should correctly substring string that is nested in between two strings", .test = testSubstringBetween},

        {.name =  "Test split string - should correctly split and iterate token string", .test = testSplitString},

        {.name =  "Test joinChars() - should correctly join character elements with specified delimiter", .test = testJoinChars},
        {.name =  "Test joinStringArray() - should correctly join string array elements with specified delimiter", .test = testJoinArrayString},
        {.name =  "Test joinStrings() - should correctly join BufferString elements with specified delimiter", .test = testJoinStrings},

        {.name =  "Test repeatChar() - should correctly concat single char by count", .test = testRepeatChar},
        {.name =  "Test repeatChars() - should correctly concat string by count", .test = testRepeatChars},

        {.name =  "Test int64ToString() - should correctly convert long long to string", .test = testInt64ToString},
        {.name =  "Test uInt64ToString() - should correctly convert unsigned long long to string", .test = testUint64ToString},
        {.name =  "Test cStrToInt64() - should correctly convert string to long long", .test = testStringToI64},

        {.name =  "Test isBuffStringBlank() - should correctly check string blankness", .test = testIsBuffStringBlank},
        {.name =  "Test isBuffStringEquals() - should correctly check string equality", .test = testIsBuffStringEquals},
        {.name =  "Test isBuffStringEqualsIgnoreCase() - should correctly check string equality ignoring case", .test = testIsBuffStringEqualsIgnoreCase},

        {.name =  "Test indexOfChar() - should return index of char or -1 when not found", .test = testIndexOfChar},
        {.name =  "Test indexOfString() - should return index of string or -1 when not found", .test = testIndexOfString},
        {.name =  "Test lastIndexOfString() - should return last index of string or -1 when not found", .test = testLastIndexOfString},
        {.name =  "Test isStringStartsWith() - should correctly check that string starts with substring", .test = testIsStringStartsWith},
        {.name =  "Test isStringStartsWithIgnoreCase() - should correctly check that string starts with substring ignoring case", .test = testIsStringStartsWithIgnoreCase},

        {.name =  "Test charAt() - should correctly return index of char in string", .test = testCharAt},
        {.name =  "Test containsStr() - should check that string contains other string", .test = testContainsStr},
        {.name =  "Test isBuffStringEmpty() - should check that string is empty", .test = testIsBuffStringEmpty},
        {.name =  "Test isBuffStringNotEmpty() - should check that string is not empty", .test = testIsBuffStringNotEmpty},
        {.name =  "Test isBuffStringNotBlank() - should check that string is not bank", .test = testIsBuffStringNotBlank},
        {.name =  "Test isBuffStringNotEquals() - should check that string is not equals to other string", .test = testIsBuffStringNotEquals},
        {.name =  "Test string get methods - should return string struct values", .test = testStringProperties},

        {.name =  "Test string parsing - should correctly parse ESP8266 response", .test = testStringParsing},
        END_OF_TESTS
};

static const MunitSuite bufferStringTestSuite = {
        .prefix = "BufferString: ",
        .tests = bufferStringTests,
        .suites = NULL,
        .iterations = 1,
        .options = MUNIT_SUITE_OPTION_NONE
};