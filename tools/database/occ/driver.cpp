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
#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include "token.h"
#include "buffer.h"
#include "parse.h"
#include "walker.h"
#include "classwalk.h"
#include "ptree-core.h"
#include "mop.h"

#if defined(_MSC_VER) || defined(OS2)
#include <stdlib.h>
#else
#include <unistd.h>
#endif

static char thisVersion[] = "2.5.11";

static char copyingNote[]
	= "Copyright (c) 1997-2001 Shigeru Chiba.  All Rights Reserved.\n"
	  "Copyright (c) 1995, 1996 Xerox Corporation. All Rights Reserved.";

const char opencxxErrorMessage[] = " Error(s).  OpenC++ stops.\n";

extern const char* compilerName;

bool showProgram;
bool doCompile;
bool makeExecutable;
bool doPreprocess;
bool doTranslate;
bool verboseMode;
bool regularCpp;
bool makeSharedLibrary;
char* sharedLibraryName;
bool preprocessTwice;

const int NARGS = 64;
const char* cppArgv[NARGS];
const char* ccArgv[NARGS];
static const char* cc2Argv[NARGS];	// arguments following a source file
int cppArgc = 0;
int ccArgc = 0;
static int cc2Argc = 0;
static int numOfObjectFiles = 0;

// defined in driver2.cc
extern bool ParseTargetSpecificOptions(char* arg, char*& source_file);
extern void RunLinker();
extern char* RunPreprocessor(const char* src);
extern char* OpenCxxOutputFileName(const char* src);
extern void RunCompiler(const char* src, const char* occserc);

// defined in metaclass.cc
extern void LoadMetaclass(char*);

void Compile(int argc, char** argv);
void ParseCmdOptions(int from, int argc, char** argv, char*& source_file);
void AddCppOption(const char* arg);
void AddCcOption(const char* arg);
void CloseCcOptions();
bool IsCxxSource(char* fname);
void ShowCommandLine(const char* cmd, const char** args);

static void ReadStdin();
static void ReadFile(const char* src);
static char* RunOpencxx(const char* src);
static int ParseOpencxx(Program* parse);
static char* ParseOptions(int argc, char** argv);
static void ShowVersion();
static void ShowHelp(char**);
static void AddCc2Option(const char* arg);
static void RecordCmdOption(char* option);
static void ParseCcOptions(char* arg, char*& source_file);


void Compile(int argc, char** argv)
{
    char* src = ParseOptions(argc, argv);
    if(src != nil)
	ReadFile(src);
    else
	if(numOfObjectFiles == 0)
	    ReadStdin();
	else
	    RunLinker();
}

static void ReadStdin()
{
    cerr << "OpenC++: No source file.  Read from console.\n";

    ProgramFromStdin prog;
    int nerrors = ParseOpencxx(&prog);

    if(!showProgram)
	if(nerrors == 0)
	    prog.Write(cout, "stdout");
	else{
	    cerr << nerrors << opencxxErrorMessage;
	    exit(1);
	}
}

static void ReadFile(const char* src)
{
    char *occfile;

    if(doPreprocess){
        char* cppfile = RunPreprocessor(src);
	occfile = RunOpencxx(cppfile);
	unlink(cppfile);
	delete cppfile;
    }
    else
	occfile = RunOpencxx(src);

    if(doCompile){
        RunCompiler(src, occfile);
        // unlink(occfile);
    }

    delete occfile;
}

static char* RunOpencxx(const char* src)
{
    char* dest = OpenCxxOutputFileName(src);

    ifstream src_stream(src);
    if(!src_stream){
        perror(src);
	exit(1);
    }

    ProgramFile src_prog(src_stream);
    if(verboseMode){
	if(doTranslate)
	    cerr << "[Translate... " << src
		 << " into: " << dest << "]\n";
	else
	    cerr << "[Parse... " << src << "]\n";
    }

    int nerrors = ParseOpencxx(&src_prog);

    if(nerrors != 0){
	cerr << nerrors << opencxxErrorMessage;
	exit(1);
    }

    if(doTranslate){
	ofstream dest_stream(dest, (ios::out | ios::trunc));
	if(!dest_stream){
	    perror(dest);
	    exit(1);
	}

	src_prog.Write(dest_stream, dest);
    }

    src_stream.close();
    return dest;
}

static int ParseOpencxx(Program* prog)
{
    Lex lex(prog);
    Parser parse(&lex);
    ClassWalker w(&parse);
    Ptree* def;

    while(parse.rProgram(def)){
	if(showProgram)
	    def->Display2(cout);

	if(doTranslate){
	    Ptree* def2 = w.Translate(def);
	    Ptree* before = w.GetInsertedPtree();
	    Ptree* after = w.GetAppendedPtree();

	    prog->Insert(def, before, after);
	    if(def != def2)
		prog->MinimumSubst(def2, def);
	}
    }

    return parse.NumOfErrors();
}

static char* ParseOptions(int argc, char** argv)
{
    char* source_file = nil;
    showProgram = FALSE;
    doCompile = TRUE;
    verboseMode = FALSE;
    makeExecutable = TRUE;
    doPreprocess = TRUE;
    doTranslate = TRUE;
    regularCpp = FALSE;
    makeSharedLibrary = FALSE;
    sharedLibraryName = nil;
    preprocessTwice = FALSE;

    AddCppOption(compilerName);
    AddCcOption(compilerName);

    ParseCmdOptions(1, argc, argv, source_file);

    return source_file;
}

void ParseCmdOptions(int from, int argc, char** argv, char*& source_file)
{
    int i;
    for(i = from; i < argc; ++i)
	if(strcmp("--", argv[i]) == 0)
	    break;
	else if(strcmp("-E", argv[i]) == 0)
	    doCompile = FALSE;
	else if(strcmp("-g", argv[i]) == 0)
	    AddCcOption(argv[i]);
	else if (strcmp("-n", argv[i]) == 0)
	    doPreprocess = FALSE;
	else if(*argv[i] == '-' && argv[i][1] == 'm'){
	    makeSharedLibrary = TRUE;
	    sharedLibraryName = &argv[i][2];
	}
	else if (strcmp("-P", argv[i]) == 0)
	    preprocessTwice = TRUE;
        else if (strcmp("-p", argv[i]) == 0)
	    doTranslate = FALSE;
	else if (strcmp("-C", argv[i]) == 0) {
	    AddCppOption("-C");
	    preprocessTwice = TRUE;
	}
	else if(strcmp("-c", argv[i]) == 0)
	    makeExecutable = FALSE;
	else if(strcmp("-l", argv[i]) == 0){
	    cout << "[Loaded metaclasses...]\n";
	    opcxx_ListOfMetaclass::PrintAllMetaclasses();
	    exit(0);
	}
	else if(*argv[i] == '-' && argv[i][1] == 'S')
	    LoadMetaclass(&argv[i][2]);
	else if(strcmp("-s", argv[i]) == 0)
	    showProgram = TRUE;
	else if(strcmp("-v", argv[i]) == 0)
	    verboseMode = TRUE;
	else if(strcmp("-V", argv[i]) == 0){
	    ShowVersion();
	    exit(1);
	}
	else if (strcmp("--regular-c++", argv[i]) == 0)
	    regularCpp = TRUE;
	else if(*argv[i] == '-'
		&& (argv[i][1] == 'D' || argv[i][1] == 'I'))
	    AddCppOption(argv[i]);
	else if(*argv[i] == '-' && argv[i][1] == 'd' && argv[i][2] != '\0')
	    AddCppOption(&argv[i][2]);
	else if(*argv[i] == '-' && argv[i][1] == 'M')
	    RecordCmdOption(&argv[i][2]);
	else if(*argv[i] == '-'){
	    ShowVersion();
	    ShowHelp(argv);
	    exit(1);
	}
	else if(!ParseTargetSpecificOptions(argv[i], source_file))
	    ParseCcOptions(argv[i], source_file);

    while(++i < argc)
	if(!ParseTargetSpecificOptions(argv[i], source_file))
	    ParseCcOptions(argv[i], source_file);

    if(!doTranslate)
	doCompile = FALSE;
}

static void ShowVersion()
{
    cerr << "OpenC++ version " << thisVersion << "\n" << copyingNote << "\n";
}

static void ShowHelp(char** argv)
{
    cerr << "Usage: "
	 << argv[0]
	 << " [-l][-s][-V][-v][-E][-m[<file name>]][-c][-n][-p][--regular-c++]\n"
	 << "\t\t[-I<directory>][-D<macro>[=<def>]][-M<option>[=<value>]]\n"
	 << "\t\t[-g][-d<option>][-S<metaclass>]\n"
	 << "\t\t[-- <compiler options>] <source file>\n"
	 << "\n"
	 << "    -g             Produce debugging information\n"
	 << "    -M             Specify an <option> with <value> passed to metaobjects\n"
	 << "    -l             List metaclasses\n"
	 << "    -S             Load a metaclass\n"
	 << "    -s             Show program tree on stdout\n"
	 << "    -V             Show version\n"
	 << "    -v             Verbose mode\n"
	 << "\n"
	 << " Building stages options\n"
	 << "    -n             Don't preprocess\n"
	 << "    -p             Don't translate (stop after parsing)\n"
	 << "    -E             Don't compile (stop after translation)\n"
	 << "    -c             Don't make executable (stop after compilation)\n"
#if !defined(IRIX_CC) && !defined (_MSC_VER)
	 << "    -P             Preprocess again after translation\n"
#endif
	 << "    -m             Compile a metaclass (make a shared library)\n"
	 << "\n"
	 << " Preprocessor options\n"
	 << "    -I<directory>  Add <directory> to the #include path\n"
	 << "    -D<name>=<def> Define a macro <name> as <def>\n"
	 << "    -d<option>     Specify a preprocessor option\n"
#if !defined(IRIX_CC) && !defined (_MSC_VER)
	 << "    -C             Don't discard comments\n"
#endif
	 << "\n"
	 << " Other options\n"
	 << "    --regular-c++  Inhibit the extended syntax\n";
}

void AddCppOption(const char* arg)
{
    if(cppArgc < NARGS)
	cppArgv[cppArgc++] = arg;
    else{
	cerr << "OpenC++: too many arguments\n";
	exit(1);
    }
}

void AddCcOption(const char* arg)
{
    if(ccArgc < NARGS)
	ccArgv[ccArgc++] = arg;
    else{
	cerr << "OpenC++: too many arguments\n";
	exit(1);
    }
}

void CloseCcOptions()
{
    for(int i = 0; i < cc2Argc; ++i)
	AddCcOption(cc2Argv[i]);

    AddCcOption((char*)0);
}

static void AddCc2Option(const char* arg)
{
    if(cc2Argc < NARGS)
	cc2Argv[cc2Argc++] = arg;
    else{
	cerr << "OpenC++: too many arguments\n";
	exit(1);
    }
}

static void RecordCmdOption(char* option)
{
    if(option == nil || *option == '\0')
	return;

    char* value = strchr(option, '=');
    if(value != nil)
	*value++ = '\0';

    if(!Class::RecordCmdLineOption(option, value)){
	cerr << "OpenC++: Too many -M options.\n";
	exit(1);
    }
}

static void ParseCcOptions(char* arg, char*& source_file)
{
    static char* errmsg
	= "OpenC++: only a single source file is accepted at a time.\n";

    if(arg != nil && arg[0] != '-'){
	if(IsCxxSource(arg))
	    if(source_file == nil){
		source_file = arg;
		return;
	    }
	    else{
		cerr << errmsg;
		exit(1);
	    }
    }

    ++numOfObjectFiles;
    if(source_file == nil)	// source file is already given
	AddCcOption(arg);
    else
	AddCc2Option(arg);

    return;
}

/*
  File Naming convention

  .c, .C, .cc, .cpp, .cxx : C++ source files
  .mc : metaclass files
  .occ, .ii, .i (IRIX_CC): temporary files used by OpenC++
*/
bool IsCxxSource(char* fname)
{
    char* ext = strrchr(fname, '.');
    if(ext == nil)
	return FALSE;

    if(strcmp(ext,    ".cc") == 0
       || strcmp(ext, ".C") == 0
       || strcmp(ext, ".c") == 0
       || strcmp(ext, ".mc") == 0
       || strcmp(ext, ".cxx") == 0
       || strcmp(ext, ".cpp") == 0
       || strcmp(ext, ".ii") == 0
       || strcmp(ext, ".i") == 0
       || strcmp(ext, ".occ") == 0)
	return TRUE;

    return FALSE;
}

void ShowCommandLine(const char*, const char** args)
{
    while(*args != nil)
	cerr << ' ' << *args++;
}
