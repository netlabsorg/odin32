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

#ifndef _typeinfo_h
#define _typeinfo_h

#include "types.h"

class Ptree;
class Class;
class Environment;
class Bind;

enum TypeInfoId {
    UndefType,
    BuiltInType, ClassType, EnumType, TemplateType,
    PointerType, ReferenceType, PointerToMemberType,
    ArrayType, FunctionType
};

// These are used by TypeInfo::WhatBuiltinType()
//
enum {
    CharType = 1, IntType = 2, ShortType = 4, LongType = 8,
    LongLongType = 16, SignedType = 32, UnsignedType = 64, FloatType = 128,
    DoubleType = 256, LongDoubleType = 512, VoidType = 1024,
    BooleanType = 2048
};

/*
  TypeInfo interprets an encoded type name.  For details of the encoded
  type name, see class Encoding in encoding.h and encoding.cc.
*/
class OCXXMOP TypeInfo : public LightObject {
public:
    TypeInfo();
    void Unknown();
    void Set(char*, Environment*);
    void Set(Class*);
    void SetVoid();
    void SetInt();
    void SetMember(Ptree*);

    TypeInfoId WhatIs();

    bool IsNoReturnType();

    bool IsConst();
    bool IsVolatile();

    uint IsBuiltInType();
    bool IsFunction();
    bool IsEllipsis();
    bool IsPointerType();
    bool IsReferenceType();
    bool IsArray();
    bool IsPointerToMember();
    bool IsTemplateClass();
    Class* ClassMetaobject();
    bool IsClass(Class*&);
    bool IsEnum();
    bool IsEnum(Ptree*& spec);

    void Dereference() { --refcount; }
    void Dereference(TypeInfo&);
    void Reference() { ++refcount; }
    void Reference(TypeInfo&);
    bool NthArgument(int, TypeInfo&);
    int NumOfArguments();
    bool NthTemplateArgument(int, TypeInfo&);

    Ptree* FullTypeName();
    Ptree* MakePtree(Ptree* = nil);

private:
    static Ptree* GetQualifiedName(Environment*, Ptree*);
    static Ptree* GetQualifiedName2(Class*);
    void Normalize();
    bool ResolveTypedef(Environment*&, char*&, bool);

    static char* SkipCv(char*, Environment*&);
    static char* SkipName(char*, Environment*);
    static char* GetReturnType(char*, Environment*);
    static char* SkipType(char*, Environment*);

private:
    int refcount;
    char* encode;
    Class* metaobject;
    Environment* env;
};

#endif /* _typeinfo_h */
