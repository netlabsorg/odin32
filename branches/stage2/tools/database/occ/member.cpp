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

#include <string.h>
#include "member.h"
#include "mop.h"
#include "token.h"
#include "env.h"
#include "walker.h"
#include "encoding.h"
#include "ptree.h"

// class Member

Member::Member()
{
    metaobject = nil;
    declarator = nil;
    nth = -1;
    removed = FALSE;
    new_name = nil;
    new_args = nil;
    new_init = nil;
    new_body = nil;
    arg_name_filled = FALSE;

    implementation = nil;
    original_decl = nil;
}

Member::Member(const Member& m)
{
    metaobject = m.metaobject;
    declarator = m.declarator;
    nth = m.nth;
    removed = m.removed;
    new_name = m.new_name;
    new_args = m.new_args;
    new_init = m.new_init;
    new_body = m.new_body;
    arg_name_filled = m.arg_name_filled;

    implementation = m.implementation;
    original_decl = m.original_decl;
}

Member::Member(Class* c, Ptree* decl)
{
    metaobject = c;
    declarator = decl;
    nth = -1;
    removed = FALSE;
    new_name = nil;
    new_args = nil;
    new_init = nil;
    new_body = nil;
    arg_name_filled = FALSE;

    implementation = nil;
    original_decl = nil;
}

void Member::Set(Class* c, Ptree* decl, int n)
{
    metaobject = c;
    declarator = decl;
    nth = n;
    removed = FALSE;
    new_name = nil;
    new_args = nil;
    new_init = nil;
    new_body = nil;
    arg_name_filled = FALSE;

    implementation = nil;
    original_decl = nil;
}

void Member::Signature(TypeInfo& t) const
{
    if(declarator == nil){
	MopErrorMessage("Member::Signature()", "not initialized object.");
	return;
    }

    char* type = declarator->GetEncodedType();
    if(type == nil)
	t.Unknown();
    else
	t.Set(type, metaobject->GetEnvironment());
}

Ptree* Member::Name()
{
    int len;
    char* name = Name(len);
    return Encoding::NameToPtree(name, len);
}

char* Member::Name(int& len)
{
    if(declarator == nil){
	MopErrorMessage("Member::Name()", "not initialized object.");
	return nil;
    }

    char* name = declarator->GetEncodedName();
    if(name != nil){
	Environment* e = metaobject->GetEnvironment();
	name = Encoding::GetBaseName(name, len, e);
    }

    return name;
}

Ptree* Member::Comments()
{
    if(declarator == nil){
	MopErrorMessage("Member::Comments()", "not initialized object.");
	return nil;
    }

    if (declarator->IsA(ntDeclarator))
	return ((PtreeDeclarator*)declarator)->GetComments();
    else
	return nil;
}

int Member::Nth()
{
    if(Find())
	return nth;
    else
	return -1;
}

Class* Member::Supplier()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->supplying;
    else
	return nil;
}

bool Member::IsConstructor()
{
    if(declarator == nil){
	MopErrorMessage("Member::IsConstructor()", "not initialized object.");
	return FALSE;
    }

    char* name = declarator->GetEncodedName();
    if(name != nil){
	int len;
	Environment* e = metaobject->GetEnvironment();
	name = Encoding::GetBaseName(name, len, e);
	if(name != nil) {
	    Class* sup = Supplier();
	    if (sup != nil)
		return sup->Name()->Eq(name, len);
	}
    }

    return FALSE;
}

bool Member::IsDestructor()
{
    if(declarator == nil){
	MopErrorMessage("Member::IsDestructor()", "not initialized object.");
	return FALSE;
    }

    char* name = declarator->GetEncodedName();
    if(name != nil){
	int len;
	Environment* e = metaobject->GetEnvironment();
	name = Encoding::GetBaseName(name, len, e);
	if(name != nil)
	    return bool(*name == '~');
    }

    return FALSE;
}

bool Member::IsFunction()
{
    TypeInfo t;
    Signature(t);
    return t.IsFunction();
}

bool Member::IsPublic()
{
    if(Find())
	return bool(metaobject->GetMemberList()->Ref(nth)->access
		    == PUBLIC);
    else
	return FALSE;
}

bool Member::IsProtected()
{
    if(Find())
	return bool(metaobject->GetMemberList()->Ref(nth)->access
		    == PROTECTED);
    else
	return FALSE;
}

bool Member::IsPrivate()
{
    if(Find())
	return bool(metaobject->GetMemberList()->Ref(nth)->access
		    == PRIVATE);
    else
	return FALSE;
}

bool Member::IsStatic()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->is_static;
    else
	return FALSE;
}

bool Member::IsMutable()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->is_mutable;
    else
	return FALSE;
}

bool Member::IsInline()
{
    if(Find() && metaobject->GetMemberList()->Ref(nth)->is_inline)
	return TRUE;

    if(IsFunctionImplementation())
	return IsInlineFuncImpl();
    else
	return FALSE;
}

bool Member::IsInlineFuncImpl()
{
    Ptree* header = implementation->Car();
    while(header != nil){
	Ptree* h = header->Car();
	if(h->IsA(INLINE))
	    return TRUE;

	header = header->Cdr();
    }

    return FALSE;
}

bool Member::IsVirtual()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->is_virtual;
    else
	return FALSE;
}

bool Member::IsPureVirtual()
{
    if(IsFunction())
	return declarator->Last()->Car()->Eq('0');
    else
	return FALSE;
}

Ptree* Member::GetUserAccessSpecifier()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->user_access;
    else
	return nil;
}

bool Member::GetUserArgumentModifiers(PtreeArray& mods)
{
    Ptree* args;

    mods.Clear();
    if(!Find())
	return FALSE;

    if(!Walker::GetArgDeclList((PtreeDeclarator*)declarator, args))
	return FALSE;

    while(args != nil){
	Ptree* a = args->Car();
	if(!a->IsLeaf() && a->Car()->IsA(ntUserdefKeyword))
	    mods.Append(a->Car());
	else
	    mods.Append(nil);
	
	args = Ptree::ListTail(args, 2);	// skip ,
    }

    return TRUE;
}

Ptree* Member::GetUserMemberModifier()
{
    if(Find())
	return metaobject->GetMemberList()->Ref(nth)->user_mod;
    else
	return nil;
}

bool Member::Find()
{
    if(nth >= 0)
	return TRUE;
    else if(metaobject == nil || declarator == nil)
	return FALSE;

    MemberList* mlist = metaobject->GetMemberList();

    int len;
    char* name = Name(len);
    char* sig = declarator->GetEncodedType();
    if(mlist == nil || name == nil || sig == nil)
	return FALSE;

    nth = mlist->Lookup(name, len, sig);
    if(nth >= 0){
	MemberList::Mem* m = mlist->Ref(nth);
	metaobject = m->supplying;
	declarator = m->declarator;
	return TRUE;
    }
    else
	return FALSE;
}

void Member::SetQualifiedName(Ptree* name)
{
    new_name = name;
}

void Member::SetName(Ptree* name)
{
    if(IsFunctionImplementation())
	SetName(name, original_decl);
    else
	SetName(name, declarator);
}

void Member::SetName(Ptree* name, Ptree* decl)
{
    if(decl == nil){
	MopErrorMessage("Member::SetName()", "not initialized object.");
	return;
    }

    char* encoded = decl->GetEncodedName();
    if(encoded != nil && *encoded == 'Q'){
	Ptree* qname = ((PtreeDeclarator*)decl)->Name();
	Ptree* old_name = qname->Last()->First();
	new_name = Ptree::ShallowSubst(name, old_name, qname);
    }
    else
	new_name = name;
}

Ptree* Member::ArgumentList()
{
    if(IsFunctionImplementation())
	return ArgumentList(original_decl);
    else
	return ArgumentList(declarator);
}

Ptree* Member::ArgumentList(Ptree* decl)
{
    Ptree* args;
    if(Walker::GetArgDeclList((PtreeDeclarator*)decl, args))
	return args;
    else
	return nil;
}

void Member::SetArgumentList(Ptree* args)
{
    new_args = args;
}

Ptree* Member::MemberInitializers()
{
    if(IsFunctionImplementation())
	return MemberInitializers(original_decl);
    else
	return MemberInitializers(declarator);
}

Ptree* Member::MemberInitializers(Ptree* decl)
{
    if(IsConstructor()){
	Ptree* init = decl->Last()->Car();
	if(!init->IsLeaf() && init->Car()->Eq(':'))
	    return init;
    }

    return nil;
}

void Member::SetMemberInitializers(Ptree* init)
{
    new_init = init;
}

Ptree* Member::FunctionBody()
{
    if(IsFunctionImplementation())
	return implementation->Nth(3);
    else if(Find()){
	Ptree* def = metaobject->GetMemberList()->Ref(nth)->definition;
	Ptree* decls = def->Third();
	if(decls->IsA(ntDeclarator))
	    return def->Nth(3);
    }

    return nil;
}

void Member::SetFunctionBody(Ptree* body)
{
    new_body = body;
}

Ptree* Member::Arguments()
{
    return Arguments(ArgumentList(), 0);
}

Ptree* Member::Arguments(Ptree* args, int i)
{
    Ptree* rest;

    if(args == nil)
	return args;

    if(args->Cdr() == nil)
	rest = nil;
    else{
	rest = Arguments(args->Cddr(), i + 1);	// skip ","
	rest = Ptree::Cons(args->Cadr(), rest);
    }

    Ptree* a = args->Car();
    Ptree* p;
    if(a->IsLeaf())
	p = a;
    else{
	if(a->Car()->IsA(ntUserdefKeyword, REGISTER))
	    p = a->Third();
	else
	    p = a->Second();

	p = ((PtreeDeclarator*)p)->Name();
    }

    if(p == nil){
	arg_name_filled = TRUE;
	p = Ptree::Make(Walker::argument_name, i);
    }

    return Ptree::Cons(p, rest);
}

void Member::Copy(Member* mem, void* ptr)
{
    ChangedMemberList::Cmem* cmem = (ChangedMemberList::Cmem*)ptr;
    ChangedMemberList::Copy(mem, cmem, Class::Undefined);
    if(mem->IsFunctionImplementation()){
	cmem->declarator = mem->original_decl;
	cmem->def = mem->implementation;
    }
}


// class MemberFunction

MemberFunction::MemberFunction(Class* c, Ptree* impl, Ptree* decl)
: Member(c, decl)
{
    implementation = impl;
    original_decl = decl;
}


// class MemberList

MemberList::MemberList()
{
    this_class = nil;
    num = 0;
    size = -1;
    array = nil;
}

MemberList::Mem* MemberList::Ref(int i)
{
    const int unit = 16;
    if(i >= size){
	int old_size = size;
	size = ((unsigned int)i + unit) & ~(unit - 1);
	Mem* a = new (GC) Mem[size];
	if(old_size > 0)
	    memmove(a, array, old_size * sizeof(Mem));

	array = a;
    }

    return &array[i];
}

void MemberList::Make(Class* metaobject)
{
    this_class = metaobject;
    num = 0;

    AppendThisClass(metaobject);

    Environment* env = metaobject->GetEnvironment();
    Ptree* bases = metaobject->BaseClasses();
    while(bases != nil){
	bases = bases->Cdr();		// skip : or ,
	if(bases != nil){
	    AppendBaseClass(env, bases->Car());
	    bases = bases->Cdr();
	}
    }
}

void MemberList::AppendThisClass(Class* metaobject)
{
    int access = PRIVATE;
    Ptree* user_access = nil;
    Ptree* members = metaobject->Members();
    while(members != nil){
	Ptree* def = members->Car();
	if(def->IsA(ntDeclaration)){
	    Ptree* decl;
	    int nth = 0;
	    do{
		int i = nth++;
		decl = Walker::NthDeclarator(def, i);
		if(decl != nil)
		    Append(def, decl, access, user_access);
	    } while(decl != nil);
	}
	else if(def->IsA(ntAccessSpec)){
	    access = def->Car()->What();
	    user_access = nil;
	}
	else if(def->IsA(ntUserAccessSpec))
	    user_access = def;
	else if(def->IsA(ntAccessDecl))
	    /* not implemented */;

	members = members->Cdr();
    }
}

void MemberList::Append(Ptree* declaration, Ptree* decl,
			int access, Ptree* user_access)
{
    int len;
    Mem mem;
    char* name = decl->GetEncodedName();
    char* signature = decl->GetEncodedType();
    Environment* e = this_class->GetEnvironment();
    name = Encoding::GetBaseName(name, len, e);

    CheckHeader(declaration, &mem);

    Mem* m = Ref(num++);
    m->supplying = this_class;
    m->definition = declaration;
    m->declarator = decl;
    m->name = name;
    m->signature = signature;
    m->is_constructor = bool(this_class->Name()->Eq(name, len));
    m->is_destructor = bool(*name == '~');
    m->is_virtual = mem.is_virtual;
    m->is_static = mem.is_static;
    m->is_mutable = mem.is_mutable;
    m->is_inline = mem.is_inline;
    m->user_mod = mem.user_mod;
    m->access = access;
    m->user_access = user_access;
}

void MemberList::AppendBaseClass(Environment* env, Ptree* base_class)
{
    int access = PRIVATE;
    while(base_class->Cdr() != nil){
	Ptree* p = base_class->Car();
	if(p->IsA(PUBLIC, PROTECTED, PRIVATE))
	    access = p->What();

	base_class = base_class->Cdr();
    }	

    Class* metaobject = env->LookupClassMetaobject(base_class->Car());
    if(metaobject == nil)
	return;

    MemberList* mlist = metaobject->GetMemberList();
    for(int i = 0; i < mlist->num; ++i){
	Mem* m = &mlist->array[i];
	Mem* m2 = Lookup(m->name, m->signature);
	if(m2 != nil){				// overwrittten
	    if(!m2->is_virtual)
		m2->is_virtual = m->is_virtual;
	}
	else if(m->access != PRIVATE){		// inherited
	    m2 = Ref(num++);
	    *m2 = *m;
	    if(access == PRIVATE)
		m2->access = PRIVATE;
	    else if(access == PROTECTED)
		m2->access = PROTECTED;
	}
    }
}

MemberList::Mem* MemberList::Lookup(char* name, char* signature)
{
    for(int i = 0; i < num; ++i){
	Mem* m = Ref(i);
	if(strcmp(m->name, name) == 0 && strcmp(m->signature, signature) == 0)
	    return m;
    }

    return nil;
}

int MemberList::Lookup(char* name, int len, char* signature)
{
    for(int i = 0; i < num; ++i){
	Mem* m = Ref(i);
	if(strcmp(m->signature, signature) == 0
	   && strncmp(m->name, name, len) == 0 && m->name[len] == '\0')
	    return i;
    }

    return -1;
}

int MemberList::Lookup(Environment* env, Ptree* member, int index)
{
    char* name;
    int len;

    if(member == nil)
	return -1;
    else if(member->IsLeaf()){
	name = member->GetPosition();
	len = member->GetLength();
    }
    else
	name = Encoding::GetBaseName(member->GetEncodedName(), len, env);

    for(int i = 0; i < num; ++i){
	Mem* m = Ref(i);
	if(strncmp(m->name, name, len) == 0 && m->name[len] == '\0')
	    if(index-- <= 0)
		return i;
    }

    return -1;
}

int MemberList::Lookup(Environment*, char* name, int index)
{
    if(name == nil)
	return -1;

    for(int i = 0; i < num; ++i){
	Mem* m = Ref(i);
	if(strcmp(m->name, name) == 0)
	    if(index-- <= 0)
		return i;
    }

    return -1;
}

void MemberList::CheckHeader(Ptree* declaration, Mem* m)
{
    m->is_virtual = FALSE;
    m->is_static = FALSE;
    m->is_mutable = FALSE;
    m->is_inline = FALSE;
    m->user_mod = nil;

    Ptree* header = declaration->Car();
    while(header != nil){
	Ptree* h = header->Car();
	if(h->IsA(VIRTUAL))
	    m->is_virtual = TRUE;
	else if(h->IsA(STATIC))
	    m->is_static = TRUE;
	else if(h->IsA(MUTABLE))
	    m->is_mutable = TRUE;
	else if(h->IsA(INLINE))
	    m->is_inline = TRUE;
	else if(h->IsA(ntUserdefKeyword))
	    m->user_mod = h;

	header = header->Cdr();
    }

    Ptree* d = declaration->Third();
    if(d != nil && d->IsA(ntDeclarator))
	m->is_inline = TRUE;
}

// class ChangedMemberList

ChangedMemberList::ChangedMemberList()
{
    num = 0;
    size = -1;
    array = nil;
}

void ChangedMemberList::Append(Member* m, int access)
{
    Cmem* mem = Ref(num++);
    Copy(m, mem, access);
}

void ChangedMemberList::Copy(Member* src, Cmem* dest, int access)
{
    dest->declarator = src->declarator;
    dest->removed = src->removed;
    dest->name = src->new_name;
    dest->args = src->new_args;
    dest->init = src->new_init;
    dest->body = src->new_body;
    dest->arg_name_filled = src->arg_name_filled;

    if(src->Find()){
	MemberList::Mem* m = src->metaobject->GetMemberList()->Ref(src->nth);
	dest->def = m->definition;
	if(access == Class::Undefined)
	    dest->access = m->access;
	else
	    dest->access = access;
    }
    else{
	dest->def = nil;
	if(access == Class::Undefined)
	    dest->access = Class::Public;
	else
	    dest->access = access;
    }
}

ChangedMemberList::Cmem* ChangedMemberList::Lookup(Ptree* decl)
{
    for(int i = 0; i < num; ++i){
	Cmem* m = Ref(i);
	if(m->declarator == decl)
	    return m;
    }

    return nil;
}

ChangedMemberList::Cmem* ChangedMemberList::Get(int i)
{
    if(i >= num)
	return nil;
    else
	return Ref(i);
}

ChangedMemberList::Cmem* ChangedMemberList::Ref(int i)
{
    const int unit = 16;
    if(i >= size){
	int old_size = size;
	size = ((unsigned int)i + unit) & ~(unit - 1);
	Cmem* a = new (GC) Cmem[size];
	if(old_size > 0)
	    memmove(a, array, old_size * sizeof(Cmem));

	array = a;
    }

    return &array[i];
}
