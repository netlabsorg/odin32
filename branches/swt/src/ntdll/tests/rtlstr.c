/* Unit test suite for Rtl string functions
 *
 * Copyright 2002 Robert Shearman
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

static UNICODE_STRING uni;
static STRING str;

/* Function ptrs for ntdll calls */
static HMODULE hntdll = 0;
static NTSTATUS (WINAPI *pRtlAppendUnicodeStringToString)(UNICODE_STRING *, const UNICODE_STRING *);
static NTSTATUS (WINAPI *pRtlCharToInteger)(char *, ULONG, int *);
static VOID     (WINAPI *pRtlCopyString)(STRING *, const STRING *);
static BOOLEAN  (WINAPI *pRtlCreateUnicodeString)(PUNICODE_STRING, LPCWSTR);
static NTSTATUS (WINAPI *pRtlDowncaseUnicodeString)(UNICODE_STRING *, const UNICODE_STRING *, BOOLEAN);
static BOOLEAN  (WINAPI *pRtlEqualUnicodeString)(const UNICODE_STRING *, const UNICODE_STRING *, BOOLEAN);
static VOID     (WINAPI *pRtlFreeAnsiString)(PSTRING);
static VOID     (WINAPI *pRtlInitAnsiString)(PSTRING, LPCSTR);
static VOID     (WINAPI *pRtlInitString)(PSTRING, LPCSTR);
static VOID     (WINAPI *pRtlInitUnicodeString)(PUNICODE_STRING, LPCWSTR);
static NTSTATUS (WINAPI *pRtlIntegerToChar)(ULONG, ULONG, ULONG, PCHAR);
static NTSTATUS (WINAPI *pRtlIntegerToUnicodeString)(ULONG, ULONG, UNICODE_STRING *);
static NTSTATUS (WINAPI *pRtlUnicodeStringToAnsiString)(STRING *, const UNICODE_STRING *, BOOLEAN);
static NTSTATUS (WINAPI *pRtlUnicodeStringToInteger)(const UNICODE_STRING *, int, int *);
static WCHAR    (WINAPI *pRtlUpcaseUnicodeChar)(WCHAR);
static NTSTATUS (WINAPI *pRtlUpcaseUnicodeString)(UNICODE_STRING *, const UNICODE_STRING *, BOOLEAN);
static CHAR     (WINAPI *pRtlUpperChar)(CHAR);
static NTSTATUS (WINAPI *pRtlUpperString)(STRING *, const STRING *);

/*static VOID (WINAPI *pRtlFreeOemString)(PSTRING);*/
/*static BOOLEAN (WINAPI *pRtlCreateUnicodeStringFromAsciiz)(PUNICODE_STRING,LPCSTR);*/
/*static VOID (WINAPI *pRtlFreeUnicodeString)(PUNICODE_STRING);*/
/*static VOID (WINAPI *pRtlCopyUnicodeString)(UNICODE_STRING *, const UNICODE_STRING *);*/
/*static VOID (WINAPI *pRtlEraseUnicodeString)(UNICODE_STRING *);*/
/*static LONG (WINAPI *pRtlCompareString)(const STRING *,const STRING *,BOOLEAN);*/
/*static LONG (WINAPI *pRtlCompareUnicodeString)(const UNICODE_STRING *,const UNICODE_STRING *,BOOLEAN);*/
/*static BOOLEAN (WINAPI *pRtlEqualString)(const STRING *,const STRING *,BOOLEAN);*/
/*static BOOLEAN (WINAPI *pRtlPrefixString)(const STRING *, const STRING *, BOOLEAN);*/
/*static BOOLEAN (WINAPI *pRtlPrefixUnicodeString)(const UNICODE_STRING *, const UNICODE_STRING *, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlAnsiStringToUnicodeString)(PUNICODE_STRING, PCANSI_STRING, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlOemStringToUnicodeString)(PUNICODE_STRING, const STRING *, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlUnicodeStringToOemString)(STRING *, const UNICODE_STRING *, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlMultiByteToUnicodeN)(LPWSTR, DWORD, LPDWORD, LPCSTR, DWORD);*/
/*static NTSTATUS (WINAPI *pRtlOemToUnicodeN)(LPWSTR, DWORD, LPDWORD, LPCSTR, DWORD);*/
/*static NTSTATUS (WINAPI *pRtlUpcaseUnicodeStringToAnsiString)(STRING *, const UNICODE_STRING *, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlUpcaseUnicodeStringToOemString)(STRING *, const UNICODE_STRING *, BOOLEAN);*/
/*static NTSTATUS (WINAPI *pRtlUpcaseUnicodeToMultiByteN)(LPSTR, DWORD, LPDWORD, LPCWSTR, DWORD);*/
/*static NTSTATUS (WINAPI *pRtlUpcaseUnicodeToOemN)(LPSTR, DWORD, LPDWORD, LPCWSTR, DWORD);*/
/*static UINT (WINAPI *pRtlOemToUnicodeSize)(const STRING *);*/
/*static DWORD (WINAPI *pRtlAnsiStringToUnicodeSize)(const STRING *);*/
/*static DWORD (WINAPI *pRtlIsTextUnicode)(LPVOID, DWORD, DWORD *);*/


static WCHAR* AtoW( char* p )
{
    WCHAR* buffer;
    DWORD len = MultiByteToWideChar( CP_ACP, 0, p, -1, NULL, 0 );
    buffer = malloc( len * sizeof(WCHAR) );
    MultiByteToWideChar( CP_ACP, 0, p, -1, buffer, len );
    return buffer;
}


static void InitFunctionPtrs(void)
{
    hntdll = LoadLibraryA("ntdll.dll");
    ok(hntdll != 0, "LoadLibrary failed");
    if (hntdll) {
	pRtlAppendUnicodeStringToString = (void *)GetProcAddress(hntdll, "RtlAppendUnicodeStringToString");
	pRtlCharToInteger = (void *)GetProcAddress(hntdll, "RtlCharToInteger");
	pRtlCopyString = (void *)GetProcAddress(hntdll, "RtlCopyString");
	pRtlCreateUnicodeString = (void *)GetProcAddress(hntdll, "RtlCreateUnicodeString");
	pRtlDowncaseUnicodeString = (void *)GetProcAddress(hntdll, "RtlDowncaseUnicodeString");
	pRtlEqualUnicodeString = (void *)GetProcAddress(hntdll, "RtlEqualUnicodeString");
	pRtlFreeAnsiString = (void *)GetProcAddress(hntdll, "RtlFreeAnsiString");
	pRtlInitAnsiString = (void *)GetProcAddress(hntdll, "RtlInitAnsiString");
	pRtlInitString = (void *)GetProcAddress(hntdll, "RtlInitString");
	pRtlInitUnicodeString = (void *)GetProcAddress(hntdll, "RtlInitUnicodeString");
	pRtlIntegerToChar = (void *)GetProcAddress(hntdll, "RtlIntegerToChar");
	pRtlIntegerToUnicodeString = (void *)GetProcAddress(hntdll, "RtlIntegerToUnicodeString");
	pRtlUnicodeStringToAnsiString = (void *)GetProcAddress(hntdll, "RtlUnicodeStringToAnsiString");
	pRtlUnicodeStringToInteger = (void *)GetProcAddress(hntdll, "RtlUnicodeStringToInteger");
	pRtlUpcaseUnicodeChar = (void *)GetProcAddress(hntdll, "RtlUpcaseUnicodeChar");
	pRtlUpcaseUnicodeString = (void *)GetProcAddress(hntdll, "RtlUpcaseUnicodeString");
	pRtlUpperChar = (void *)GetProcAddress(hntdll, "RtlUpperChar");
	pRtlUpperString = (void *)GetProcAddress(hntdll, "RtlUpperString");
    } /* if */
}


static void test_RtlInitString(void)
{
	static const char teststring[] = "Some Wild String";
	str.Length = 0;
	str.MaximumLength = 0;
	str.Buffer = (void *)0xdeadbeef;
	pRtlInitString(&str, teststring);
	ok(str.Length == sizeof(teststring) - sizeof(char), "Length uninitialized");
	ok(str.MaximumLength == sizeof(teststring), "MaximumLength uninitialized");
	ok(str.Buffer == teststring, "Buffer not equal to teststring");
	ok(strcmp(str.Buffer, "Some Wild String") == 0, "Buffer written to");
	pRtlInitString(&str, NULL);
	ok(str.Length == 0, "Length uninitialized");
	ok(str.MaximumLength == 0, "MaximumLength uninitialized");
	ok(str.Buffer == NULL, "Buffer not equal to NULL");
/*	pRtlInitString(NULL, teststring); */
}


static void test_RtlInitUnicodeString(void)
{
#define STRINGW {'S','o','m','e',' ','W','i','l','d',' ','S','t','r','i','n','g',0}
    static const WCHAR teststring[] = STRINGW;
	static const WCHAR originalstring[] = STRINGW;
#undef STRINGW
	uni.Length = 0;
	uni.MaximumLength = 0;
	uni.Buffer = (void *)0xdeadbeef;
	pRtlInitUnicodeString(&uni, teststring);
	ok(uni.Length == sizeof(teststring) - sizeof(WCHAR), "Length uninitialized");
	ok(uni.MaximumLength == sizeof(teststring), "MaximumLength uninitialized");
	ok(uni.Buffer == teststring, "Buffer not equal to teststring");
	ok(lstrcmpW(uni.Buffer, originalstring) == 0, "Buffer written to");
	pRtlInitUnicodeString(&uni, NULL);
	ok(uni.Length == 0, "Length uninitialized");
	ok(uni.MaximumLength == 0, "MaximumLength uninitialized");
	ok(uni.Buffer == NULL, "Buffer not equal to NULL");
/*	pRtlInitUnicodeString(NULL, teststring); */
}


static void test_RtlCopyString(void)
{
	static const char teststring[] = "Some Wild String";
	static char deststring[] = "                    ";
	STRING deststr;
	pRtlInitString(&str, teststring);
	pRtlInitString(&deststr, deststring);
	pRtlCopyString(&deststr, &str);
	ok(strncmp(str.Buffer, deststring, str.Length) == 0, "String not copied");
}


static void test_RtlUpperChar(void)
{
    int ch;
    int upper_ch;
    int expected_upper_ch;
    int byte_ch;

    for (ch = -1; ch <= 1024; ch++) {
	upper_ch = pRtlUpperChar(ch);
	byte_ch = ch & 0xff;
	if (byte_ch >= 'a' && byte_ch <= 'z') {
	    expected_upper_ch = (CHAR) (byte_ch - 'a' + 'A');
	} else {
	    expected_upper_ch = (CHAR) byte_ch;
	} /* if */
	ok(upper_ch == expected_upper_ch,
	   "RtlUpperChar('%c'[=0x%x]) has result '%c'[=0x%x], expected '%c'[=0x%x]",
	   ch, ch, upper_ch, upper_ch, expected_upper_ch, expected_upper_ch);
    } /* for */
}


static void test_RtlUpperString(void)
{
    int i;
    CHAR ch;
    CHAR upper_ch;
    char ascii_buf[257];
    char result_buf[257];
    char upper_buf[257];
    STRING ascii_str;
    STRING result_str;
    STRING upper_str;

    for (i = 0; i <= 255; i++) {
	ch = (CHAR) i;
	if (ch >= 'a' && ch <= 'z') {
	    upper_ch = ch - 'a' + 'A';
	} else {
	    upper_ch = ch;
	}
	ascii_buf[i] = ch;
	result_buf[i] = '\0';
	upper_buf[i] = upper_ch;
    } /* for */
    ascii_buf[i] = '\0';
    result_buf[i] = '\0';
    upper_buf[i] = '\0';
    ascii_str.Length = 256;
    ascii_str.MaximumLength = 256;
    ascii_str.Buffer = ascii_buf;
    result_str.Length = 256;
    result_str.MaximumLength = 256;
    result_str.Buffer = result_buf;
    upper_str.Length = 256;
    upper_str.MaximumLength = 256;
    upper_str.Buffer = upper_buf;

    pRtlUpperString(&result_str, &ascii_str);
    ok(memcmp(result_str.Buffer, upper_str.Buffer, 256) == 0,
       "RtlUpperString does not work as expected");
}


static void test_RtlUpcaseUnicodeChar(void)
{
    int i;
    WCHAR ch;
    WCHAR upper_ch;
    WCHAR expected_upper_ch;

    for (i = 0; i <= 255; i++) {
	ch = (WCHAR) i;
	upper_ch = pRtlUpcaseUnicodeChar(ch);
	if (ch >= 'a' && ch <= 'z') {
	    expected_upper_ch = ch - 'a' + 'A';
	} else if (ch >= 0xe0 && ch <= 0xfe && ch != 0xf7) {
	    expected_upper_ch = ch - 0x20;
	} else if (ch == 0xff) {
	    expected_upper_ch = 0x178;
	} else {
	    expected_upper_ch = ch;
	} /* if */
	ok(upper_ch == expected_upper_ch,
	   "RtlUpcaseUnicodeChar('%c'[=0x%x]) has result '%c'[=0x%x], expected: '%c'[=0x%x]",
	   ch, ch, upper_ch, upper_ch, expected_upper_ch, expected_upper_ch);
    } /* for */
}


static void test_RtlUpcaseUnicodeString(void)
{
    int i;
    WCHAR ch;
    WCHAR upper_ch;
    WCHAR ascii_buf[257];
    WCHAR result_buf[257];
    WCHAR upper_buf[257];
    UNICODE_STRING ascii_str;
    UNICODE_STRING result_str;
    UNICODE_STRING upper_str;

    for (i = 0; i <= 255; i++) {
	ch = (WCHAR) i;
	if (ch >= 'a' && ch <= 'z') {
	    upper_ch = ch - 'a' + 'A';
	} else if (ch >= 0xe0 && ch <= 0xfe && ch != 0xf7) {
	    upper_ch = ch - 0x20;
	} else if (ch == 0xff) {
	    upper_ch = 0x178;
	} else {
	    upper_ch = ch;
	} /* if */
	ascii_buf[i] = ch;
	result_buf[i] = '\0';
	upper_buf[i] = upper_ch;
    } /* for */
    ascii_buf[i] = '\0';
    result_buf[i] = '\0';
    upper_buf[i] = '\0';
    ascii_str.Length = 512;
    ascii_str.MaximumLength = 512;
    ascii_str.Buffer = ascii_buf;
    result_str.Length = 512;
    result_str.MaximumLength = 512;
    result_str.Buffer = result_buf;
    upper_str.Length = 512;
    upper_str.MaximumLength = 512;
    upper_str.Buffer = upper_buf;

    pRtlUpcaseUnicodeString(&result_str, &ascii_str, 0);
    for (i = 0; i <= 255; i++) {
	ok(result_str.Buffer[i] == upper_str.Buffer[i],
	   "RtlUpcaseUnicodeString works wrong: '%c'[=0x%x] is converted to '%c'[=0x%x], expected: '%c'[=0x%x]",
	   ascii_str.Buffer[i], ascii_str.Buffer[i],
	   result_str.Buffer[i], result_str.Buffer[i],
	   upper_str.Buffer[i], upper_str.Buffer[i]);
    } /* for */
}


static void test_RtlDowncaseUnicodeString(void)
{
    int i;
    WCHAR ch;
    WCHAR lower_ch;
    WCHAR source_buf[1025];
    WCHAR result_buf[1025];
    WCHAR lower_buf[1025];
    UNICODE_STRING source_str;
    UNICODE_STRING result_str;
    UNICODE_STRING lower_str;

    for (i = 0; i <= 1024; i++) {
	ch = (WCHAR) i;
	if (ch >= 'A' && ch <= 'Z') {
	    lower_ch = ch - 'A' + 'a';
	} else if (ch >= 0xc0 && ch <= 0xde && ch != 0xd7) {
	    lower_ch = ch + 0x20;
	} else if (ch >= 0x391 && ch <= 0x3ab && ch != 0x3a2) {
	    lower_ch = ch + 0x20;
	} else {
	    switch (ch) {
		case 0x178: lower_ch = 0xff; break;
		case 0x181: lower_ch = 0x253; break;
		case 0x186: lower_ch = 0x254; break;
		case 0x189: lower_ch = 0x256; break;
		case 0x18a: lower_ch = 0x257; break;
		case 0x18e: lower_ch = 0x1dd; break;
		case 0x18f: lower_ch = 0x259; break;
		case 0x190: lower_ch = 0x25b; break;
		case 0x193: lower_ch = 0x260; break;
		case 0x194: lower_ch = 0x263; break;
		case 0x196: lower_ch = 0x269; break;
		case 0x197: lower_ch = 0x268; break;
		case 0x19c: lower_ch = 0x26f; break;
		case 0x19d: lower_ch = 0x272; break;
		case 0x19f: lower_ch = 0x275; break;
		case 0x1a9: lower_ch = 0x283; break;
		case 0x1ae: lower_ch = 0x288; break;
		case 0x1b1: lower_ch = 0x28a; break;
		case 0x1b2: lower_ch = 0x28b; break;
		case 0x1b7: lower_ch = 0x292; break;
		case 0x1c4: lower_ch = 0x1c6; break;
		case 0x1c7: lower_ch = 0x1c9; break;
		case 0x1ca: lower_ch = 0x1cc; break;
		case 0x1f1: lower_ch = 0x1f3; break;
		case 0x386: lower_ch = 0x3ac; break;
		case 0x388: lower_ch = 0x3ad; break;
		case 0x389: lower_ch = 0x3ae; break;
		case 0x38a: lower_ch = 0x3af; break;
		case 0x38c: lower_ch = 0x3cc; break;
		case 0x38e: lower_ch = 0x3cd; break;
		case 0x38f: lower_ch = 0x3ce; break;
		case 0x400: lower_ch = 0x0; break;
		default: lower_ch = ch; break;
	    } /* switch */
	} /* if */
	source_buf[i] = ch;
	result_buf[i] = '\0';
	lower_buf[i] = lower_ch;
    } /* for */
    source_buf[i] = '\0';
    result_buf[i] = '\0';
    lower_buf[i] = '\0';
    source_str.Length = 2048;
    source_str.MaximumLength = 2048;
    source_str.Buffer = source_buf;
    result_str.Length = 2048;
    result_str.MaximumLength = 2048;
    result_str.Buffer = result_buf;
    lower_str.Length = 2048;
    lower_str.MaximumLength = 2048;
    lower_str.Buffer = lower_buf;

    pRtlDowncaseUnicodeString(&result_str, &source_str, 0);
    for (i = 0; i <= 1024; i++) {
	ok(result_str.Buffer[i] == lower_str.Buffer[i] || result_str.Buffer[i] == source_str.Buffer[i] + 1,
	   "RtlDowncaseUnicodeString works wrong: '%c'[=0x%x] is converted to '%c'[=0x%x], expected: '%c'[=0x%x]",
	   source_str.Buffer[i], source_str.Buffer[i],
	   result_str.Buffer[i], result_str.Buffer[i],
	   lower_str.Buffer[i], lower_str.Buffer[i]);
    } /* for */
}


static void test_RtlAppendUnicodeStringToString(void)
{
    CHAR dest_buf[257];
    CHAR src_buf[257];
    UNICODE_STRING dest_str;
    UNICODE_STRING src_str;
    NTSTATUS result;

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    strcpy(src_buf, "nicodeStringZYXWVUTS");
    dest_str.Length = 12;
    dest_str.MaximumLength = 26;
    dest_str.Buffer = (WCHAR *) dest_buf;
    src_str.Length = 12;
    src_str.MaximumLength = 12;
    src_str.Buffer = (WCHAR *) src_buf;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeUnicodeString\0\0efghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 25;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeUnicodeString\0\0efghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 24;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeUnicodeStringcdefghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 23;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_BUFFER_TOO_SMALL,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeU0123456789abcdefghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 0;
    src_str.Length = 0;
    src_str.MaximumLength = 0;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeU0123456789abcdefghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 22;
    src_str.Length = 0;
    src_str.MaximumLength = 0;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeU0123456789abcdefghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);

    strcpy(dest_buf, "ThisisafakeU0123456789abcdefghij");
    dest_str.Length = 12;
    dest_str.MaximumLength = 22;
    src_str.Length = 0;
    src_str.MaximumLength = 0;
    src_str.Buffer = NULL;
    result = pRtlAppendUnicodeStringToString(&dest_str, &src_str);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has result %lx",
       result);
    ok(memcmp(dest_buf, "ThisisafakeU0123456789abcdefghij", 32) == 0,
       "call failed: RtlAppendUnicodeStringToString(dest, src) has dest \"%s\"",
       dest_buf);
}


typedef struct {
    int base;
    char *str;
    int value;
    NTSTATUS result;
} str2int_t;

static const str2int_t str2int[] = {
    { 0, "1011101100",   1011101100, STATUS_SUCCESS},
    { 0, "1234567",         1234567, STATUS_SUCCESS},
    { 0, "-214",               -214, STATUS_SUCCESS},
    { 0, "+214",                214, STATUS_SUCCESS}, /* The + sign is allowed also */
    { 0, "--214",                 0, STATUS_SUCCESS}, /* Do not accept more than one sign */
    { 0, "-+214",                 0, STATUS_SUCCESS},
    { 0, "++214",                 0, STATUS_SUCCESS},
    { 0, "+-214",                 0, STATUS_SUCCESS},
    { 0, "\001\002\003\00411",   11, STATUS_SUCCESS}, /* whitespace char  1 to  4 */
    { 0, "\005\006\007\01012",   12, STATUS_SUCCESS}, /* whitespace char  5 to  8 */
    { 0, "\011\012\013\01413",   13, STATUS_SUCCESS}, /* whitespace char  9 to 12 */
    { 0, "\015\016\017\02014",   14, STATUS_SUCCESS}, /* whitespace char 13 to 16 */
    { 0, "\021\022\023\02415",   15, STATUS_SUCCESS}, /* whitespace char 17 to 20 */
    { 0, "\025\026\027\03016",   16, STATUS_SUCCESS}, /* whitespace char 21 to 24 */
    { 0, "\031\032\033\03417",   17, STATUS_SUCCESS}, /* whitespace char 25 to 28 */
    { 0, "\035\036\037\04018",   18, STATUS_SUCCESS}, /* whitespace char 29 to 32 */
    { 0, " \n \r \t214",        214, STATUS_SUCCESS},
    { 0, " \n \r \t+214",       214, STATUS_SUCCESS}, /* Signs can be used after whitespace */
    { 0, " \n \r \t-214",      -214, STATUS_SUCCESS},
    { 0, "+214 0",              214, STATUS_SUCCESS}, /* Space terminates the number */
    { 0, " 214.01",             214, STATUS_SUCCESS}, /* Decimal point not accepted */
    { 0, " 214,01",             214, STATUS_SUCCESS}, /* Decimal comma not accepted */
    { 0, "f81",                   0, STATUS_SUCCESS},
    { 0, "0x12345",         0x12345, STATUS_SUCCESS}, /* Hex */
    { 0, "00x12345",              0, STATUS_SUCCESS},
    { 0, "0xx12345",              0, STATUS_SUCCESS},
    { 0, "1x34",                  1, STATUS_SUCCESS},
    { 0, "-9999999999", -1410065407, STATUS_SUCCESS}, /* Big negative integer */
    { 0, "-2147483649",  2147483647, STATUS_SUCCESS}, /* Too small to fit in 32 Bits */
    { 0, "-2147483648", 0x80000000L, STATUS_SUCCESS}, /* Smallest negative integer */
    { 0, "-2147483647", -2147483647, STATUS_SUCCESS},
    { 0, "-1",                   -1, STATUS_SUCCESS},
    { 0, "0",                     0, STATUS_SUCCESS},
    { 0, "1",                     1, STATUS_SUCCESS},
    { 0, "2147483646",   2147483646, STATUS_SUCCESS},
    { 0, "2147483647",   2147483647, STATUS_SUCCESS}, /* Largest signed positive integer */
    { 0, "2147483648",  0x80000000L, STATUS_SUCCESS}, /* Positive int equal to smallest negative int */
    { 0, "2147483649",  -2147483647, STATUS_SUCCESS},
    { 0, "4294967294",           -2, STATUS_SUCCESS},
    { 0, "4294967295",           -1, STATUS_SUCCESS}, /* Largest unsigned integer */
    { 0, "4294967296",            0, STATUS_SUCCESS}, /* Too big to fit in 32 Bits */
    { 0, "9999999999",   1410065407, STATUS_SUCCESS}, /* Big positive integer */
    { 0, "056789",            56789, STATUS_SUCCESS}, /* Leading zero and still decimal */
    { 0, "b1011101100",           0, STATUS_SUCCESS}, /* Binary (b-notation) */
    { 0, "-b1011101100",          0, STATUS_SUCCESS}, /* Negative Binary (b-notation) */
    { 0, "b10123456789",          0, STATUS_SUCCESS}, /* Binary with nonbinary digits (2-9) */
    { 0, "0b1011101100",        748, STATUS_SUCCESS}, /* Binary (0b-notation) */
    { 0, "-0b1011101100",      -748, STATUS_SUCCESS}, /* Negative binary (0b-notation) */
    { 0, "0b10123456789",         5, STATUS_SUCCESS}, /* Binary with nonbinary digits (2-9) */
    { 0, "-0b10123456789",       -5, STATUS_SUCCESS}, /* Negative binary with nonbinary digits (2-9) */
    { 0, "0b1",                   1, STATUS_SUCCESS}, /* one digit binary */
    { 0, "0b2",                   0, STATUS_SUCCESS}, /* empty binary */
    { 0, "0b",                    0, STATUS_SUCCESS}, /* empty binary */
    { 0, "o1234567",              0, STATUS_SUCCESS}, /* Octal (o-notation) */
    { 0, "-o1234567",             0, STATUS_SUCCESS}, /* Negative Octal (o-notation) */
    { 0, "o56789",                0, STATUS_SUCCESS}, /* Octal with nonoctal digits (8 and 9) */
    { 0, "0o1234567",      01234567, STATUS_SUCCESS}, /* Octal (0o-notation) */
    { 0, "-0o1234567",    -01234567, STATUS_SUCCESS}, /* Negative octal (0o-notation) */
    { 0, "0o56789",            0567, STATUS_SUCCESS}, /* Octal with nonoctal digits (8 and 9) */
    { 0, "-0o56789",          -0567, STATUS_SUCCESS}, /* Negative octal with nonoctal digits (8 and 9) */
    { 0, "0o7",                   7, STATUS_SUCCESS}, /* one digit octal */
    { 0, "0o8",                   0, STATUS_SUCCESS}, /* empty octal */
    { 0, "0o",                    0, STATUS_SUCCESS}, /* empty octal */
    { 0, "0d1011101100",          0, STATUS_SUCCESS}, /* explizit decimal with 0d */
    { 0, "x89abcdef",             0, STATUS_SUCCESS}, /* Hex with lower case digits a-f (x-notation) */
    { 0, "xFEDCBA00",             0, STATUS_SUCCESS}, /* Hex with upper case digits A-F (x-notation) */
    { 0, "-xFEDCBA00",            0, STATUS_SUCCESS}, /* Negative Hexadecimal (x-notation) */
    { 0, "0x89abcdef",   0x89abcdef, STATUS_SUCCESS}, /* Hex with lower case digits a-f (0x-notation) */
    { 0, "0xFEDCBA00",   0xFEDCBA00, STATUS_SUCCESS}, /* Hex with upper case digits A-F (0x-notation) */
    { 0, "-0xFEDCBA00", -0xFEDCBA00, STATUS_SUCCESS}, /* Negative Hexadecimal (0x-notation) */
    { 0, "0xabcdefgh",     0xabcdef, STATUS_SUCCESS}, /* Hex with illegal lower case digits (g-z) */
    { 0, "0xABCDEFGH",     0xABCDEF, STATUS_SUCCESS}, /* Hex with illegal upper case digits (G-Z) */
    { 0, "0xF",                 0xf, STATUS_SUCCESS}, /* one digit hexadecimal */
    { 0, "0xG",                   0, STATUS_SUCCESS}, /* empty hexadecimal */
    { 0, "0x",                    0, STATUS_SUCCESS}, /* empty hexadecimal */
    { 0, "",                      0, STATUS_SUCCESS}, /* empty string */
    { 2, "1011101100",          748, STATUS_SUCCESS},
    { 2, "-1011101100",        -748, STATUS_SUCCESS},
    { 2, "2",                     0, STATUS_SUCCESS},
    { 2, "0b1011101100",          0, STATUS_SUCCESS},
    { 2, "0o1011101100",          0, STATUS_SUCCESS},
    { 2, "0d1011101100",          0, STATUS_SUCCESS},
    { 2, "0x1011101100",          0, STATUS_SUCCESS},
    { 2, "",                      0, STATUS_SUCCESS}, /* empty string */
    { 8, "1011101100",    136610368, STATUS_SUCCESS},
    { 8, "-1011101100",  -136610368, STATUS_SUCCESS},
    { 8, "8",                     0, STATUS_SUCCESS},
    { 8, "0b1011101100",          0, STATUS_SUCCESS},
    { 8, "0o1011101100",          0, STATUS_SUCCESS},
    { 8, "0d1011101100",          0, STATUS_SUCCESS},
    { 8, "0x1011101100",          0, STATUS_SUCCESS},
    { 8, "",                      0, STATUS_SUCCESS}, /* empty string */
    {10, "1011101100",   1011101100, STATUS_SUCCESS},
    {10, "-1011101100", -1011101100, STATUS_SUCCESS},
    {10, "0b1011101100",          0, STATUS_SUCCESS},
    {10, "0o1011101100",          0, STATUS_SUCCESS},
    {10, "0d1011101100",          0, STATUS_SUCCESS},
    {10, "0x1011101100",          0, STATUS_SUCCESS},
    {10, "o12345",                0, STATUS_SUCCESS}, /* Octal altrough base is 10 */
    {10, "",                      0, STATUS_SUCCESS}, /* empty string */
    {16, "1011101100",    286265600, STATUS_SUCCESS},
    {16, "-1011101100",  -286265600, STATUS_SUCCESS},
    {16, "G",                     0, STATUS_SUCCESS},
    {16, "g",                     0, STATUS_SUCCESS},
    {16, "0b1011101100",  286265600, STATUS_SUCCESS},
    {16, "0o1011101100",          0, STATUS_SUCCESS},
    {16, "0d1011101100",  286265600, STATUS_SUCCESS},
    {16, "0x1011101100",          0, STATUS_SUCCESS},
    {16, "",                      0, STATUS_SUCCESS}, /* empty string */
    {20, "0",            0xdeadbeef, STATUS_INVALID_PARAMETER}, /* illegal base */
    {-8, "0",            0xdeadbeef, STATUS_INVALID_PARAMETER}, /* Negative base */
/*    { 0, NULL,                    0, STATUS_SUCCESS}, */ /* NULL as string */
};
#define NB_STR2INT (sizeof(str2int)/sizeof(*str2int))


static void test_RtlUnicodeStringToInteger(void)
{
    int test_num;
    int value;
    NTSTATUS result;
    WCHAR *wstr;

    for (test_num = 0; test_num < NB_STR2INT; test_num++) {
	wstr = AtoW(str2int[test_num].str);
	value = 0xdeadbeef;
	pRtlInitUnicodeString(&uni, wstr);
	result = pRtlUnicodeStringToInteger(&uni, str2int[test_num].base, &value);
	ok(result == str2int[test_num].result,
	   "(test %d): RtlUnicodeStringToInteger(\"%s\", %d, [out]) has result %lx, expected: %lx",
	   test_num, str2int[test_num].str, str2int[test_num].base, result, str2int[test_num].result);
	ok(value == str2int[test_num].value,
	   "(test %d): RtlUnicodeStringToInteger(\"%s\", %d, [out]) assigns value %d, expected: %d",
	   test_num, str2int[test_num].str, str2int[test_num].base, value, str2int[test_num].value);
	free(wstr);
    } /* for */

    wstr = AtoW(str2int[1].str);
    pRtlInitUnicodeString(&uni, wstr);
    result = pRtlUnicodeStringToInteger(&uni, str2int[1].base, NULL);
    ok(result == STATUS_ACCESS_VIOLATION,
       "call failed: RtlUnicodeStringToInteger(\"%s\", %d, NULL) has result %lx",
       str2int[1].str, str2int[1].base, result);
    result = pRtlUnicodeStringToInteger(&uni, 20, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "call failed: RtlUnicodeStringToInteger(\"%s\", 20, NULL) has result %lx",
       str2int[1].str, result);

    uni.Length = 10; /* Make Length shorter (5 WCHARS instead of 7) */
    result = pRtlUnicodeStringToInteger(&uni, str2int[1].base, &value);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlUnicodeStringToInteger(\"12345\", %d, [out]) has result %lx",
       str2int[1].base, result);
    ok(value == 12345,
       "didn't return expected value (test a): expected: %d, got: %d",
       12345, value);

    uni.Length = 5; /* Use odd Length (2.5 WCHARS) */
    result = pRtlUnicodeStringToInteger(&uni, str2int[1].base, &value);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlUnicodeStringToInteger(\"12\", %d, [out]) has result %lx",
       str2int[1].base, result);
    ok(value == 12,
       "didn't return expected value (test b): expected: %d, got: %d",
       12, value);

    uni.Length = 2;
    result = pRtlUnicodeStringToInteger(&uni, str2int[1].base, &value);
    ok(result == STATUS_SUCCESS,
       "call failed: RtlUnicodeStringToInteger(\"1\", %d, [out]) has result %lx",
       str2int[1].base, result);
    ok(value == 1,
       "didn't return expected value (test c): expected: %d, got: %d",
       1, value);
    /* w2k: uni.Length = 0 returns value 11234567 instead of 0 */
    free(wstr);
}


static void test_RtlCharToInteger(void)
{
    int test_num;
    int value;
    NTSTATUS result;

    for (test_num = 0; test_num < NB_STR2INT; test_num++) {
	/* w2k skips a leading '\0' and processes the string after */
	if (str2int[test_num].str[0] != '\0') {
	    value = 0xdeadbeef;
	    result = pRtlCharToInteger(str2int[test_num].str, str2int[test_num].base, &value);
	    ok(result == str2int[test_num].result,
	       "(test %d): call failed: RtlCharToInteger(\"%s\", %d, [out]) has result %lx, expected: %lx",
	       test_num, str2int[test_num].str, str2int[test_num].base, result, str2int[test_num].result);
	    ok(value == str2int[test_num].value,
	       "(test %d): call failed: RtlCharToInteger(\"%s\", %d, [out]) assigns value %d, expected: %d",
	       test_num, str2int[test_num].str, str2int[test_num].base, value, str2int[test_num].value);
	} /* if */
    } /* for */

    result = pRtlCharToInteger(str2int[1].str, str2int[1].base, NULL);
    ok(result == STATUS_ACCESS_VIOLATION,
       "call failed: RtlCharToInteger(\"%s\", %d, NULL) has result %lx",
       str2int[1].str, str2int[1].base, result);

    result = pRtlCharToInteger(str2int[1].str, 20, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "call failed: RtlCharToInteger(\"%s\", 20, NULL) has result %lx",
       str2int[1].str, result);
}


#define STRI_BUFFER_LENGTH 35

typedef struct {
    int base;
    ULONG value;
    USHORT Length;
    USHORT MaximumLength;
    char *Buffer;
    NTSTATUS result;
} int2str_t;

static const int2str_t int2str[] = {
    {10,          123,  3, 11, "123\0-------------------------------", STATUS_SUCCESS},

    { 0,  0x80000000U, 10, 11, "2147483648\0------------------------", STATUS_SUCCESS}, /* min signed int */
    { 0,  -2147483647, 10, 11, "2147483649\0------------------------", STATUS_SUCCESS},
    { 0,           -2, 10, 11, "4294967294\0------------------------", STATUS_SUCCESS},
    { 0,           -1, 10, 11, "4294967295\0------------------------", STATUS_SUCCESS},
    { 0,            0,  1, 11, "0\0---------------------------------", STATUS_SUCCESS},
    { 0,            1,  1, 11, "1\0---------------------------------", STATUS_SUCCESS},
    { 0,           12,  2, 11, "12\0--------------------------------", STATUS_SUCCESS},
    { 0,          123,  3, 11, "123\0-------------------------------", STATUS_SUCCESS},
    { 0,         1234,  4, 11, "1234\0------------------------------", STATUS_SUCCESS},
    { 0,        12345,  5, 11, "12345\0-----------------------------", STATUS_SUCCESS},
    { 0,       123456,  6, 11, "123456\0----------------------------", STATUS_SUCCESS},
    { 0,      1234567,  7, 11, "1234567\0---------------------------", STATUS_SUCCESS},
    { 0,     12345678,  8, 11, "12345678\0--------------------------", STATUS_SUCCESS},
    { 0,    123456789,  9, 11, "123456789\0-------------------------", STATUS_SUCCESS},
    { 0,   2147483646, 10, 11, "2147483646\0------------------------", STATUS_SUCCESS},
    { 0,   2147483647, 10, 11, "2147483647\0------------------------", STATUS_SUCCESS}, /* max signed int */
    { 0,  2147483648U, 10, 11, "2147483648\0------------------------", STATUS_SUCCESS}, /* uint = -max int */
    { 0,  2147483649U, 10, 11, "2147483649\0------------------------", STATUS_SUCCESS},
    { 0,  4294967294U, 10, 11, "4294967294\0------------------------", STATUS_SUCCESS},
    { 0,  4294967295U, 10, 11, "4294967295\0------------------------", STATUS_SUCCESS}, /* max unsigned int */

    { 2,  0x80000000U, 32, 33, "10000000000000000000000000000000\0--", STATUS_SUCCESS}, /* min signed int */
    { 2,  -2147483647, 32, 33, "10000000000000000000000000000001\0--", STATUS_SUCCESS},
    { 2,           -2, 32, 33, "11111111111111111111111111111110\0--", STATUS_SUCCESS},
    { 2,           -1, 32, 33, "11111111111111111111111111111111\0--", STATUS_SUCCESS},
    { 2,            0,  1, 33, "0\0---------------------------------", STATUS_SUCCESS},
    { 2,            1,  1, 33, "1\0---------------------------------", STATUS_SUCCESS},
    { 2,           10,  4, 33, "1010\0------------------------------", STATUS_SUCCESS},
    { 2,          100,  7, 33, "1100100\0---------------------------", STATUS_SUCCESS},
    { 2,         1000, 10, 33, "1111101000\0------------------------", STATUS_SUCCESS},
    { 2,        10000, 14, 33, "10011100010000\0--------------------", STATUS_SUCCESS},
    { 2,        32767, 15, 33, "111111111111111\0-------------------", STATUS_SUCCESS},
    { 2,        32768, 16, 33, "1000000000000000\0------------------", STATUS_SUCCESS},
    { 2,        65535, 16, 33, "1111111111111111\0------------------", STATUS_SUCCESS},
    { 2,        65536, 17, 33, "10000000000000000\0-----------------", STATUS_SUCCESS},
    { 2,       100000, 17, 33, "11000011010100000\0-----------------", STATUS_SUCCESS},
    { 2,      1000000, 20, 33, "11110100001001000000\0--------------", STATUS_SUCCESS},
    { 2,     10000000, 24, 33, "100110001001011010000000\0----------", STATUS_SUCCESS},
    { 2,    100000000, 27, 33, "101111101011110000100000000\0-------", STATUS_SUCCESS},
    { 2,   1000000000, 30, 33, "111011100110101100101000000000\0----", STATUS_SUCCESS},
    { 2,   1073741823, 30, 33, "111111111111111111111111111111\0----", STATUS_SUCCESS},
    { 2,   2147483646, 31, 33, "1111111111111111111111111111110\0---", STATUS_SUCCESS},
    { 2,   2147483647, 31, 33, "1111111111111111111111111111111\0---", STATUS_SUCCESS}, /* max signed int */
    { 2,  2147483648U, 32, 33, "10000000000000000000000000000000\0--", STATUS_SUCCESS}, /* uint = -max int */
    { 2,  2147483649U, 32, 33, "10000000000000000000000000000001\0--", STATUS_SUCCESS},
    { 2,  4294967294U, 32, 33, "11111111111111111111111111111110\0--", STATUS_SUCCESS},
    { 2,  4294967295U, 32, 33, "11111111111111111111111111111111\0--", STATUS_SUCCESS}, /* max unsigned int */

    { 8,  0x80000000U, 11, 12, "20000000000\0-----------------------", STATUS_SUCCESS}, /* min signed int */
    { 8,  -2147483647, 11, 12, "20000000001\0-----------------------", STATUS_SUCCESS},
    { 8,           -2, 11, 12, "37777777776\0-----------------------", STATUS_SUCCESS},
    { 8,           -1, 11, 12, "37777777777\0-----------------------", STATUS_SUCCESS},
    { 8,            0,  1, 12, "0\0---------------------------------", STATUS_SUCCESS},
    { 8,            1,  1, 12, "1\0---------------------------------", STATUS_SUCCESS},
    { 8,   2147483646, 11, 12, "17777777776\0-----------------------", STATUS_SUCCESS},
    { 8,   2147483647, 11, 12, "17777777777\0-----------------------", STATUS_SUCCESS}, /* max signed int */
    { 8,  2147483648U, 11, 12, "20000000000\0-----------------------", STATUS_SUCCESS}, /* uint = -max int */
    { 8,  2147483649U, 11, 12, "20000000001\0-----------------------", STATUS_SUCCESS},
    { 8,  4294967294U, 11, 12, "37777777776\0-----------------------", STATUS_SUCCESS},
    { 8,  4294967295U, 11, 12, "37777777777\0-----------------------", STATUS_SUCCESS}, /* max unsigned int */

    {10,  0x80000000U, 10, 11, "2147483648\0------------------------", STATUS_SUCCESS}, /* min signed int */
    {10,  -2147483647, 10, 11, "2147483649\0------------------------", STATUS_SUCCESS},
    {10,           -2, 10, 11, "4294967294\0------------------------", STATUS_SUCCESS},
    {10,           -1, 10, 11, "4294967295\0------------------------", STATUS_SUCCESS},
    {10,            0,  1, 11, "0\0---------------------------------", STATUS_SUCCESS},
    {10,            1,  1, 11, "1\0---------------------------------", STATUS_SUCCESS},
    {10,   2147483646, 10, 11, "2147483646\0------------------------", STATUS_SUCCESS},
    {10,   2147483647, 10, 11, "2147483647\0------------------------", STATUS_SUCCESS}, /* max signed int */
    {10,  2147483648U, 10, 11, "2147483648\0------------------------", STATUS_SUCCESS}, /* uint = -max int */
    {10,  2147483649U, 10, 11, "2147483649\0------------------------", STATUS_SUCCESS},
    {10,  4294967294U, 10, 11, "4294967294\0------------------------", STATUS_SUCCESS},
    {10,  4294967295U, 10, 11, "4294967295\0------------------------", STATUS_SUCCESS}, /* max unsigned int */

    {16,  0x80000000U,  8,  9, "80000000\0--------------------------", STATUS_SUCCESS}, /* min signed int */
    {16,  -2147483647,  8,  9, "80000001\0--------------------------", STATUS_SUCCESS},
    {16,           -2,  8,  9, "FFFFFFFE\0--------------------------", STATUS_SUCCESS},
    {16,           -1,  8,  9, "FFFFFFFF\0--------------------------", STATUS_SUCCESS},
    {16,            0,  1,  9, "0\0---------------------------------", STATUS_SUCCESS},
    {16,            1,  1,  9, "1\0---------------------------------", STATUS_SUCCESS},
    {16,   2147483646,  8,  9, "7FFFFFFE\0--------------------------", STATUS_SUCCESS},
    {16,   2147483647,  8,  9, "7FFFFFFF\0--------------------------", STATUS_SUCCESS}, /* max signed int */
    {16,  2147483648U,  8,  9, "80000000\0--------------------------", STATUS_SUCCESS}, /* uint = -max int */
    {16,  2147483649U,  8,  9, "80000001\0--------------------------", STATUS_SUCCESS},
    {16,  4294967294U,  8,  9, "FFFFFFFE\0--------------------------", STATUS_SUCCESS},
    {16,  4294967295U,  8,  9, "FFFFFFFF\0--------------------------", STATUS_SUCCESS}, /* max unsigned int */

    { 2,        32768, 16, 17, "1000000000000000\0------------------", STATUS_SUCCESS},
    { 2,        32768, 16, 16, "1000000000000000-------------------",  STATUS_SUCCESS},
    { 2,        65536, 17, 18, "10000000000000000\0-----------------", STATUS_SUCCESS},
    { 2,        65536, 17, 17, "10000000000000000------------------",  STATUS_SUCCESS},
    { 2,       131072, 18, 19, "100000000000000000\0----------------", STATUS_SUCCESS},
    { 2,       131072, 18, 18, "100000000000000000-----------------",  STATUS_SUCCESS},
    {16,   0xffffffff,  8,  9, "FFFFFFFF\0--------------------------", STATUS_SUCCESS},
    {16,   0xffffffff,  8,  8, "FFFFFFFF---------------------------",  STATUS_SUCCESS}, /* No \0 term */
    {16,   0xffffffff,  8,  7, "-----------------------------------",  STATUS_BUFFER_OVERFLOW}, /* Too short */
    {16,          0xa,  1,  2, "A\0---------------------------------", STATUS_SUCCESS},
    {16,          0xa,  1,  1, "A----------------------------------",  STATUS_SUCCESS}, /* No \0 term */
    {16,            0,  1,  0, "-----------------------------------",  STATUS_BUFFER_OVERFLOW},
    {20,   0xdeadbeef,  0,  9, "-----------------------------------",  STATUS_INVALID_PARAMETER}, /* ill. base */
    {-8,     07654321,  0, 12, "-----------------------------------",  STATUS_INVALID_PARAMETER}, /* neg. base */
};
#define NB_INT2STR (sizeof(int2str)/sizeof(*int2str))


static void one_RtlIntegerToUnicodeString_test(int test_num, const int2str_t *int2str)
{
    int pos;
    WCHAR expected_str_Buffer[STRI_BUFFER_LENGTH + 1];
    UNICODE_STRING expected_unicode_string;
    STRING expected_ansi_str;
    WCHAR str_Buffer[STRI_BUFFER_LENGTH + 1];
    UNICODE_STRING unicode_string;
    STRING ansi_str;
    NTSTATUS result;

    for (pos = 0; pos < STRI_BUFFER_LENGTH; pos++) {
	expected_str_Buffer[pos] = int2str->Buffer[pos];
    } /* for */
    expected_unicode_string.Length = int2str->Length * sizeof(WCHAR);
    expected_unicode_string.MaximumLength = int2str->MaximumLength * sizeof(WCHAR);
    expected_unicode_string.Buffer = expected_str_Buffer;
    pRtlUnicodeStringToAnsiString(&expected_ansi_str, &expected_unicode_string, 1);

    for (pos = 0; pos < STRI_BUFFER_LENGTH; pos++) {
	str_Buffer[pos] = '-';
    } /* for */
    unicode_string.Length = 0;
    unicode_string.MaximumLength = int2str->MaximumLength * sizeof(WCHAR);
    unicode_string.Buffer = str_Buffer;

    result = pRtlIntegerToUnicodeString(int2str->value, int2str->base, &unicode_string);
    pRtlUnicodeStringToAnsiString(&ansi_str, &unicode_string, 1);
    if (result == STATUS_BUFFER_OVERFLOW) {
	/* On BUFFER_OVERFLOW the string Buffer should be unchanged */
	for (pos = 0; pos < STRI_BUFFER_LENGTH; pos++) {
	    expected_str_Buffer[pos] = '-';
	} /* for */
	/* w2k: The native function has two reasons for BUFFER_OVERFLOW: */
	/* If the value is too large to convert: The Length is unchanged */
	/* If str is too small to hold the string: Set str->Length to the length */
	/* the string would have (which can be larger than the MaximumLength). */
	/* To allow all this in the tests we do the following: */
	if (expected_unicode_string.Length > 32 && unicode_string.Length == 0) {
	    /* The value is too large to convert only triggerd when testing native */
	    expected_unicode_string.Length = 0;
	} /* if */
    } else {
	ok(result == int2str->result,
	   "(test %d): RtlIntegerToUnicodeString(%lu, %d, [out]) has result %lx, expected: %lx",
	   test_num, int2str->value, int2str->base, result, int2str->result);
	if (result == STATUS_SUCCESS) {
	    ok(unicode_string.Buffer[unicode_string.Length/sizeof(WCHAR)] == '\0',
	       "(test %d): RtlIntegerToUnicodeString(%lu, %d, [out]) string \"%s\" is not NULL terminated",
	       test_num, int2str->value, int2str->base, ansi_str.Buffer);
	} /* if */
    } /* if */
    ok(memcmp(unicode_string.Buffer, expected_unicode_string.Buffer, STRI_BUFFER_LENGTH * sizeof(WCHAR)) == 0,
       "(test %d): RtlIntegerToUnicodeString(%lu, %d, [out]) assigns string \"%s\", expected: \"%s\"",
       test_num, int2str->value, int2str->base, ansi_str.Buffer, expected_ansi_str.Buffer);
    ok(unicode_string.Length == expected_unicode_string.Length,
       "(test %d): RtlIntegerToUnicodeString(%lu, %d, [out]) string has Length %d, expected: %d",
       test_num, int2str->value, int2str->base, unicode_string.Length, expected_unicode_string.Length);
    ok(unicode_string.MaximumLength == expected_unicode_string.MaximumLength,
       "(test %d): RtlIntegerToUnicodeString(%lu, %d, [out]) string has MaximumLength %d, expected: %d",
       test_num, int2str->value, int2str->base, unicode_string.MaximumLength, expected_unicode_string.MaximumLength);
    pRtlFreeAnsiString(&expected_ansi_str);
    pRtlFreeAnsiString(&ansi_str);
}


static void test_RtlIntegerToUnicodeString(void)
{
    int test_num;

    for (test_num = 0; test_num < NB_INT2STR; test_num++) {
	one_RtlIntegerToUnicodeString_test(test_num, &int2str[test_num]);
    } /* for */
}


static void one_RtlIntegerToChar_test(int test_num, const int2str_t *int2str)
{
    NTSTATUS result;
    char dest_str[STRI_BUFFER_LENGTH + 1];

    memset(dest_str, '-', STRI_BUFFER_LENGTH);
    dest_str[STRI_BUFFER_LENGTH] = '\0';
    result = pRtlIntegerToChar(int2str->value, int2str->base, int2str->MaximumLength, dest_str);
    ok(result == int2str->result,
       "(test %d): RtlIntegerToChar(%lu, %d, %d, [out]) has result %lx, expected: %lx",
       test_num, int2str->value, int2str->base, int2str->MaximumLength, result, int2str->result);
    ok(memcmp(dest_str, int2str->Buffer, STRI_BUFFER_LENGTH) == 0,
       "(test %d): RtlIntegerToChar(%lu, %d, %d, [out]) assigns string \"%s\", expected: \"%s\"",
       test_num, int2str->value, int2str->base, int2str->MaximumLength, dest_str, int2str->Buffer);
}


static void test_RtlIntegerToChar(void)
{
    NTSTATUS result;
    int test_num;

    for (test_num = 0; test_num < NB_INT2STR; test_num++) {
	one_RtlIntegerToChar_test(test_num, &int2str[test_num]);
    } /* for */

    result = pRtlIntegerToChar(int2str[0].value, 20, int2str[0].MaximumLength, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "(test a): RtlIntegerToChar(%lu, %d, %d, NULL) has result %lx, expected: %x",
       int2str[0].value, 20, int2str[0].MaximumLength, result, STATUS_INVALID_PARAMETER);

    result = pRtlIntegerToChar(int2str[0].value, 20, 0, NULL);
    ok(result == STATUS_INVALID_PARAMETER,
       "(test b): RtlIntegerToChar(%lu, %d, %d, NULL) has result %lx, expected: %x",
       int2str[0].value, 20, 0, result, STATUS_INVALID_PARAMETER);

    result = pRtlIntegerToChar(int2str[0].value, int2str[0].base, 0, NULL);
    ok(result == STATUS_BUFFER_OVERFLOW,
       "(test c): RtlIntegerToChar(%lu, %d, %d, NULL) has result %lx, expected: %x",
       int2str[0].value, int2str[0].base, 0, result, STATUS_BUFFER_OVERFLOW);

    result = pRtlIntegerToChar(int2str[0].value, int2str[0].base, int2str[0].MaximumLength, NULL);
    ok(result == STATUS_ACCESS_VIOLATION,
       "(test d): RtlIntegerToChar(%lu, %d, %d, NULL) has result %lx, expected: %x",
       int2str[0].value, int2str[0].base, int2str[0].MaximumLength, result, STATUS_ACCESS_VIOLATION);
}


START_TEST(rtlstr)
{
    InitFunctionPtrs();
    if (pRtlInitAnsiString) {
	test_RtlInitString();
	test_RtlInitUnicodeString();
	test_RtlCopyString();
	test_RtlUnicodeStringToInteger();
	test_RtlCharToInteger();
	test_RtlIntegerToUnicodeString();
	test_RtlIntegerToChar();
	test_RtlUpperChar();
	test_RtlUpperString();
	test_RtlAppendUnicodeStringToString();
    } /* if */
	/*
	 * test_RtlUpcaseUnicodeChar();
	 * test_RtlUpcaseUnicodeString();
	 * test_RtlDowncaseUnicodeString();
	 */
}
