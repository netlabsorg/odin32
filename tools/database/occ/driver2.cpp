/*
  Copyright (C) 1997-2001 Shigeru Chiba, Tokyo Institute of Technology.

  Permission to use, copy, distribute and modify this software and
  its documentation for any purpose is hereby granted without fee,
  provided that the above copyright notice appear in all copies and that
  both that copyright notice and this permission notice appear in
  supporting documentation.

  Shigeru Chiba makes no representations about the suitability of this
  software for any purpose.  It is provided "as is" without express or
  implied warranty.
*/

/*
  Copyright (c) 1995, 1996 Xerox Corporation.
  All Rights Reserved.

  Use and copying of this software and preparation of derivative works
  based upon this software are permitted. Any copy of this software or
  of any derivative work must include the above copyright notice of
  Xerox Corporation, this paragraph and the one after it.  Any
  distribution of this software or derivative works must comply with all
  applicable United States export control laws.

  This software is made available AS IS, and XEROX CORPORATION DISCLAIMS
  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#include <stdio.h>
#include <fstream.h>
#include <string.h>

/*** driver2.cpp is a Win32 version of driver2.cc ***/

// No possibility to include types.h, because of type clash
// at BOOL between types.h and windows.h
// So, windows.h is included always (it is needed for dynamic
// loading support) and it is assumed that USE_DLOADER is always
// on for Microsoft Visual C++

#ifdef OS2
#define INCL_BASE
#include <os2.h>
#include "types.h"
#else
#define STRICT
#define WIN32_LEAN_AND_MEAN
#define NOSERVICE
#include <windows.h>
#endif

#include <stdlib.h>	// For exit
#include <process.h>	// For _spawn
#include <fcntl.h>	// For _O_RDONLY
#include <io.h>		// For open, lseek, read

/*
  Be carefull !!! I have changed the meaning of the extensions!!!!!!
  In the original openc++ :

  ".cpp" file --(preprocessor)--> ".occ" file --(translator)--> ".i" file
  --(compiler)--> ".o" file

 In this version :

  ".cpp" file --(preprocessor)--> ".i" file --(translator)--> ".occ" file
  --(compiler)--> ".o" file
*/

#define PREPROCESSOR_EXT  ".i"	// cl generates .i files when preprocessing
#define TRANSLATOR_EXT    ".occ"

#ifdef OS2
const char* compilerName = "icc";
#else
const char* compilerName = "cl";
#endif
const char* opencxxErrorMessage = " Error(s).  OpenC++ stops.\n";

// defined in driver.cc

extern bool showProgram;
extern bool doCompile;
extern bool makeExecutable;
extern bool doPreprocess;
extern bool doTranslate;
extern bool verboseMode;
extern bool regularCpp;
extern bool makeSharedLibrary;

extern const char* cppArgv[];
extern const char* ccArgv[];

const NLIBS = 32;
static int libsArgc = 0;
const char* libsArgv[NLIBS];

const NLINKERARGS = 32;
static int linkerArgc = 0;
static bool linkerFlag = FALSE;
char* linkerArgv[NLINKERARGS];

extern void ParseCmdOptions(int from, int argc, char** argv, char*& source);
extern void AddCppOption(const char* arg);
extern void AddCcOption(const char* arg);
extern void CloseCcOptions();
extern bool IsCxxSource(char* fname);
extern void ShowCommandLine(const char* cmd, const char** args);

void RunLinker();
char* RunPreprocessor(const char* src);
char* OpenCxxOutputFileName(const char* src);
void RunCompiler(const char* src, const char* occsrc);
void RunSoCompiler(const char* src_file);
void BaseClassUsed(char *name, int len);
void* LoadSoLib(char* file_name);
void* LookupSymbol(void* handle, char* symbol);
bool ParseTargetSpecificOptions(char* arg, char*& source_file);

static int Launch(const char *tool, const char *const *argv);
static char* MakeTempFilename(const char* src, const char* suffix);
static void ParseFileOptions(const char* filename, char*& source_file);
static void File2Args(const char *fname, int max, int &argc, char *argv[]);
static void String2Args(const char *str, int max, int &argc, char *argv[]);
static char *NewArg(const char *from, size_t size, int argc, int max);
static void ParseBackslash(char arg[], size_t &i, const char* &string);
static void ParseDoubleQuote(char arg[], size_t &i, const char* &string);


void RunLinker()
{
    cerr << "OpenC++: no source file.\n";
}

char *RunPreprocessor(const char *file_in)
{
    #ifdef OS2
    AddCppOption("-Q+");                /* quiet (-nologo) */
    if(!regularCpp)
	    AddCppOption("-D__opencxx");
    AddCppOption("-P");                 /* preprocess the file. */
    AddCppOption("-Pe+");               /* #line */
    AddCppOption("-Tp"); // Consider source file as C++ file,

    #else

    AddCppOption("-nologo");
    if(!regularCpp)
	AddCppOption("-D__opencxx");

    AddCppOption("-E"); // Use -EP if you don't want the directives #line
    AddCppOption("-P"); // Preprocessor output to <file>.i
    AddCppOption("-Tp"); // Consider source file as C++ file,
			 // whatever extension.
    #endif
    AddCppOption(file_in);
    AddCppOption((char*)0);

    if(verboseMode) {
        cerr << "[Preprocess... ";
	ShowCommandLine(compilerName, cppArgv);
	cerr << "]\n";
    }

    int status = Launch(compilerName, cppArgv);
    if(status != 0) {
        if(status == -1)
	    perror("cannot invoke the preprocessor");

        // VIC Feb 20 1998
        // MSVC 5.0 returns non-zero exit code on normal input
        if (status != 2)
	    exit(status);
    }

    return MakeTempFilename(file_in, PREPROCESSOR_EXT);
}

char* OpenCxxOutputFileName(const char* src)
{
    return MakeTempFilename(src, TRANSLATOR_EXT);
}

void BaseClassUsed(char *name, int len)
{
    static char tail[] = ".lib";
    char *lib_name = new char[len + sizeof(tail)];
    memcpy(lib_name, name, len);
    strcpy(lib_name + len, tail);
    libsArgv[libsArgc++] = lib_name;
}

void RunCompiler(const char* org_src, const char *file_in)
{
    AddCcOption("-nologo");
    if(!makeExecutable)
	AddCcOption("-c");

    AddCcOption("-Tp");
    AddCcOption(file_in);
    if (makeExecutable && makeSharedLibrary) {
        // Add superclass libraries
        for (int i = 0; i < libsArgc; ++i)
            AddCcOption(libsArgv[i]);
        AddCcOption("-link");
        AddCcOption("-dll");
        char buffer[256];
        strcpy(buffer, "-out:");
        strcat(buffer, org_src);
        char *last_dot = strrchr(buffer, '.');
        if (last_dot != 0) *last_dot = '\0';
        strcat(buffer, ".dll");
        for (int j = 0; j < linkerArgc; ++j)
            AddCcOption(linkerArgv[j]);

        AddCcOption(buffer);
        // Add linked libraries
	if(linkerArgc == 0){
	    AddCcOption("occ.lib");	// default
	    AddCcOption("gc.lib");
	}
	else
	    for(int j = 0; j < linkerArgc; ++j)
		AddCcOption(linkerArgv[j]);
    }

    CloseCcOptions();

    if(verboseMode){
        cerr << "[Compile... ";
	ShowCommandLine(compilerName, ccArgv);
	cerr << "]\n";
    }

    int status = Launch(compilerName, ccArgv);
    if(status != 0){
	if(status == -1)
	    perror("cannot invoke a compiler");

	exit(status);
    }
}

void RunSoCompiler(const char* src_file)
{
    // not implemented
}

void* LoadSoLib(char* file_name)
{
    #ifdef OS2
    APIRET  rc;
    HMODULE hmod = NULL;
    CHAR    szObj[32];
    rc = DosLoadModule(szObj, sizeof(szObj), file_name, &hmod);
    if (rc) {
        cerr << "DosLoadModule failed: rc=" << rc << "obj=" << szObj << '\n';
        exit(8);                        /* isn't this a fatal error? */
    }
    return (void *) hmod;
    #else
    // NYI - error handling
    HINSTANCE handle = LoadLibrary(file_name);
    if (handle == NULL) {
	cerr << "LoadLibrary() failed: " << GetLastError() << '\n';
	exit(1);
    }
    return (void *) handle;
    #endif
}

void* LookupSymbol(void* handle, char* symbol)
{
    #ifdef OS2
    PFN     pfn;
    if (DosQueryProcAddr((HMODULE) handle, 0, symbol, &pfn))
        return NULL;
    return (void *)pfn;
    #else
    return (void *) GetProcAddress((HMODULE) handle, symbol);
    #endif
}


static int Launch(const char *tool, const char *const *argv)
{
    #ifdef OS2
    return _spawnvp(P_WAIT, (char*)tool, (char**)argv);
    #else
    return _spawnvp(_P_WAIT, tool, argv);
    #endif
}

static char* MakeTempFilename(const char* src, const char* suffix)
{
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];

    #ifdef OS2
    _splitpath((char*)src, drive, dir, fname, ext);
    #else
    _splitpath(src, drive, dir, fname, ext);
    #endif
    char* tmpfile = new char[_MAX_PATH];
    #ifdef OS2
    _makepath(tmpfile, drive, dir, fname, (char*)suffix);
    #else
    _makepath(tmpfile, drive, dir, fname, suffix);
    #endif
    return tmpfile;
}

/*
  Arguments parsing (low-level stuff)
 The goal of this section is to split a string into an argv[] array
 This is necessary for two reasons:
        - To be able to parse an options file (a text file containg
          options, allowing to overcome the MS-DOS limitation to 128
          characters for the command line (under win31)
        - To be able to convert the LPSTR cmdLine argument of the WinMain
	  function (for future developments I have planned to do)
*/

bool ParseTargetSpecificOptions(char* arg, char*& source_file)
{
    if(*arg == '@'){
	// +1 to skip '@' : the following characters are the filename
	ParseFileOptions(arg + 1, source_file);
	return TRUE;
    }
    else if(strcmp("link",arg+1)==0){
        linkerFlag = TRUE;
        return TRUE;
    }
    else if(linkerFlag && linkerArgc < NLINKERARGS && IsCxxSource(arg)==FALSE)
    {
        linkerArgv[linkerArgc] = new char[strlen(arg)+1];
        strcpy(linkerArgv[linkerArgc++], arg);
        return TRUE;
    }
    else
	return FALSE;
}

static void ParseFileOptions(const char* filename, char*& source_file)
{
    const max = 32;
    int argc;
    char *argv[max];

    File2Args(filename, max, argc, argv);
    ParseCmdOptions(0, argc, argv, source_file);
}

static void File2Args(const char *filename, int max, int &argc, char *argv[])
//                                IN            IN        OUT         OUT
{
    #ifdef OS2
    int handle = open(filename, O_RDONLY);
    #else
    int handle = open(filename, _O_RDONLY);
    #endif
    if(handle == -1 ){
        perror(filename);
        exit(1);
    }

    long size = filelength(handle);
    if(size == -1 ){
        perror(filename);
        exit(1);
    }

    char *buffer = new char[size+1];
    if(buffer == NULL){
        cerr << "out of memory\n";
        exit(1);
    }

    size = read(handle, buffer, (int)size);
    buffer[size] = '\0';

    String2Args(buffer, max, argc, argv);
    delete [] buffer;
}

/*
  Parsing Command-Line Arguments

  Microsoft C startup code uses the following rules when parsing arguments
  given on the MS-DOS command line:

  Arguments are delimited by white space, which is either a space or a
  tab.  A string surrounded by double quotation marks is interpreted as
  a single argument, regardless of white space contained within. A
  quoted string can be embedded in an argument.. Note that the caret (^)
  is not recognized as an escape character or delimiter. A double
  quotation mark preceded by a backslash, \", is interpreted as a
  literal double quotation mark (").  Backslashes are interpreted
  literally, unless they immediately precede a double quotation mark.
  If an even number of backslashes is followed by a double quotation
  mark, one \ is placed in the argv array for every \\ pair, and the "
  is interpreted as a string delimiter.  If an odd number of backslashes
  is followed by a double quotation mark, one is placed in the argv
  array for every \ pair, and the " is escaped by the \ remaining,
  causing a literal " to be placed in argv.

  The following list shows the interpreted result passed to argv for
  several examples of command-line arguments.

  Command-Line Input	argv[1]	argv[2]	argv[3]
  "a b c" d e		a b c	d	e
  "ab\"c" "\\" d	ab"c	\	d
  a\\\b d"e f"g h	a\\\b	de fg	h
  a\\\"b c d		a\"b	c	d
  a\\\\"b c" d e	a\\b c	d	e
*/

#define is_blank(c)     ((c) == ' ' || (c) == '\t' || (c) == '\f' || (c) == '\r' || (c) == '\n')

/*                           IN          IN        OUT         OUT    */
static void String2Args(const char *string, int max, int &argc, char *argv[])
{
    argc = 0;
    if(string == NULL)
	return;

    const sizemax = 255;
    char arg[sizemax];

    while(is_blank(*string))
	string++;

    size_t i = 0;
    while(1){
        if(*string == '"')
	    ParseDoubleQuote(arg, i, string);
        else if(*string == '\\')
	    ParseBackslash(arg, i, string);
        else if(is_blank(*string) || *string == '\0'){
            argv[argc++] = NewArg(arg, i, argc, max);
            i = 0;
            while(is_blank(*string))
		string++;
            if(*string == '\0')
		break;
        }
        else{
            if(i == sizemax){
                cerr << "argument too long\n";
                exit(1);
            }

            arg[i++] = *string++;
        }
    }

    argv[argc] = NewArg(NULL, 0, argc, max);
}

static char *NewArg(const char *from, size_t size, int argc, int max)
{
    if (argc == max) {
        cerr << "too many arguments\n";
        exit(1);
    }

    if (from == NULL) return NULL;

    char *arg = new char[size+1];
    if (arg == NULL) {
        cerr << "out of memory\n";
        exit(1);
    }
    strncpy(arg, from, size);
    *(arg + size) = '\0';

    return arg;
}

static void ParseBackslash(char arg[], size_t &i, const char* &string)
{
    string++;
    int count = 1;
    while(*string == '\\'){
        string++;
        count++;
    }

    if(*string == '"'){
        if(count % 2 == 0){
            count = count / 2;
            while(count-- != 0)
		arg[i++] = '\\';
        }
	else{
            count = count / 2;
            while(count-- != 0)
		arg[i++] = '\\';

            arg[i++] = *string++;
        }
    }
    else
	while(count-- != 0)
	    arg[i++] = '\\';
}

static void ParseDoubleQuote(char arg[], size_t &i, const char* &string)
{
    string++;
    while(*string != '\0'){
        if(*string == '\\')
	    ParseBackslash(arg, i, string);
        else if(*string == '"'){
            string++;
            return;
        }
        else
	    arg[i++] = *string++;
    }

    cerr << "missing double quote\n";
    exit(1);
}
