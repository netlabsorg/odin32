/* Unit test suite for Rtl large integer functions
 *
 * Copyright 2003 Thomas Mertes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * NOTES
 * We use function pointers here as there is no import library for NTDLL on
 * windows.
 */

#include <stdlib.h>
#include <string.h>

#include "winbase.h"
#include "test.h"
#include "winnt.h"
#include "winnls.h"
#include "winternl.h"

/* Function ptrs for ntdll calls */
static HMODULE hntdll = 0;
static LONGLONG (WINAPI *pRtlExtendedMagicDivide)(LONGLONG, LONGLONG, INT);
static VOID     (WINAPI *pRtlFreeAnsiString)(PSTRING);
static NTSTATUS (WINAPI *pRtlInt64ToUnicodeString)(ULONGLONG, ULONG, UNICODE_STRING *);
static NTSTATUS (WINAPI *pRtlLargeIntegerToChar)(ULONGLONG *, ULONG, ULONG, PCHAR);
static NTSTATUS (WINAPI *pRtlUnicodeStringToAnsiString)(STRING *, const UNICODE_STRING *, BOOLEAN);


static void InitFunctionPtrs()
{
    hntdll = LoadLibraryA("ntdll.dll");
    ok(hntdll != 0, "LoadLibrary failed");
    if (hntdll) {
	pRtlExtendedMagicDivide = (void *)GetProcAddress(hntdll, "RtlExtendedMagicDivide");
	pRtlFreeAnsiString = (void *)GetProcAddress(hntdll, "RtlFreeAnsiString");
	pRtlInt64ToUnicodeString = (void *)GetProcAddress(hntdll, "RtlInt64ToUnicodeString");
	pRtlLargeIntegerToChar = (void *)GetProcAddress(hntdll, "RtlLargeIntegerToChar");
	pRtlUnicodeStringToAnsiString = (void *)GetProcAddress(hntdll, "RtlUnicodeStringToAnsiString");
    } /* if */
}


typedef struct {
    LONGLONG a;
    LONGLONG b;
    INT shift;
    LONGLONG result;
} magic_divide_t;

static const magic_divide_t magic_divide[] = {
    {                  3,   0x5555555555555555U, 0,                   0}, /* 1 */
    {          333333333,   0x5555555555555555U, 0,           111111110}, /* 111111111 */
    { 0x7fffffffffffffff,   0x5555555555555555U, 0,  0x2aaaaaaaaaaaaaaa},
    {                  3,   0xaaaaaaaaaaaaaaaaU, 1,                   0}, /* 1 */
    {          333333333,   0xaaaaaaaaaaaaaaaaU, 1,           111111110}, /* 111111111 */
    { 0x7fffffffffffffff,   0xaaaaaaaaaaaaaaaaU, 1,  0x2aaaaaaaaaaaaaaa},
    {                 -3,   0x5555555555555555U, 0,                   0}, /* -1 */
    {         -333333333,   0x5555555555555555U, 0,          -111111110}, /* -111111111 */
    {-0x7fffffffffffffff,   0x5555555555555555U, 0, -0x2aaaaaaaaaaaaaaa},
    {                 -3,   0xaaaaaaaaaaaaaaaaU, 1,                   0}, /* -1 */
    {         -333333333,   0xaaaaaaaaaaaaaaaaU, 1,          -111111110}, /* -111111111 */
    {-0x7fffffffffffffff,   0xaaaaaaaaaaaaaaaaU, 1, -0x2aaaaaaaaaaaaaaa},
    {                 -3,  -0x5555555555555555U, 0,                  -2}, /* -1 */
    {         -333333333,  -0x5555555555555555U, 0,          -222222222}, /* -111111111 */
    {-0x7fffffffffffffff,  -0x5555555555555555U, 0, -0x5555555555555554},
    {                 -3,  -0xaaaaaaaaaaaaaaaaU, 1,                   0}, /* -1 */
    {         -333333333,  -0xaaaaaaaaaaaaaaaaU, 1,           -55555555}, /* -111111111 */
    {-0x7fffffffffffffff,  -0xaaaaaaaaaaaaaaaaU, 1, -0x1555555555555555},
    {                  3,  -0x5555555555555555U, 0,                   2}, /* -1 */
    {          333333333,  -0x5555555555555555U, 0,           222222222}, /* -111111111 */
    { 0x7fffffffffffffff,  -0x5555555555555555U, 0,  0x5555555555555554},
    {                  3,  -0xaaaaaaaaaaaaaaaaU, 1,                   0}, /* -1 */
    {          333333333,  -0xaaaaaaaaaaaaaaaaU, 1,            55555555}, /* -111111111 */
    { 0x7fffffffffffffff,  -0xaaaaaaaaaaaaaaaaU, 1,  0x1555555555555555},
    {                  3, 12297829382473033728U, 1,                   0}, /* 1 */
    {          333333333, 12297829382473033728U, 1,           111111110}, /* 111111111 */
    { 0x7fffffffffffffff, 12297829382473033728U, 1,  0x2aaaaaaaaaaaa9ff}, /* 0x2aaaaaaaaaaaaaaa */
    {                  5,  3689348814741910527U, 0,                   1},
    {          555555555,  3689348814741910527U, 0,           111111111},
    { 0x7fffffffffffffff,  3689348814741910527U, 0,  0x19999999999999ff}, /* 0x199999999999999a */
    {                  5,  7378697629483821054U, 1,                   1},
    {          555555555,  7378697629483821054U, 1,           111111111},
    { 0x7fffffffffffffff,  7378697629483821054U, 1,  0x19999999999999ff}, /* 0x199999999999999a */
    {                  5, 14757395258967642109U, 2,                   1},
    {          555555555, 14757395258967642109U, 2,           111111111},
    { 0x7fffffffffffffff, 14757395258967642109U, 2,  0x19999999999999ff}, /* 0x199999999999999a */
    {      0xaddcafeface,     0x2f1e28fd1b5ccaU, 33,                  1},
    {  0x81ac1b9c2310a80,     0x2f1e28fd1b5ccaU, 33,             0xbeef},
    { 0x74ae3b5f1558c800,     0x2f1e28fd1b5ccaU, 33,            0xabcde},
    {      0xaddcafeface,   0x2f1e28fd1b5cca00U, 41,                  1},
    {  0x81ac1b9c2310a80,   0x2f1e28fd1b5cca00U, 41,             0xbeef},
    { 0x74ae3b5f1558c800,   0x2f1e28fd1b5cca00U, 41,            0xabcde},

};
#define NB_MAGIC_DIVIDE (sizeof(magic_divide)/sizeof(*magic_divide))


static void test_RtlExtendedMagicDivide(void)
{
    int i;
    LONGLONG result;

    for (i = 0; i < NB_MAGIC_DIVIDE; i++) {
	result = pRtlExtendedMagicDivide(magic_divide[i].a, magic_divide[i].b, magic_divide[i].shift);
	ok(result == magic_divide[i].result,
	   "call failed: RtlExtendedMagicDivide(%lld, %llu, %d) has result %llx, expected %llx",
	   magic_divide[i].a, magic_divide[i].b, magic_divide[i].shift, result, magic_divide[i].result);
    }
}


#define LARGE_STRI_BUFFER_LENGTH 67

typedef struct {
    int base;
    ULONGLONG value;
    USHORT Length;
    USHORT MaximumLength;
    char *Buffer;
    NTSTATUS result;
} largeint2str_t;

/*
 * The native DLL does produce garbage or STATUS_BUFFER_OVERFLOW for
 * base 2, 8 and 16 when the value is larger than 0xFFFFFFFF.
 * Therefore these testcases are commented out.
 */

static const largeint2str_t largeint2str[] = {
    {10,          123,  3, 11, "123\0---------------------------------------------------------------", STATUS_SUCCESS},

    { 0,  0x80000000U, 10, 11, "2147483648\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,  -2147483647, 20, 21, "18446744071562067969\0----------------------------------------------", STATUS_SUCCESS},
    { 0,           -2, 20, 21, "18446744073709551614\0----------------------------------------------", STATUS_SUCCESS},
    { 0,           -1, 20, 21, "18446744073709551615\0----------------------------------------------", STATUS_SUCCESS},
    { 0,            0,  1, 11, "0\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 0,            1,  1, 11, "1\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 0,           12,  2, 11, "12\0----------------------------------------------------------------", STATUS_SUCCESS},
    { 0,          123,  3, 11, "123\0---------------------------------------------------------------", STATUS_SUCCESS},
    { 0,         1234,  4, 11, "1234\0--------------------------------------------------------------", STATUS_SUCCESS},
    { 0,        12345,  5, 11, "12345\0-------------------------------------------------------------", STATUS_SUCCESS},
    { 0,       123456,  6, 11, "123456\0------------------------------------------------------------", STATUS_SUCCESS},
    { 0,      1234567,  7, 11, "1234567\0-----------------------------------------------------------", STATUS_SUCCESS},
    { 0,     12345678,  8, 11, "12345678\0----------------------------------------------------------", STATUS_SUCCESS},
    { 0,    123456789,  9, 11, "123456789\0---------------------------------------------------------", STATUS_SUCCESS},
    { 0,   2147483646, 10, 11, "2147483646\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,   2147483647, 10, 11, "2147483647\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,  2147483648U, 10, 11, "2147483648\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,  2147483649U, 10, 11, "2147483649\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,  4294967294U, 10, 11, "4294967294\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,  4294967295U, 10, 11, "4294967295\0--------------------------------------------------------", STATUS_SUCCESS},
    { 0,       12345678901U, 11, 12, "12345678901\0-------------------------------------------------------", STATUS_SUCCESS},
    { 0,      987654321012U, 12, 13, "987654321012\0------------------------------------------------------", STATUS_SUCCESS},
    { 0,     1928374656574U, 13, 14, "1928374656574\0-----------------------------------------------------", STATUS_SUCCESS},
    { 0,      0xBADCAFEFACE, 14, 15, "12841062955726\0----------------------------------------------------", STATUS_SUCCESS},
    { 0,     0x5BADCAFEFACE, 15, 16, "100801993177806\0---------------------------------------------------", STATUS_SUCCESS},
    { 0,    0xAFACEBEEFCAFE, 16, 20, "3090515640699646\0--------------------------------------------------", STATUS_SUCCESS},
    { 0,   0xA5BEEFABCDCAFE, 17, 20, "46653307746110206\0-------------------------------------------------", STATUS_SUCCESS},
    { 0,  0x1F8CF9BF2DF3AF1, 18, 20, "142091656963767025\0------------------------------------------------", STATUS_SUCCESS},
    { 0,  0xFFFFFFFFFFFFFFF, 19, 20, "1152921504606846975\0-----------------------------------------------", STATUS_SUCCESS},
    { 0, 0xFFFFFFFFFFFFFFFE, 20, 21, "18446744073709551614\0----------------------------------------------", STATUS_SUCCESS},
    { 0, 0xFFFFFFFFFFFFFFFF, 20, 21, "18446744073709551615\0----------------------------------------------", STATUS_SUCCESS},

    { 2,  0x80000000U, 32, 33, "10000000000000000000000000000000\0----------------------------------", STATUS_SUCCESS},
/*
 *  { 2,  -2147483647, 64, 65, "1111111111111111111111111111111110000000000000000000000000000001\0--", STATUS_SUCCESS},
 *  { 2,           -2, 64, 65, "1111111111111111111111111111111111111111111111111111111111111110\0--", STATUS_SUCCESS},
 *  { 2,           -1, 64, 65, "1111111111111111111111111111111111111111111111111111111111111111\0--", STATUS_SUCCESS},
 */
    { 2,            0,  1, 33, "0\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 2,            1,  1, 33, "1\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 2,           10,  4, 33, "1010\0--------------------------------------------------------------", STATUS_SUCCESS},
    { 2,          100,  7, 33, "1100100\0-----------------------------------------------------------", STATUS_SUCCESS},
    { 2,         1000, 10, 33, "1111101000\0--------------------------------------------------------", STATUS_SUCCESS},
    { 2,        10000, 14, 33, "10011100010000\0----------------------------------------------------", STATUS_SUCCESS},
    { 2,        32767, 15, 33, "111111111111111\0---------------------------------------------------", STATUS_SUCCESS},
    { 2,        32768, 16, 33, "1000000000000000\0--------------------------------------------------", STATUS_SUCCESS},
    { 2,        65535, 16, 33, "1111111111111111\0--------------------------------------------------", STATUS_SUCCESS},
    { 2,       100000, 17, 33, "11000011010100000\0-------------------------------------------------", STATUS_SUCCESS},
    { 2,      1000000, 20, 33, "11110100001001000000\0----------------------------------------------", STATUS_SUCCESS},
    { 2,     10000000, 24, 33, "100110001001011010000000\0------------------------------------------", STATUS_SUCCESS},
    { 2,    100000000, 27, 33, "101111101011110000100000000\0---------------------------------------", STATUS_SUCCESS},
    { 2,   1000000000, 30, 33, "111011100110101100101000000000\0------------------------------------", STATUS_SUCCESS},
    { 2,   1073741823, 30, 33, "111111111111111111111111111111\0------------------------------------", STATUS_SUCCESS},
    { 2,   2147483646, 31, 33, "1111111111111111111111111111110\0-----------------------------------", STATUS_SUCCESS},
    { 2,   2147483647, 31, 33, "1111111111111111111111111111111\0-----------------------------------", STATUS_SUCCESS},
    { 2,  2147483648U, 32, 33, "10000000000000000000000000000000\0----------------------------------", STATUS_SUCCESS},
    { 2,  2147483649U, 32, 33, "10000000000000000000000000000001\0----------------------------------", STATUS_SUCCESS},
    { 2,  4294967294U, 32, 33, "11111111111111111111111111111110\0----------------------------------", STATUS_SUCCESS},
    { 2,   0xFFFFFFFF, 32, 33, "11111111111111111111111111111111\0----------------------------------", STATUS_SUCCESS},
/*
 *  { 2,  0x1FFFFFFFF, 33, 34, "111111111111111111111111111111111\0---------------------------------", STATUS_SUCCESS},
 *  { 2,  0x3FFFFFFFF, 34, 35, "1111111111111111111111111111111111\0--------------------------------", STATUS_SUCCESS},
 *  { 2,  0x7FFFFFFFF, 35, 36, "11111111111111111111111111111111111\0-------------------------------", STATUS_SUCCESS},
 *  { 2,  0xFFFFFFFFF, 36, 37, "111111111111111111111111111111111111\0------------------------------", STATUS_SUCCESS},
 *  { 2, 0x1FFFFFFFFF, 37, 38, "1111111111111111111111111111111111111\0-----------------------------", STATUS_SUCCESS},
 *  { 2, 0x3FFFFFFFFF, 38, 39, "11111111111111111111111111111111111111\0----------------------------", STATUS_SUCCESS},
 *  { 2, 0x7FFFFFFFFF, 39, 40, "111111111111111111111111111111111111111\0---------------------------", STATUS_SUCCESS},
 *  { 2, 0xFFFFFFFFFF, 40, 41, "1111111111111111111111111111111111111111\0--------------------------", STATUS_SUCCESS},
 */

    { 8,  0x80000000U, 11, 12, "20000000000\0-------------------------------------------------------", STATUS_SUCCESS},
/*
 *  { 8,  -2147483647, 22, 23, "1777777777760000000001\0--------------------------------------------", STATUS_SUCCESS},
 *  { 8,           -2, 22, 23, "1777777777777777777776\0--------------------------------------------", STATUS_SUCCESS},
 *  { 8,           -1, 22, 23, "1777777777777777777777\0--------------------------------------------", STATUS_SUCCESS},
 */
    { 8,            0,  1, 12, "0\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 8,            1,  1, 12, "1\0-----------------------------------------------------------------", STATUS_SUCCESS},
    { 8,   2147483646, 11, 12, "17777777776\0-------------------------------------------------------", STATUS_SUCCESS},
    { 8,   2147483647, 11, 12, "17777777777\0-------------------------------------------------------", STATUS_SUCCESS},
    { 8,  2147483648U, 11, 12, "20000000000\0-------------------------------------------------------", STATUS_SUCCESS},
    { 8,  2147483649U, 11, 12, "20000000001\0-------------------------------------------------------", STATUS_SUCCESS},
    { 8,  4294967294U, 11, 12, "37777777776\0-------------------------------------------------------", STATUS_SUCCESS},
    { 8,  4294967295U, 11, 12, "37777777777\0-------------------------------------------------------", STATUS_SUCCESS},

    {10,  0x80000000U, 10, 11, "2147483648\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,  -2147483647, 20, 21, "18446744071562067969\0----------------------------------------------", STATUS_SUCCESS},
    {10,           -2, 20, 21, "18446744073709551614\0----------------------------------------------", STATUS_SUCCESS},
    {10,           -1, 20, 21, "18446744073709551615\0----------------------------------------------", STATUS_SUCCESS},
    {10,            0,  1, 11, "0\0-----------------------------------------------------------------", STATUS_SUCCESS},
    {10,            1,  1, 11, "1\0-----------------------------------------------------------------", STATUS_SUCCESS},
    {10,   2147483646, 10, 11, "2147483646\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,   2147483647, 10, 11, "2147483647\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,  2147483648U, 10, 11, "2147483648\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,  2147483649U, 10, 11, "2147483649\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,  4294967294U, 10, 11, "4294967294\0--------------------------------------------------------", STATUS_SUCCESS},
    {10,  4294967295U, 10, 11, "4294967295\0--------------------------------------------------------", STATUS_SUCCESS},

    {16,                  0,  1,  9, "0\0-----------------------------------------------------------------", STATUS_SUCCESS},
    {16,                  1,  1,  9, "1\0-----------------------------------------------------------------", STATUS_SUCCESS},
    {16,         2147483646,  8,  9, "7FFFFFFE\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,         2147483647,  8,  9, "7FFFFFFF\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,         0x80000000,  8,  9, "80000000\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,         0x80000001,  8,  9, "80000001\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,         0xFFFFFFFE,  8,  9, "FFFFFFFE\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,         0xFFFFFFFF,  8,  9, "FFFFFFFF\0----------------------------------------------------------", STATUS_SUCCESS},
/*
 *  {16,        0x100000000,  9, 10, "100000000\0---------------------------------------------------------", STATUS_SUCCESS},
 *  {16,      0xBADDEADBEEF, 11, 12, "BADDEADBEEF\0-------------------------------------------------------", STATUS_SUCCESS},
 *  {16, 0x8000000000000000, 16, 17, "8000000000000000\0--------------------------------------------------", STATUS_SUCCESS},
 *  {16, 0xFEDCBA9876543210, 16, 17, "FEDCBA9876543210\0--------------------------------------------------", STATUS_SUCCESS},
 *  {16, 0xFFFFFFFF80000001, 16, 17, "FFFFFFFF80000001\0--------------------------------------------------", STATUS_SUCCESS},
 *  {16, 0xFFFFFFFFFFFFFFFE, 16, 17, "FFFFFFFFFFFFFFFE\0--------------------------------------------------", STATUS_SUCCESS},
 *  {16, 0xFFFFFFFFFFFFFFFF, 16, 17, "FFFFFFFFFFFFFFFF\0--------------------------------------------------", STATUS_SUCCESS},
 */

    { 2,        32768, 16, 17, "1000000000000000\0--------------------------------------------------", STATUS_SUCCESS},
    { 2,        32768, 16, 16, "1000000000000000---------------------------------------------------",  STATUS_SUCCESS},
    { 2,        65536, 17, 18, "10000000000000000\0-------------------------------------------------", STATUS_SUCCESS},
    { 2,        65536, 17, 17, "10000000000000000--------------------------------------------------",  STATUS_SUCCESS},
    { 2,       131072, 18, 19, "100000000000000000\0------------------------------------------------", STATUS_SUCCESS},
    { 2,       131072, 18, 18, "100000000000000000-------------------------------------------------",  STATUS_SUCCESS},
    {16,   0xffffffff,  8,  9, "FFFFFFFF\0----------------------------------------------------------", STATUS_SUCCESS},
    {16,   0xffffffff,  8,  8, "FFFFFFFF-----------------------------------------------------------",  STATUS_SUCCESS},
    {16,   0xffffffff,  8,  7, "-------------------------------------------------------------------",  STATUS_BUFFER_OVERFLOW},
    {16,          0xa,  1,  2, "A\0-----------------------------------------------------------------", STATUS_SUCCESS},
    {16,          0xa,  1,  1, "A------------------------------------------------------------------",  STATUS_SUCCESS},
    {16,            0,  1,  0, "-------------------------------------------------------------------",  STATUS_BUFFER_OVERFLOW},
    {20,   0xdeadbeef,  0,  9, "-------------------------------------------------------------------",  STATUS_INVALID_PARAMETER},
    {-8,     07654321,  0, 12, "-------------------------------------------------------------------",  STATUS_INVALID_PARAMETER},
};
#define NB_LARGEINT2STR (sizeof(largeint2str)/sizeof(*largeint2str))


static void one_RtlInt64ToUnicodeString_test(int test_num, const largeint2str_t *largeint2str)
{
    int pos;
    WCHAR expected_str_Buffer[LARGE_STRI_BUFFER_LENGTH + 1];
    UNICODE_STRING expected_unicode_string;
    STRING expected_ansi_str;
    WCHAR str_Buffer[LARGE_STRI_BUFFER_LENGTH + 1];
    UNICODE_STRING unicode_string;
    STRING ansi_str;
    NTSTATUS result;

    for (pos = 0; pos < LARGE_STRI_BUFFER_LENGTH; pos++) {
	expected_str_Buffer[pos] = largeint2str->Buffer[pos];
    } /* for */
    expected_unicode_string.Length = largeint2str->Length * sizeof(WCHAR);
    expected_unicode_string.MaximumLength = largeint2str->MaximumLength * sizeof(WCHAR);
    expected_unicode_string.Buffer = expected_str_Buffer;
    pRtlUnicodeStringToAnsiString(&expected_ansi_str, &expected_unicode_string, 1);

    for (pos = 0; pos < LARGE_STRI_BUFFER_LENGTH; pos++) {
	str_Buffer[pos] = '-';
    } /* for */
    unicode_string.Length = 0;
    unicode_string.MaximumLength = largeint2str->MaximumLength * sizeof(WCHAR);
    unicode_string.Buffer = str_Buffer;

    if (largeint2str->base == 0) {
	result = pRtlInt64ToUnicodeString(largeint2str->value, 10, &unicode_string);
    } else {
	result = pRtlInt64ToUnicodeString(largeint2str->value, largeint2str->base, &unicode_string);
    } /* if */
    pRtlUnicodeStringToAnsiString(&ansi_str, &unicode_string, 1);
    if (result == STATUS_BUFFER_OVERFLOW) {
	/* On BUFFER_OVERFLOW the string Buffer should be unchanged */
	for (pos = 0; pos < LARGE_STRI_BUFFER_LENGTH; pos++) {
	    expected_str_Buffer[pos] = '-';
	} /* for */
	/* w2k: The native function has two reasons for BUFFER_OVERFLOW: */
	/* If the value is too large to convert: The Length is unchanged */
	/* If str is too small to hold the string: Set str->Length to the length */
	/* the string would have (which can be larger than the MaximumLength). */
	/* To allow all this in the tests we do the following: */
	if (expected_unicode_string.Length >= 64) {
	    /* The value is too large to convert only triggerd when testing native */
	    /* Length is not filled with the expected string length (garbage?) */
	    expected_unicode_string.Length = unicode_string.Length;
	} /* if */
    } else {
	ok(result == largeint2str->result,
	   "(test %d): RtlInt64ToUnicodeString(%llu, %d, [out]) has result %lx, expected: %lx",
	   test_num, largeint2str->value, largeint2str->base, result, largeint2str->result);
	if (result == STATUS_SUCCESS) {
	    ok(unicode_string.Buffer[unicode_string.Length/sizeof(WCHAR)] == '\0',
	       "(test %d): RtlInt64ToUnicodeString(%llu, %d, [out]) string \"%s\" is not NULL terminated",
	       test_num, largeint2str->value, largeint2str->base, ansi_str.Buffer);
	} /* if */
    } /* if */
    ok(memcmp(unicode_string.Buffer, expected_unicode_string.Buffer, LARGE_STRI_BUFFER_LENGTH * sizeof(WCHAR)) == 0,
       "(test %d): RtlInt64ToUnicodeString(%llu, %d, [out]) assigns string \"%s\", expected: \"%s\"",
       test_num, largeint2str->value, largeint2str->base, ansi_str.Buffer, expected_ansi_str.Buffer);
    ok(unicode_string.Length == expected_unicode_string.Length,
       "(test %d): RtlInt64ToUnicodeString(%llu, %d, [out]) string has Length %d, expected: %d",
       test_num, largeint2str->value, largeint2str->base, unicode_string.Length, expected_unicode_string.Length);
    ok(unicode_string.MaximumLength == expected_unicode_string.MaximumLength,
       "(test %d): RtlInt64ToUnicodeString(%llu, %d, [out]) string has MaximumLength %d, expected: %d",
       test_num, largeint2str->value, largeint2str->base, unicode_string.MaximumLength, expected_unicode_string.MaximumLength);
    pRtlFreeAnsiString(&expected_ansi_str);
    pRtlFreeAnsiString(&ansi_str);
}


static void test_RtlInt64ToUnicodeString(void)
{
    int test_num;

    for (test_num = 0; test_num < NB_LARGEINT2STR; test_num++) {
	one_RtlInt64ToUnicodeString_test(test_num, &largeint2str[test_num]);
    } /* for */
}


static void one_RtlLargeIntegerToChar_test(int test_num, const largeint2str_t *largeint2str)
{
    NTSTATUS result;
    char dest_str[LARGE_STRI_BUFFER_LENGTH + 1];
    ULONGLONG value;

    memset(dest_str, '-', LARGE_STRI_BUFFER_LENGTH);
    dest_str[LARGE_STRI_BUFFER_LENGTH] = '\0';
    value = largeint2str->value;
    if (largeint2str->base == 0) {
	result = pRtlLargeIntegerToChar(&value, 10, largeint2str->MaximumLength, dest_str);
    } else {
	result = pRtlLargeIntegerToChar(&value, largeint2str->base, largeint2str->MaximumLength, dest_str);
    } /* if */
    ok(result == largeint2str->result,
       "(test %d): RtlLargeIntegerToChar(%llu, %d, %d, [out]) has result %lx, expected: %lx",
       test_num, largeint2str->value, largeint2str->base, largeint2str->MaximumLength, result, largeint2str->result);
    ok(memcmp(dest_str, largeint2str->Buffer, LARGE_STRI_BUFFER_LENGTH) == 0,
       "(test %d): RtlLargeIntegerToChar(%llu, %d, %d, [out]) assigns string \"%s\", expected: \"%s\"",
       test_num, largeint2str->value, largeint2str->base, largeint2str->MaximumLength, dest_str, largeint2str->Buffer);
}


static void test_RtlLargeIntegerToChar(void)
{
    NTSTATUS result;
    int test_num;
    ULONGLONG value;

    for (test_num = 0; test_num < NB_LARGEINT2STR; test_num++) {
	one_RtlLargeIntegerToChar_test(test_num, &largeint2str[test_num]);
    } /* for */

    value = largeint2str[0].value;
    result = pRtlLargeIntegerToChar(&value, 20, largeint2str[0].MaximumLength, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "(test a): RtlLargeIntegerToChar(%llu, %d, %d, NULL) has result %lx, expected: %x",
       largeint2str[0].value, 20, largeint2str[0].MaximumLength, result, STATUS_INVALID_PARAMETER);

    result = pRtlLargeIntegerToChar(&value, 20, 0, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "(test b): RtlLargeIntegerToChar(%llu, %d, %d, NULL) has result %lx, expected: %x",
       largeint2str[0].value, 20, largeint2str[0].MaximumLength, result, STATUS_INVALID_PARAMETER);

    result = pRtlLargeIntegerToChar(&value, largeint2str[0].base, 0, NULL);
    ok(result == STATUS_BUFFER_OVERFLOW,
       "(test c): RtlLargeIntegerToChar(%llu, %d, %d, NULL) has result %lx, expected: %x",
       largeint2str[0].value, largeint2str[0].base, 0, result, STATUS_BUFFER_OVERFLOW);

    result = pRtlLargeIntegerToChar(&value, largeint2str[0].base, largeint2str[0].MaximumLength, NULL);
    ok(result == STATUS_ACCESS_VIOLATION,
       "(test d): RtlLargeIntegerToChar(%llu, %d, %d, NULL) has result %lx, expected: %x",
       largeint2str[0].value, largeint2str[0].base, largeint2str[0].MaximumLength, result, STATUS_ACCESS_VIOLATION);
}


START_TEST(large_int)
{
    InitFunctionPtrs();

    test_RtlExtendedMagicDivide();
    if (pRtlInt64ToUnicodeString != NULL) {
	test_RtlInt64ToUnicodeString();
    } /* if */
    test_RtlLargeIntegerToChar();
}
