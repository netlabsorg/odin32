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

#ifndef _metaclass_h
#define _metaclass_h

#include "mop.h"

class Metaclass : public Class {
public:
    Metaclass();

    static bool Initialize();
    char* MetaclassName();

    void TranslateClass(Environment*);
    Ptree* GetFinalizer();
    void CheckObsoleteness();
    void ProduceInitFile(Ptree* class_name);
    static bool IsBuiltinMetaclass(Ptree*);
    void InsertInitialize();
    int FindFirstNotInlinedVirtualFunction();
    void TranslateMemberFunction(Environment* env, Member& m);
    void AppendHousekeepingCode(Environment* env, Ptree* class_name,
				Ptree* creator_name, Ptree* finalizer);
    static void Load(Ptree* metaclass_name);
    static void Load(char* metaclass_name, int len);
    static void* LoadSoLib(char* file_name);
    static void* LookupSymbol(void* handle, char* symbol);

private:
    Ptree* new_function_name;
    int    first_not_inlined_vf;
};

extern void LoadMetaclass(char*);

#endif  /* _metaclass_h */
