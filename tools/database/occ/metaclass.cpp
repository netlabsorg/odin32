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

#include "metaclass.h"
#include "ptree-core.h"

#if USE_DLOADER
#include <stdio.h>
#include <fstream.h>
#include <string.h>
#include <stdlib.h>	/* for exit() */

// in driver2.cc
extern void RunSoCompiler(const char* src_file);
extern void* LoadSoLib(char* file_name);
extern void* LookupSymbol(void* handle, char* symbol);

#if !USE_SO
extern void BaseClassUsed(char *, int);		// in driver2.cc
#endif /* USE_SO */

#endif /* USE_DLOADER */

extern bool verboseMode;	// defined in driver.cc
extern bool makeSharedLibrary;


// class QuoteClass
//
// Part of the implementation of QuoteClass is included here to let
// quote-class.cc linked to others if the compiler is produced by
// "g++ -o occ opencxx.a".

#include "quote-class.h"

// The followings should be automatically generated but we write it
// down here for bootstrapping.  Also see Metaclass::IsBuiltinMetaclass()

static Class* CreateQuoteClass(Ptree* def, Ptree* marg)
{
    Class* metaobject = new QuoteClass;
    metaobject->InitializeInstance(def, marg);
    return metaobject;
}

static opcxx_ListOfMetaclass QuoteClassCreator("QuoteClass",
					       CreateQuoteClass,
					       QuoteClass::Initialize,
					       nil);

opcxx_ListOfMetaclass* opcxx_init_QuoteClass()
{
    return new opcxx_ListOfMetaclass("QuoteClass", CreateQuoteClass,
				     QuoteClass::Initialize, nil);
}

bool QuoteClass::Initialize()
{
    return TRUE;
}

char* QuoteClass::MetaclassName()
{
    return "QuoteClass";
}


// class Metaclass

static Class* CreateMetaclass(Ptree* def, Ptree* marg)
{
    Class* metaobject = new Metaclass;
    metaobject->InitializeInstance(def, marg);
    return metaobject;
}

static opcxx_ListOfMetaclass metaclassCreator("Metaclass", CreateMetaclass,
					      Metaclass::Initialize, nil);

opcxx_ListOfMetaclass* opcxx_init_Metaclass()
{
    return new opcxx_ListOfMetaclass("Metaclass", CreateMetaclass,
				     Metaclass::Initialize, nil);
}

Metaclass::Metaclass()
{
    new_function_name = nil;
    first_not_inlined_vf = -1;
}

bool Metaclass::Initialize()
{
    return TRUE;
}

char* Metaclass::MetaclassName()
{
    return "Metaclass";
}

void Metaclass::TranslateClass(Environment* env)
{
    Ptree* name = Name();

    if(!IsBuiltinMetaclass(name)){
	CheckObsoleteness();
	InsertInitialize();
#if defined(_MSC_VER) || defined(_PARSE_VCC)
	AddClassSpecifier(Ptree::Make("__declspec(dllexport)"));
#endif
	AppendMember(Ptree::Make("public: char* MetaclassName() {\n"
				 "    return \"%p\"; }\n",
				 Name()));

	Ptree* tmpname = Ptree::GenSym();
	Ptree* tmpname2 = Ptree::GenSym();
	Ptree* finalizer = GetFinalizer();
	AppendAfterToplevel(env, Ptree::Make(
		"static Class* %p(Ptree* def, Ptree* marg){\n"
		"    Class* c = new %p;\n"
		"    c->InitializeInstance(def, marg);\n"
		"    return c; }\n"
		"static opcxx_ListOfMetaclass %p(\"%p\", %p,\n"
		"    %p::Initialize, %p);\n",
		tmpname, name, tmpname2, name, tmpname, name, finalizer));

	if(makeSharedLibrary){
	    ProduceInitFile(name);
	    first_not_inlined_vf = FindFirstNotInlinedVirtualFunction();
	    new_function_name = tmpname;
	    if(first_not_inlined_vf < 0)
		AppendHousekeepingCode(env, Name(), tmpname, finalizer);
	}
    }

    Class::TranslateClass(env);
}

Ptree* Metaclass::GetFinalizer()
{
    Member m;
    if(LookupMember("FinalizeClass", m) && m.Supplier() == this){
        if(!m.IsStatic())
	    ErrorMessage("FinalizeClass() must be static in ", Name(),
			 Definition());

	return Ptree::Make("%p::FinalizeClass", Name());
    }
    else
	return Ptree::Make("0");
}

void Metaclass::CheckObsoleteness()
{
    Member m;

    if(LookupMember("Finalize", m) && m.Supplier() == this)
	WarningMessage("Finalize() is obsolete.  Use FinalizeInstance() in ",
		       Name(),
		       Definition());
}

void Metaclass::ProduceInitFile(Ptree* class_name)
{
#if USE_DLOADER
#if USE_SO
    const char* fname = Ptree::Make("%p-init.cc", class_name)->ToString();
    if(verboseMode)
	cerr << "Produce " << fname << " ..\n";

    ofstream src_file(fname);
    if(!src_file){
        perror(fname);
	exit(1);
    }

    src_file << "extern void LoadMetaclass(char*);\n";
    src_file << "char* opcxx_Init_" << class_name << "(){\n";

    Ptree* base_name;
    for(int i = 0; (base_name = NthBaseClassName(i)) != nil; ++i)
	if(!base_name->Eq("Class"))
	    src_file << "  LoadMetaclass(\"" << base_name << "\");\n";

    src_file <<	"    return 0;\n}\n";

    src_file.close();

    RunSoCompiler(fname);
#else
    // Push base class names forward to RunCompiler
    Ptree* base_name;
    for (int i = 0; (base_name = NthBaseClassName(i)) != nil; ++i)
	if (!base_name->Eq("Class") && !base_name->Eq("TemplateClass"))
	    BaseClassUsed(base_name->GetPosition(), base_name->GetLength());
#endif /* USE_SO */
#endif /* USE_DLOADER */
}

bool Metaclass::IsBuiltinMetaclass(Ptree* name)
{
    return bool(name->Eq("Class") || name->Eq("Metaclass")
		|| name->Eq("TemplateClass")
		|| name->Eq("QuoteClass"));
}

void Metaclass::InsertInitialize()
{
    Member m;
    if(!LookupMember("Initialize", m) || m.Supplier() != this){
#if !defined(_MSC_VER) || (_MSC_VER >= 1100)
	AppendMember(Ptree::Make(
		"public: static bool Initialize() { return 1; }\n"));
#else
	AppendMember(Ptree::Make(
		"public: static int Initialize() { return 1; }\n"));
#endif
    }
    else if(!m.IsStatic())
	ErrorMessage("Initialize() must be static in ", Name(),
		     Definition());
}

int Metaclass::FindFirstNotInlinedVirtualFunction()
{
    Member m;
    for(int i = 0; NthMember(i, m); ++i)
	if(m.IsFunction() && m.IsVirtual() && !m.IsInline()
	   && m.Supplier() == this)
	    return i;

    WarningMessage("a metaclass should include at least one"
		   " not-inlined virtual function: ", Name(), Name());
    return -1;
}

void Metaclass::TranslateMemberFunction(Environment* env, Member& m)
{
    if(m.Nth() != first_not_inlined_vf)
	return;

    if(m.IsInline()){
	ErrorMessage("This member function should not be inlined: ",
		     m.Name(), m.ArgumentList());
	return;
    }

    AppendHousekeepingCode(env, Name(), new_function_name,
			   GetFinalizer());
}

void Metaclass::AppendHousekeepingCode(Environment* env, Ptree* class_name,
				       Ptree* creator_name,
				       Ptree* finalizer)
{
#if !defined(_MSC_VER)
    AppendAfterToplevel(env, Ptree::Make(
		"opcxx_ListOfMetaclass* opcxx_init_%p(){\n"
		"    return new opcxx_ListOfMetaclass(\"%p\", %p,\n"
		"                   %p::Initialize, %p); }\n",
		class_name, class_name, creator_name, class_name,
		finalizer));
#endif
}

void LoadMetaclass(char* metaclass_name)
{
#if USE_DLOADER
    if(metaclass_name != nil && *metaclass_name != '\0')
	if(!opcxx_ListOfMetaclass::AlreadyRecorded(metaclass_name))
	    Metaclass::Load(metaclass_name, strlen(metaclass_name));
#endif
}

void Metaclass::Load(Ptree* metaclass_name)
{
#if USE_DLOADER
    if(opcxx_ListOfMetaclass::AlreadyRecorded(metaclass_name))
	return;

    Load(metaclass_name->GetPosition(), metaclass_name->GetLength());
#endif
}

void Metaclass::Load(char* metaclass_name, int len)
{
#if USE_DLOADER
#if USE_SO
#if DLSYM_NEED_UNDERSCORE
    static char header[] = "_opcxx_Init_";
#else
    static char header[] = "opcxx_Init_";
#endif
#if defined(IRIX_CC)
    static char tail[] = "__Gv";
#else
    static char tail[] = "__Fv";
#endif

    char* func_name = new char[len + sizeof(header) + sizeof(tail)];
    strcpy(func_name, header);
    memmove(func_name + sizeof(header) - 1, metaclass_name, len);
    strcpy(func_name + sizeof(header) + len - 1, tail);

    // here, func_name is "_opcxx_Init_<metaclass>__Fv" or ".._Gv".

    char* file_name = new char[len + 9];
    memmove(file_name, metaclass_name, len);

    strcpy(file_name + len, "-init.so");
    void* handle_init = LoadSoLib(file_name);	// load <metaclass>-init.so

    // call opcxx_Init_<metaclass>() in <metaclass>-init.so

    void (*loader)();
    loader = (void (*)())LookupSymbol(handle_init, func_name);
    (*loader)();

    strcpy(file_name + len, ".so");
    void* handle = LoadSoLib(file_name);	// load <metaclass>.so

    if(verboseMode)
	cerr << "Initialize.. ";

    // call opcxx_init_<metaclass>() in <metaclass>.so

    func_name[sizeof(header) - 6] = 'i';
    void (*func)();
    func = (void (*)())LookupSymbol(handle, func_name);
    (*func)();

    delete [] file_name;
    delete [] func_name;

    if(verboseMode)
	cerr << "Done.\n";

#else /* USE_SO */

    // Win32 uses auto-initialization of DLL's static variables
    static char ext[] = ".dll";
    char* file_name = new char[len + sizeof(ext)];
    memmove(file_name, metaclass_name, len);
    strcpy(file_name + len, ext);
    void* handle = LoadSoLib(file_name);	// load <metaclass>.dll

    delete [] file_name;

    if(verboseMode)
	cerr << "Done.\n";
#endif /* USE_SO */
#endif /* USE_DLOADER */
}

void* Metaclass::LoadSoLib(char* file_name)
{
    void* handle = nil;
#if USE_DLOADER
    if(verboseMode)
	cerr << "Load " << file_name << ".. ";

    handle = ::LoadSoLib(file_name);
#endif /* USE_DLOADER */

    return handle;
}

void* Metaclass::LookupSymbol(void* handle, char* symbol)
{
    void* func = 0;
#if USE_DLOADER
    func = ::LookupSymbol(handle, symbol);
#endif
    return func;
}
