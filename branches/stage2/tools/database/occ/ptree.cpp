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
#include <iostream.h>
#include "token.h"
#include "ptree.h"
#include "encoding.h"
#include "walker.h"

// class Leaf

Leaf::Leaf(char* ptr, int len)
{
    data.leaf.position = ptr;
    data.leaf.length = len;
}

Leaf::Leaf(Token& tk)
{
    data.leaf.position = tk.ptr;
    data.leaf.length = tk.len;
}

bool Leaf::IsLeaf()
{
    return TRUE;
}

void Leaf::Print(ostream& s, int, int)
{
    char* p = data.leaf.position;
    int n = data.leaf.length;

    // Recall that [, ], and @ are special characters.

    if(n < 1)
	return;
    else if(n == 1 && *p == '@'){
	s << "\\@";
	return;
    }

    char c = *p++;
    if(c == '[' || c == ']')
	s << '\\' << c;		// [ and ] at the beginning are escaped.
    else
	s << c;

    while(--n > 0)
	s << *p++;
}

int Leaf::Write(ostream& out, int indent)
{
    int n = 0;
    char* ptr = data.leaf.position;
    int len = data.leaf.length;
    while(len-- > 0){
	char c = *ptr++;
	if(c == '\n'){
	    PrintIndent(out, indent);
	    ++n;
	}
	else
	    out << c;
    }

    return n;
}

void Leaf::WritePS(ProgramString& out)
{
    char* ptr = data.leaf.position;
    int len = data.leaf.length;
    while(len-- > 0)
	out << *ptr++;
}


// class LeafName

LeafName::LeafName(Token& t) : Leaf(t) {}

Ptree* LeafName::Translate(Walker* w)
{
    return w->TranslateVariable(this);
}

void LeafName::Typeof(Walker* w, TypeInfo& t)
{
    w->TypeofVariable(this, t);
}

// class DupLeaf
// This allocates new memory to the given string.

DupLeaf::DupLeaf(char* str, int len) : Leaf(new (GC) char[len], len)
{
    memmove(data.leaf.position, str, len);
}

DupLeaf::DupLeaf(char* str1, int len1, char* str2, int len2)
: Leaf(new (GC) char[len1 + len2], len1 + len2)
{
    memmove(data.leaf.position, str1, len1);
    memmove(&data.leaf.position[len1], str2, len2);
}

void DupLeaf::Print(ostream& s, int, int)
{
    int i, j;
    char* pos;

    pos = data.leaf.position;
    j = data.leaf.length;

    if(j == 1 && *pos == '@'){
	s << "\\@";
	return;
    }

    s << '`';
    for(i = 0; i < j; ++i)
	if(pos[i] == '[' || pos[i] == ']'){
	    s << '\\';
	    s << pos[i];
	}
	else
	    s << pos[i];

    s << '`';
}

int LeafThis::What()
{
    return THIS;
}

Ptree* LeafThis::Translate(Walker* w)
{
    return w->TranslateThis(this);
}

void LeafThis::Typeof(Walker* w, TypeInfo& t)
{
    w->TypeofThis(this, t);
}

#define ResearvedWordImpl(w) \
int Leaf##w::What() { return w; }

ResearvedWordImpl(AUTO)
ResearvedWordImpl(BOOLEAN)
ResearvedWordImpl(CHAR)
ResearvedWordImpl(CONST)
ResearvedWordImpl(DOUBLE)
ResearvedWordImpl(EXTERN)
ResearvedWordImpl(FLOAT)
ResearvedWordImpl(FRIEND)
ResearvedWordImpl(INLINE)
ResearvedWordImpl(INT)
ResearvedWordImpl(LONG)
ResearvedWordImpl(MUTABLE)
ResearvedWordImpl(NAMESPACE)
ResearvedWordImpl(PRIVATE)
ResearvedWordImpl(PROTECTED)
ResearvedWordImpl(PUBLIC)
ResearvedWordImpl(REGISTER)
ResearvedWordImpl(SHORT)
ResearvedWordImpl(SIGNED)
ResearvedWordImpl(STATIC)
ResearvedWordImpl(UNSIGNED)
ResearvedWordImpl(USING)
ResearvedWordImpl(VIRTUAL)
ResearvedWordImpl(VOID)
ResearvedWordImpl(VOLATILE)

ResearvedWordImpl(UserKeyword2)

#undef ResearvedWordImpl

// class NonLeaf

NonLeaf::NonLeaf(Ptree* p, Ptree* q)
{
    data.nonleaf.child = p;
    data.nonleaf.next = q;
}

bool NonLeaf::IsLeaf()
{
    return FALSE;
}

void NonLeaf::Print(ostream& s, int indent, int depth)
{
    if(TooDeep(s, depth))
	return;

    Ptree* rest = this;
    s << '[';
    while(rest != nil){
	if(rest->IsLeaf()){
	    s << "@ ";
	    rest->Print(s, indent, depth + 1);
	    rest = nil;
	}
	else{
	    Ptree* head = rest->data.nonleaf.child;
	    if(head == nil)
		s << "nil";
	    else
		head->Print(s, indent, depth + 1);
	    rest = rest->data.nonleaf.next;
	    if(rest != nil)
		s << ' ';
	}
    }

    s << ']';
}

bool NonLeaf::TooDeep(ostream& s, int depth)
{
    if(depth >= 32){
	s << " ** too many nestings ** ";
	return TRUE;
    }
    else
	return FALSE;
}

int NonLeaf::Write(ostream& out, int indent)
{
    int n = 0;
    Ptree* p = this;
    for(;;){
	Ptree* head = p->Car();
	if(head != nil)
	    n += head->Write(out, indent);

	p = p->Cdr();
	if(p == nil)
	    break;
	else if(p->IsLeaf()){
	    MopErrorMessage("NonLeaf::Write()", "not list");
	    break;
	}
	else
	    out << ' ';
    }

    return n;
}

void NonLeaf::PrintWithEncodeds(ostream& s, int indent, int depth)
{
    char* encode = GetEncodedType();
    if(encode != nil){
	s << '#';
	Encoding::Print(s, encode);
    }

    encode = GetEncodedName();
    if(encode != nil){
	s << '@';
	Encoding::Print(s, encode);
    }

    NonLeaf::Print(s, indent, depth);
}

void NonLeaf::WritePS(ProgramString& out)
{
    Ptree* p = this;
    for(;;){
	Ptree* head = p->Car();
	if(head != nil)
	    head->WritePS(out);

	p = p->Cdr();
	if(p == nil)
	    break;
	else if(p->IsLeaf()){
	    MopErrorMessage("NonLeaf::WritePS()", "not list");
	    break;
	}
	else
	    out << ' ';
    }
}


// class PtreeBrace

void PtreeBrace::Print(ostream& s, int indent, int depth)
{
    if(TooDeep(s, depth))
	return;

    int indent2 = indent + 1;
    s << "[{";
    Ptree* body = Ptree::Second(this);
    if(body == nil){
	PrintIndent(s, indent2);
	s << "nil";
    }
    else
	while(body != nil){
	    PrintIndent(s, indent2);
	    if(body->IsLeaf()){
		s << "@ ";
		body->Print(s, indent + 1, depth + 1);
	    }
	    else{
		Ptree* head = body->Car();
		if(head == nil)
		    s << "nil";
		else
		    head->Print(s, indent + 1, depth + 1);
	    }

	    body = body->Cdr();
	}

    PrintIndent(s, indent);
    s << "}]";
}

int PtreeBrace::Write(ostream& out, int indent)
{
    int n = 0;

    out << '{';
    Ptree* p = this->Cadr();
    while(p != nil){
	if(p->IsLeaf()){
	    MopErrorMessage("PtreeBrace::Write()", "non list");
	    break;
	}
	else{
	    PrintIndent(out, indent + 1);
	    ++n;
	    Ptree* q = p->Car();
	    p = p->Cdr();
	    if(q != nil)
		n += q->Write(out, indent + 1);
	}
    }

    PrintIndent(out, indent);
    ++n;
    out << '}';
    return n;
}

Ptree* PtreeBrace::Translate(Walker* w)
{
    return w->TranslateBrace(this);
}

// class PtreeBlock

Ptree* PtreeBlock::Translate(Walker* w)
{
    return w->TranslateBlock(this);
}

// class PtreeClassBody

Ptree* PtreeClassBody::Translate(Walker* w)
{
    return w->TranslateClassBody(this, nil, nil);
}

// class PtreeTypedef

int PtreeTypedef::What()
{
    return ntTypedef;
}

Ptree* PtreeTypedef::Translate(Walker* w)
{
    return w->TranslateTypedef(this);
}

// class PtreeTemplateDecl

int PtreeTemplateDecl::What()
{
    return ntTemplateDecl;
}

Ptree* PtreeTemplateDecl::Translate(Walker* w)
{
    return w->TranslateTemplateDecl(this);
}

// class PtreeTemplateInstantiation

int PtreeTemplateInstantiation::What()
{
    return ntTemplateInstantiation;
}

Ptree* PtreeTemplateInstantiation::Translate(Walker* w)
{
    return w->TranslateTemplateInstantiation(this);
}

// class PtreeExternTemplate

int PtreeExternTemplate::What()
{
    return ntExternTemplate;
}

Ptree* PtreeExternTemplate::Translate(Walker* w)
{
    return w->TranslateExternTemplate(this);
}

// class PtreeMetaclassDecl

int PtreeMetaclassDecl::What()
{
    return ntMetaclassDecl;
}

Ptree* PtreeMetaclassDecl::Translate(Walker* w)
{
    return w->TranslateMetaclassDecl(this);
}

// class PtreeLinkageSpec

int PtreeLinkageSpec::What()
{
    return ntLinkageSpec;
}

Ptree* PtreeLinkageSpec::Translate(Walker* w)
{
    return w->TranslateLinkageSpec(this);
}

// class PtreeNamespaceSpec

int PtreeNamespaceSpec::What()
{
    return ntNamespaceSpec;
}

Ptree* PtreeNamespaceSpec::Translate(Walker* w)
{
    return w->TranslateNamespaceSpec(this);
}

// class PtreeUsing

PtreeUsing::PtreeUsing(Ptree* p, Ptree* q, Ptree* r, Encoding& e, Ptree* s)
: NonLeaf(p, Ptree::List(q, r, s))
{
    name = e.Get();
}

char* PtreeUsing::GetEncodedName()
{
  return name;
}

int PtreeUsing::What()
{
    return ntUsing;
}

Ptree* PtreeUsing::Translate(Walker* w)
{
    return w->TranslateUsing(this);
}

// class PtreeDeclaration

int PtreeDeclaration::What()
{
    return ntDeclaration;
}

Ptree* PtreeDeclaration::Translate(Walker* w)
{
    return w->TranslateDeclaration(this);
}

// class PtreeDeclarator

PtreeDeclarator::PtreeDeclarator(Ptree* list, Encoding& t, Encoding& n,
				 Ptree* dname)
: NonLeaf(list->Car(), list->Cdr())
{
    type = t.Get();
    name = n.Get();
    declared_name = dname;
    comments = nil;
}

PtreeDeclarator::PtreeDeclarator(Encoding& t, Encoding& n, Ptree* dname)
: NonLeaf(nil, nil)
{
    type = t.Get();
    name = n.Get();
    declared_name = dname;
    comments = nil;
}

PtreeDeclarator::PtreeDeclarator(Ptree* p, Ptree* q,
				 Encoding& t, Encoding& n, Ptree* dname)
: NonLeaf(p, q)
{
    type = t.Get();
    name = n.Get();
    declared_name = dname;
    comments = nil;
}

PtreeDeclarator::PtreeDeclarator(Ptree* list, Encoding& t)
: NonLeaf(list->Car(), list->Cdr())
{
    type = t.Get();
    name = nil;
    declared_name = nil;
    comments = nil;
}

PtreeDeclarator::PtreeDeclarator(Encoding& t)
: NonLeaf(nil, nil)
{
    type = t.Get();
    name = nil;
    declared_name = nil;
    comments = nil;
}

PtreeDeclarator::PtreeDeclarator(PtreeDeclarator* decl, Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
    type = decl->type;
    name = decl->name;
    declared_name = decl->declared_name;
    comments = nil;
}

int PtreeDeclarator::What()
{
    return ntDeclarator;
}

char* PtreeDeclarator::GetEncodedType()
{
    return type;
}

char* PtreeDeclarator::GetEncodedName()
{
    return name;
}

void PtreeDeclarator::Print(ostream& s, int i, int d)
{
    if(show_encoded)
	PrintWithEncodeds(s, i, d);
    else
	NonLeaf::Print(s, i, d);
}


// class PtreeName

PtreeName::PtreeName(Ptree* p, Encoding& e)
: NonLeaf(p->Car(), p->Cdr())
{
    name = e.Get();
}

int PtreeName::What()
{
    return ntName;
}

char* PtreeName::GetEncodedName()
{
    return name;
}

void PtreeName::Print(ostream& s, int i, int d)
{
    if(show_encoded)
	PrintWithEncodeds(s, i, d);
    else
	NonLeaf::Print(s, i, d);
}

Ptree* PtreeName::Translate(Walker* w)
{
    return w->TranslateVariable(this);
}

void PtreeName::Typeof(Walker* w, TypeInfo& t)
{
    w->TypeofVariable(this, t);
}

// class PtreeFstyleCastExpr

PtreeFstyleCastExpr::PtreeFstyleCastExpr(Encoding& e, Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
    type = e.Get();
}

PtreeFstyleCastExpr::PtreeFstyleCastExpr(char* e, Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
    type = e;
}

int PtreeFstyleCastExpr::What()
{
    return ntFstyleCast;
}

char* PtreeFstyleCastExpr::GetEncodedType()
{
    return type;
}

void PtreeFstyleCastExpr::Print(ostream& s, int i, int d)
{
    if(show_encoded)
	PrintWithEncodeds(s, i, d);
    else
	NonLeaf::Print(s, i, d);
}

Ptree* PtreeFstyleCastExpr::Translate(Walker* w)
{
    return w->TranslateFstyleCast(this);
}

void PtreeFstyleCastExpr::Typeof(Walker* w, TypeInfo& t)
{
    w->TypeofFstyleCast(this, t);
}

// class PtreeClassSpec

PtreeClassSpec::PtreeClassSpec(Ptree* p, Ptree* q, Ptree* c)
: NonLeaf(p, q)
{
    encoded_name = nil;
    comments = c;
}

PtreeClassSpec::PtreeClassSpec(Ptree* car, Ptree* cdr,
		Ptree* c, char* encode) : NonLeaf(car, cdr)
{
    encoded_name = encode;
    comments = c;
}

int PtreeClassSpec::What()
{
    return ntClassSpec;
}

Ptree* PtreeClassSpec::Translate(Walker* w)
{
    return w->TranslateClassSpec(this);
}

char* PtreeClassSpec::GetEncodedName()
{
    return encoded_name;
}

Ptree* PtreeClassSpec::GetComments()
{
    return comments;
}

// class PtreeEnumSpec

PtreeEnumSpec::PtreeEnumSpec(Ptree* head)
: NonLeaf(head, nil)
{
    encoded_name = nil;
}

int PtreeEnumSpec::What()
{
    return ntEnumSpec;
}

Ptree* PtreeEnumSpec::Translate(Walker* w)
{
    return w->TranslateEnumSpec(this);
}

char* PtreeEnumSpec::GetEncodedName()
{
    return encoded_name;
}

PtreeAccessSpec::PtreeAccessSpec(Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
}

int PtreeAccessSpec::What()
{
    return ntAccessSpec;
}

Ptree* PtreeAccessSpec::Translate(Walker* w)
{
    return w->TranslateAccessSpec(this);
}

PtreeAccessDecl::PtreeAccessDecl(Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
}

int PtreeAccessDecl::What()
{
    return ntAccessDecl;
}

Ptree* PtreeAccessDecl::Translate(Walker* w)
{
    return w->TranslateAccessDecl(this);
}

PtreeUserAccessSpec::PtreeUserAccessSpec(Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
}

int PtreeUserAccessSpec::What()
{
    return ntUserAccessSpec;
}

Ptree* PtreeUserAccessSpec::Translate(Walker* w)
{
    return w->TranslateUserAccessSpec(this);
}

PtreeUserdefKeyword::PtreeUserdefKeyword(Ptree* p, Ptree* q)
: NonLeaf(p, q)
{
}

int PtreeUserdefKeyword::What()
{
    return ntUserdefKeyword;
}

#define PtreeStatementImpl(s)\
int Ptree##s##Statement::What() { return nt##s##Statement; }\
Ptree* Ptree##s##Statement::Translate(Walker* w) {\
    return w->Translate##s(this); }

PtreeStatementImpl(If)
PtreeStatementImpl(Switch)
PtreeStatementImpl(While)
PtreeStatementImpl(Do)
PtreeStatementImpl(For)
PtreeStatementImpl(Try)
PtreeStatementImpl(Break)
PtreeStatementImpl(Continue)
PtreeStatementImpl(Return)
PtreeStatementImpl(Goto)
PtreeStatementImpl(Case)
PtreeStatementImpl(Default)
PtreeStatementImpl(Label)

#undef PtreeStatementImpl

int PtreeExprStatement::What()
{
    return ntExprStatement;
}

Ptree* PtreeExprStatement::Translate(Walker* w)
{
    return w->TranslateExprStatement(this);
}

#define PtreeExprImpl(n)\
int Ptree##n##Expr::What() { return nt##n##Expr; }\
Ptree* Ptree##n##Expr::Translate(Walker* w) {\
    return w->Translate##n(this); }\
void Ptree##n##Expr::Typeof(Walker* w, TypeInfo& t) {\
    w->Typeof##n(this, t); }

PtreeExprImpl(Comma)
PtreeExprImpl(Assign)
PtreeExprImpl(Cond)
PtreeExprImpl(Infix)
PtreeExprImpl(Pm)
PtreeExprImpl(Cast)
PtreeExprImpl(Unary)
PtreeExprImpl(Throw)
PtreeExprImpl(Sizeof)
PtreeExprImpl(New)
PtreeExprImpl(Delete)
PtreeExprImpl(Array)
PtreeExprImpl(Funcall)
PtreeExprImpl(Postfix)
PtreeExprImpl(UserStatement)
PtreeExprImpl(DotMember)
PtreeExprImpl(ArrowMember)
PtreeExprImpl(Paren)
PtreeExprImpl(StaticUserStatement)

#undef PtreeExprImpl
