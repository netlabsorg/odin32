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

#ifndef _member_h
#define _member_h

#include "types.h"

class Ptree;
class PtreeArray;
class Class;
class Environment;
class TypeInfo;

class OCXXMOP Member : public LightObject {
public:
    Member();
    Member(const Member&);
    Member(Class*, Ptree*);
    void Set(Class*, Ptree*, int);

    void Signature(TypeInfo& t) const;
    Ptree* Name();
    Ptree* Comments();
    int Nth();
    Class* Supplier();
    bool IsConstructor();
    bool IsDestructor();
    bool IsFunction();
    bool IsPublic();
    bool IsProtected();
    bool IsPrivate();
    bool IsStatic();
    bool IsMutable();
    bool IsInline();
    bool IsVirtual();
    bool IsPureVirtual();

    Ptree* GetUserMemberModifier();
    Ptree* GetUserAccessSpecifier();
    bool GetUserArgumentModifiers(PtreeArray& result);

    void Remove() { removed = TRUE; }
    void SetName(Ptree*);
    void SetQualifiedName(Ptree*);
    Ptree* NewName() { return new_name; }
    Ptree* ArgumentList();
    void SetArgumentList(Ptree*);
    Ptree* NewArgumentList() { return new_args; }
    Ptree* MemberInitializers();
    void SetMemberInitializers(Ptree*);
    Ptree* NewMemberInitializers() { return new_init; }
    Ptree* FunctionBody();
    void SetFunctionBody(Ptree*);
    Ptree* NewFunctionBody() { return new_body; }

    Ptree* Arguments();

    static void Copy(Member*, void* /* ChangedMemberList::Cmem* */);

protected:
    bool IsInlineFuncImpl();
    void SetName(Ptree*, Ptree*);
    Ptree* ArgumentList(Ptree* decl);
    Ptree* Arguments(Ptree*, int);
    Ptree* MemberInitializers(Ptree* decl);

private:
    char* Name(int&);
    bool Find();

protected:

    // The next function is TRUE if the member is a function
    // with the implementation but it is not inlined.  And if so,
    // the following variables are effective.

    bool IsFunctionImplementation() { return bool(implementation != nil); }
    Ptree *implementation;
    Ptree* original_decl;

private:
    bool removed;
    Ptree* new_name;
    Ptree* new_args;
    Ptree* new_init;
    Ptree* new_body;
    bool arg_name_filled;

    Class* metaobject;
    Ptree* declarator;
    int nth;

friend class ChangedMemberList;
};

class OCXXMOP MemberFunction : public Member {
public:
    MemberFunction(Class*, Ptree*, Ptree*);
};

class OCXXMOP MemberList : public LightObject {
public:
    struct Mem {
	Class* supplying;
	Ptree* definition;
	Ptree* declarator;
	char* name;
	char* signature;
	bool is_constructor, is_destructor;
	bool is_virtual, is_static, is_mutable, is_inline;
	int  access;
	Ptree* user_access;
	Ptree* user_mod;
    };

    MemberList();
    void Make(Class*);
    Mem* Ref(int);
    int Number() { return num; }
    Mem* Lookup(char*, char*);
    int Lookup(char*, int, char*);
    int Lookup(Environment*, Ptree*, int);
    int Lookup(Environment*, char*, int);

private:
    void AppendThisClass(Class*);
    void Append(Ptree*, Ptree*, int, Ptree*);
    void AppendBaseClass(Environment*, Ptree*);
    void CheckHeader(Ptree*, Mem*);

    Class* this_class;
    int num;

    int size;
    Mem* array;
};

class ChangedMemberList : public LightObject {
public:
    struct Cmem {
	Ptree* declarator;
	bool removed;
	Ptree* name;
	Ptree* args;
	Ptree* init;
	Ptree* body;
	Ptree* def;
	int    access;		// used only by Classs::appended_member_list
	bool arg_name_filled;
    };

    ChangedMemberList();
    void Append(Member*, int access);
    static void Copy(Member* src, Cmem* dest, int access);
    Cmem* Lookup(Ptree* decl);
    Cmem* Get(int);

private:
    Cmem* Ref(int);

private:
    int num;
    int size;
    Cmem* array;
};

#endif // _member_h
