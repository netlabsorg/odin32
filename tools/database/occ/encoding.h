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

#ifndef _encoding_h
#define _encoding_h

#include "types.h"

class ostream;
class Ptree;
class Environment;
class Bind;

class Encoding {
public:
    enum { MaxNameLen = 256 }; // const MaxNameLen = 256;
			       // MSVC doesn't compile
    Encoding() { len = 0; }
    Encoding(Encoding& e) { Reset(e); }
    void Clear() { len = 0; }
    void Reset(Encoding&);
    char* Get();
    bool IsEmpty() { return len == 0; }

    void CvQualify(Ptree*, Ptree* = nil);
    void SimpleConst() { Append("Ci", 2); }
    void GlobalScope();				// ::Type
    void SimpleName(Ptree*);
    void NoName();
    void Template(Ptree*, Encoding&);
    void Qualified(int);
    void Destructor(Ptree*);
    void PtrOperator(int);
    void PtrToMember(Encoding&, int);
    void CastOperator(Encoding&);
    void Array() { Insert('A'); }
    void Function(Encoding& args) { Insert((char*)args.name, args.len); }
    void Recursion(Encoding& e) { Insert((char*)e.name, e.len); }
    void StartFuncArgs() { Append((unsigned char)'F'); }
    void EndFuncArgs() { Append((unsigned char)'_'); }
    void Void() { Append((unsigned char)'v'); }
    void EllipsisArg() { Append((unsigned char)'e'); }
    void NoReturnType() { Append((unsigned char)'?'); }
    void ValueTempParam() { Append((unsigned char)'*'); }

    void Insert(unsigned char);
    void Insert(char*, int);
    void Append(unsigned char);
    void Append(char*, int);
    void Append(Encoding& e) { Append((char*)e.name, e.len); }
    void AppendWithLen(char*, int);
    void AppendWithLen(Encoding& e) { AppendWithLen((char*)e.name, e.len); }

    static void Print(ostream&, char*);
    static char* GetBaseName(char*, int&, Environment*&);
    static Ptree* MakePtree(unsigned char*&, Ptree*);

    static Ptree* MakeQname(unsigned char*&);
    static Ptree* MakeLeaf(unsigned char*&);
    static bool IsSimpleName(unsigned char*);
    static Ptree* NameToPtree(char*, int);

    static unsigned char* GetTemplateArguments(unsigned char*, int&);

private:
    static Environment* ResolveTypedefName(Environment*, char*, int);
    static int GetBaseNameIfTemplate(unsigned char*, Environment*&);

private:
    unsigned char name[MaxNameLen];
    int len;

public:
    static Ptree *bool_t, *char_t, *int_t, *short_t, *long_t,
		 *float_t, *double_t, *void_t;

    static Ptree *signed_t, *unsigned_t, *const_t, *volatile_t;

    static Ptree *operator_name, *new_operator, *anew_operator,
		 *delete_operator, *adelete_operator;

    static Ptree *star, *ampersand, *comma, *dots, *scope, *tilder,
		 *left_paren, *right_paren, *left_bracket, *right_bracket,
		 *left_angle, *right_angle;
};

#endif /* _encoding_h */
