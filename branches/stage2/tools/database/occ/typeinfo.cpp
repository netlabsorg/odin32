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

#include "typeinfo.h"
#include "encoding.h"
#include "ptree-core.h"
#include "env.h"
#include "mop.h"

TypeInfo::TypeInfo()
{
    refcount = 0;
    encode = nil;
    metaobject = nil;
    env = nil;
}

void TypeInfo::Unknown()
{
    refcount = 0;
    encode = nil;
    metaobject = nil;
    env = nil;
}

void TypeInfo::Set(char* type, Environment* e)
{
    refcount = 0;
    encode = type;
    metaobject = nil;
    env = e;
}

void TypeInfo::Set(Class* m)
{
    refcount = 0;
    encode = nil;
    metaobject = m;
    env = nil;
}

void TypeInfo::SetVoid()
{
    refcount = 0;
    encode = "v";
    metaobject = nil;
    env = nil;
}

void TypeInfo::SetInt()
{
    refcount = 0;
    encode = "i";
    metaobject = nil;
    env = nil;
}

void TypeInfo::SetMember(Ptree* member)
{
    Class* c = ClassMetaobject();
    if(c == nil)
	Unknown();
    else{
	Environment* e = c->GetEnvironment();
	if(e == nil)
	    Unknown();
	else
	    e->Lookup(member, *this);
    }
}

TypeInfoId TypeInfo::WhatIs()
{
    if(refcount > 0)
	return PointerType;

    Normalize();
    if(metaobject != nil)
	return ClassType;

    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    if(ptr == nil)
	return UndefType;

    switch(*ptr){
    case 'T' :
	return TemplateType;
    case 'P' :
	return PointerType;
    case 'R' :
	return ReferenceType;
    case 'M' :
	return PointerToMemberType;
    case 'A' :
	return ArrayType;
    case 'F' :
	return FunctionType;
    case 'S' : case 'U' :
    case 'b' : case 'c' : case 'i' : case 's' : case 'l' : case 'j' :
    case 'f' : case 'd' : case 'r' : case 'v' :
	return BuiltInType;
    default :
	if(*ptr == 'Q' || *(unsigned char*)ptr >= 0x80){
	    TypeInfo t;
	    Class* c;
	    t.Set(ptr, e);
	    if(t.IsClass(c))
		return ClassType;
	    else if(t.IsEnum())
		return EnumType;
	}

	return UndefType;
    };
}

bool TypeInfo::IsNoReturnType()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == '?');
}

bool TypeInfo::IsConst()
{
    Normalize();
    char* ptr = encode;
    return(ptr != nil && *ptr == 'C');
}

bool TypeInfo::IsVolatile()
{
    Normalize();
    char* ptr = encode;
    if(ptr == nil)
	return FALSE;
    else if(*ptr == 'V')
	return TRUE;
    else if(*ptr == 'C')
	return(ptr[1] == 'V');
    else
	return FALSE;
}

uint TypeInfo::IsBuiltInType()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    if(ptr == nil)
	return 0;

    uint result = 0;
    for(;; ++ptr){
	switch(*ptr){
	case 'b' :
	    return(result | BooleanType);
	case 'c' :
	    return(result | CharType);
	case 'i' :
	    return(result | IntType);
	case 's' :
	    return(result | ShortType);
	case 'l' :
	    return(result | LongType);
	case 'j' :
	    return(result | LongLongType);
	case 'f' :
	    return(result | FloatType);
	case 'd' :
	    return(result | DoubleType);
	case 'r' :
	    return(result | LongDoubleType);
	case 'v' :
	    return(result | VoidType);
	case 'S' :
	    result |= SignedType;
	    break;
	case 'U' :
	    result |= UnsignedType;
	    break;
	default :
	    return 0;
	}
    }
}

bool TypeInfo::IsFunction()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'F');
}

bool TypeInfo::IsEllipsis()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'e');
}

bool TypeInfo::IsPointerType()
{
    if(refcount > 0)
	return TRUE;

    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    if(ptr != nil){
	char c = *ptr;
	return c == 'P' || c == 'A' || c == 'M';
    }

    return FALSE;
}

bool TypeInfo::IsReferenceType()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'R');
}

bool TypeInfo::IsArray()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'A');
}

bool TypeInfo::IsPointerToMember()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'M');
}

bool TypeInfo::IsTemplateClass()
{
    Normalize();
    Environment* e = env;
    char* ptr = SkipCv(encode, e);
    return(ptr != nil && *ptr == 'T');
}

Class* TypeInfo::ClassMetaobject()
{
    Class* c;
    IsClass(c);
    return c;
}

bool TypeInfo::IsClass(Class*& c)
{
    Normalize();
    if(metaobject != nil){
	c = metaobject;
	return TRUE;
    }
    else{
	c = nil;
	Environment* e = env;
	char* encode2 = SkipCv(encode, e);
	if(encode == encode2)
	    return FALSE;

	TypeInfo tinfo;
	tinfo.Set(encode2, e);
	return tinfo.IsClass(c);
    }
}

bool TypeInfo::IsEnum()
{
    Ptree* spec;
    return IsEnum(spec);
}

bool TypeInfo::IsEnum(Ptree*& spec)
{
    spec = nil;
    Normalize();
    if(metaobject != nil)
	return FALSE;
    else{
	Bind* bind;
	int len;
	Environment* e = env;
	char* name = Encoding::GetBaseName(encode, len, e);
	if(name != nil && e != nil)
	    if(e->LookupType(name, len, bind))
		if(bind != nil && bind->What() == Bind::isEnumName){
		    spec = ((BindEnumName*)bind)->GetSpecification();
		    return TRUE;
		}

	e = env;
	name = SkipCv(encode, e);
	if(name == encode)
	    return FALSE;

	TypeInfo tinfo;
	tinfo.Set(name, e);
	return tinfo.IsEnum(spec);
    }
}

void TypeInfo::Dereference(TypeInfo& t)
{
    t.refcount = refcount - 1;
    t.encode = encode;
    t.metaobject = metaobject;
    t.env = env;
}

void TypeInfo::Reference(TypeInfo& t)
{
    t.refcount = refcount + 1;
    t.encode = encode;
    t.metaobject = metaobject;
    t.env = env;
}

bool TypeInfo::NthArgument(int n, TypeInfo& t)
{
    Environment* e = env;
    Normalize();
    char* ptr = SkipCv(encode, e);
    if(ptr == nil || *ptr != 'F'){
	t.Unknown();
	return FALSE;
    }

    ++ptr;
    if(*ptr == 'v'){
	t.SetVoid();
	return FALSE;		// no arguments
    }

    while(n-- > 0){
	ptr = SkipType(ptr, e);
	if(ptr == nil || *ptr == '_'){
	    t.Unknown();
	    return FALSE;
	}
    }

    t.Set(ptr, e);
    return TRUE;
}

int TypeInfo::NumOfArguments()
{
    Environment* e = env;
    Normalize();
    char* ptr = SkipCv(encode, e);
    if(ptr == nil || *ptr != 'F')
	return -1;	// not a function

    ++ptr;
    if(*ptr == 'v')
	return 0;	// no arguments

    int n = 0;
    for(;;){
	++n;
	ptr = SkipType(ptr, e);
	if(ptr == nil || *ptr == '_')
	    return n;
    }
}

bool TypeInfo::NthTemplateArgument(int n, TypeInfo& t)
{
    Environment* e = env;
    Normalize();
    char* ptr = SkipCv(encode, e);
    if(ptr == nil || *ptr != 'T'){
	t.Unknown();
	return FALSE;
    }

    int len;
    ptr = (char*)Encoding::GetTemplateArguments((unsigned char*)++ptr, len);
    char* end = ptr + len;
    while(n-- > 0){
	ptr = SkipType(ptr, e);
	if(ptr == nil || ptr >= end) {
	    t.Unknown();
	    return FALSE;
	}
    }

    t.Set(ptr, e);
    return TRUE;
}

Ptree* TypeInfo::FullTypeName()
{
    Ptree *qname, *head;

    Normalize();
    if(metaobject != nil){
	qname = metaobject->Name();
	head = GetQualifiedName2(metaobject);
	if(head == nil)
	    return qname;
	else
	    return Ptree::Snoc(head, qname);
    }

    Environment* e = env;
    unsigned char* name = (unsigned char*)SkipCv(encode, e);
    if(name == nil)
	return nil;

    if(IsBuiltInType())
	return Encoding::MakePtree(name, nil)->First();

    if(*name == 'T'){
	++name;
	qname = Encoding::MakeLeaf(name);
	head = GetQualifiedName(e, qname);
	if(head == nil)
	    return qname;
	else
	    return Ptree::Snoc(head, qname);
    }
    else if(*name == 'Q'){
	qname = Encoding::MakeQname(++name);
	head = GetQualifiedName(e, qname->Car());
	if(head == nil)
	    return qname;
	else
	    return Ptree::Nconc(head, qname);
    }
    else if(Encoding::IsSimpleName(name)){
	qname = Encoding::MakeLeaf(name);
	head = GetQualifiedName(e, qname);
	if(head == nil)
	    return qname;
	else
	    return Ptree::Snoc(head, qname);
    }
    else
	return nil;
}

Ptree* TypeInfo::GetQualifiedName(Environment* e, Ptree* tname)
{
    Class* c = e->LookupClassMetaobject(tname);
    if(c == nil)
	return nil;
    else
	return GetQualifiedName2(c);
}

Ptree* TypeInfo::GetQualifiedName2(Class* c)
{
    Ptree* qname = nil;
    Environment* e = c->GetEnvironment();
    if(e != nil)
	e = e->GetOuterEnvironment();

    for(; e != nil; e = e->GetOuterEnvironment()){
	c = e->IsClassEnvironment();
	if(c != nil)
	    qname = Ptree::Cons(c->Name(),
				Ptree::Cons(Encoding::scope, qname));
    }

    return qname;
}

Ptree* TypeInfo::MakePtree(Ptree* name)
{
    Normalize();
    if(metaobject != nil){
	Ptree* decl;
	if(name == nil)
	    decl = nil;
	else
	    decl = Ptree::List(name);

	return Ptree::List(FullTypeName(), decl);
    }
    else if(encode != nil){
	unsigned char* ptr = (unsigned char*)encode;
	return Encoding::MakePtree(ptr, name);
    }
    else
	return nil;
}

void TypeInfo::Normalize()
{
    Environment* e = env;
    char* ptr = encode;
    int r = refcount;

    if(ptr == nil)
	return;
    else if(r > 0)
	return;

    while(r < 0)
	switch(*ptr){
	case 'C' :	// const
	case 'V' :	// volatile
	    ++ptr;
	    break;
	case 'A' :	// array
	case 'P' :	// pointer *
	case 'R' :	// reference &
	    ++ptr;
	    ++r;
	    break;
	case 'F' :	// function
	case 'M' :	// pointer to member ::*
	  {
	    char* p;
	    if(*ptr == 'F')
		p = GetReturnType(ptr + 1, e);
	    else
		p = SkipName(ptr + 1, e);

	    if(p == nil)
		return;
	    else{
		ptr = p;
		++r;
	    }
	    break;
	  }
	default :
	    if(!ResolveTypedef(e, ptr, TRUE))
		return;
	}

    while(ResolveTypedef(e, ptr, FALSE))
	;
}

bool TypeInfo::ResolveTypedef(Environment*& e, char*& ptr, bool resolvable)
{
    Bind* bind;
    int len;
    Class* c;
    Environment* orig_e = e;
    char* name = Encoding::GetBaseName(ptr, len, e);
    if(name != nil && e != nil && e->LookupType(name, len, bind))
	switch(bind->What()){
	case Bind::isTypedefName :
	    ptr = bind->GetEncodedType();
	    return TRUE;
	case Bind::isClassName :
	    c = bind->ClassMetaobject();
	    if(c == nil)
		Set(ptr, orig_e);
	    else if (*name == 'T')
		Set(ptr, c->GetEnvironment());
	    else
		Set(c);

	    return FALSE;
	case Bind::isTemplateClass :
	    c = bind->ClassMetaobject();
	    if(c == nil)
		Set(ptr, orig_e);
	    else
		Set(ptr, c->GetEnvironment());

	    return FALSE;
	default :
	    break;
	}

    if(resolvable)
	Unknown();
    else
	Set(ptr, orig_e);

    return FALSE;
}

char* TypeInfo::SkipCv(char* ptr, Environment*& e)
{
    if(ptr == nil)
	return nil;

    for(;;)
	switch(*ptr){
	case 'C' :	// const
	case 'V' :	// volatile
	    ++ptr;
	    break;
	default :
	    goto finish;
	}

finish:
    for(;;){
	Bind* bind;
	int len;
	char* name = Encoding::GetBaseName(ptr, len, e);
	if(name != nil && e != nil && e->LookupType(name, len, bind))
	    if(bind->What() != Bind::isTypedefName)
		return ptr;
	    else
		ptr = bind->GetEncodedType();
	else
	    return ptr;
    }
}

char* TypeInfo::SkipName(char* encode, Environment* e)
{
    if(e == nil)
	MopErrorMessage("TypeInfo::SkipName()", "nil environment");

    int len;
    Environment* e2 = e;
    char* ptr = Encoding::GetBaseName(encode, len, e2);
    if(ptr == nil)
	return nil;
    else
	return &ptr[len];
}

char* TypeInfo::GetReturnType(char* encode, Environment* env)
{
    for(;;)
	switch(*encode){
	case '_' :
	    return encode + 1;
	case '\0' :
	    return nil;
	default :
	    encode = SkipType(encode, env);
	    break;
	}
}

char* TypeInfo::SkipType(char* ptr, Environment* env)
{
    while(ptr != nil)
	switch(*ptr){
	case '\0' :
	case '_' :
	    return nil;
	case 'S' :
	case 'U' :
	case 'C' :
	case 'V' :
	case 'P' :
	case 'R' :
	case 'A' :
	    ++ptr;
	    break;
	case 'F' :
	    ptr =  GetReturnType(ptr + 1, env);
	    break;
	case 'T' :
	case 'Q' :
	    return SkipName(ptr, env);
	case 'M' :
	    ptr = SkipName(ptr + 1, env);
	    break;
	default :
	    if(*(unsigned char*)ptr < 0x80)
		return ptr + 1;
	    else
		return SkipName(ptr, env);
	}

    return ptr;
}
