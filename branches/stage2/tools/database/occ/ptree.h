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

#ifndef _ptree_h
#define _ptree_h

#include "ptree-core.h"
#include "token.h"
#include "buffer.h"

class Encoding;

class Leaf : public Ptree {
public:
    Leaf(char*, int);
    Leaf(Token&);
    bool IsLeaf();

    void Print(ostream&, int, int);
    int Write(ostream&, int);
    void WritePS(ProgramString&);
};

class LeafName : public Leaf {
public:
    LeafName(Token&);
    Ptree* Translate(Walker*);
    void Typeof(Walker*, TypeInfo&);
};

// class DupLeaf is used by Ptree::Make() and QuoteClass (qMake()).
// The string given to the constructors are duplicated.

class DupLeaf : public Leaf {
public:
    DupLeaf(char*, int);
    DupLeaf(char*, int, char*, int);

    void Print(ostream&, int, int);
};

class LeafReserved : public Leaf {
public:
    LeafReserved(Token& t) : Leaf(t) {}
    LeafReserved(char* str, int len) : Leaf(str, len) {}
};

class LeafThis : public LeafReserved {
public:
    LeafThis(Token& t) : LeafReserved(t) {}
    int What();
    Ptree* Translate(Walker*);
    void Typeof(Walker*, TypeInfo&);
};

#define ResearvedWordDecl(w) \
class Leaf##w : public LeafReserved { \
public: \
    Leaf##w(Token& t) : LeafReserved(t) {} \
    Leaf##w(char* str, int len) : LeafReserved(str, len) {} \
    int What(); \
}

ResearvedWordDecl(AUTO);
ResearvedWordDecl(BOOLEAN);
ResearvedWordDecl(CHAR);
ResearvedWordDecl(CONST);
ResearvedWordDecl(DOUBLE);
ResearvedWordDecl(EXTERN);
ResearvedWordDecl(FLOAT);
ResearvedWordDecl(FRIEND);
ResearvedWordDecl(INLINE);
ResearvedWordDecl(INT);
ResearvedWordDecl(LONG);
ResearvedWordDecl(MUTABLE);
ResearvedWordDecl(NAMESPACE);
ResearvedWordDecl(PRIVATE);
ResearvedWordDecl(PROTECTED);
ResearvedWordDecl(PUBLIC);
ResearvedWordDecl(REGISTER);
ResearvedWordDecl(SHORT);
ResearvedWordDecl(SIGNED);
ResearvedWordDecl(STATIC);
ResearvedWordDecl(UNSIGNED);
ResearvedWordDecl(USING);
ResearvedWordDecl(VIRTUAL);
ResearvedWordDecl(VOID);
ResearvedWordDecl(VOLATILE);

ResearvedWordDecl(UserKeyword2);

#undef ResearvedWordDecl

class NonLeaf : public Ptree {
public:
    NonLeaf(Ptree*, Ptree*);
    bool IsLeaf();

    void Print(ostream&, int, int);
    int Write(ostream&, int);
    void PrintWithEncodeds(ostream&, int, int);
    void WritePS(ProgramString&);

protected:
    bool TooDeep(ostream&, int);
};

class PtreeBrace : public NonLeaf {
public:
    PtreeBrace(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    PtreeBrace(Ptree* ob, Ptree* body, Ptree* cb)
	: NonLeaf(ob, Ptree::List(body, cb)) {}

    void Print(ostream&, int, int);
    int Write(ostream&, int);

    Ptree* Translate(Walker*);
};

class PtreeBlock : public PtreeBrace {
public:
    PtreeBlock(Ptree* p, Ptree* q) : PtreeBrace(p, q) {}
    PtreeBlock(Ptree* ob, Ptree* bdy, Ptree* cb) : PtreeBrace(ob, bdy, cb) {}

    Ptree* Translate(Walker*);
};

class PtreeClassBody : public PtreeBrace {
public:
    PtreeClassBody(Ptree* p, Ptree* q) : PtreeBrace(p, q) {}
    PtreeClassBody(Ptree* ob, Ptree* bdy, Ptree* cb)
						: PtreeBrace(ob, bdy, cb) {}

    Ptree* Translate(Walker*);
};

class PtreeTypedef : public NonLeaf {
public:
    PtreeTypedef(Ptree* p) : NonLeaf(p, nil) {}
    PtreeTypedef(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeTemplateDecl : public NonLeaf {
public:
    PtreeTemplateDecl(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    PtreeTemplateDecl(Ptree* p) : NonLeaf(p, nil) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeTemplateInstantiation : public NonLeaf {
public:
    PtreeTemplateInstantiation(Ptree* p) : NonLeaf(p, nil) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeExternTemplate : public NonLeaf {
public:
    PtreeExternTemplate(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    PtreeExternTemplate(Ptree* p) : NonLeaf(p, nil) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeMetaclassDecl : public NonLeaf {
public:
    PtreeMetaclassDecl(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeLinkageSpec : public NonLeaf {
public:
    PtreeLinkageSpec(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeNamespaceSpec : public NonLeaf {
public:
    PtreeNamespaceSpec(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeUsing : public NonLeaf {
public:
    PtreeUsing(Ptree* p, Ptree* q, Ptree* r, Encoding& e, Ptree* s);
    int What();
    Ptree* Translate(Walker*);

    /* the encoded name of the symbol or namespace specified
     * by this using declaration.
     */
    char* GetEncodedName();

    /* true if this using declaration specifies a namespace.
     */
    bool isNamespace() { return bool(Cadr() != nil); }

private:
    char* name;
};

class PtreeDeclaration : public NonLeaf {
public:
    PtreeDeclaration(Ptree* p, Ptree* q) : NonLeaf(p, q) {}
    int What();
    Ptree* Translate(Walker*);
};

class PtreeDeclarator : public NonLeaf {
public:
    PtreeDeclarator(Ptree*, Encoding&, Encoding&, Ptree*);
    PtreeDeclarator(Encoding&, Encoding&, Ptree*);
    PtreeDeclarator(Ptree*, Ptree*, Encoding&, Encoding&, Ptree*);
    PtreeDeclarator(Ptree*, Encoding&);
    PtreeDeclarator(Encoding&);
    PtreeDeclarator(PtreeDeclarator*, Ptree*, Ptree*);

    int What();
    char* GetEncodedType();
    char* GetEncodedName();
    Ptree* Name() { return declared_name; }
    void Print(ostream&, int, int);

    Ptree* GetComments() { return comments; }
    void SetComments(Ptree* c) { comments = c; }

private:
    char* type;
    char* name;
    Ptree* declared_name;
    Ptree* comments;
};

class PtreeName : public NonLeaf {
public:
    PtreeName(Ptree*, Encoding&);
    int What();
    char* GetEncodedName();
    void Print(ostream&, int, int);
    Ptree* Translate(Walker*);
    void Typeof(Walker*, TypeInfo&);

private:
    char* name;
};

class PtreeFstyleCastExpr : public NonLeaf {
public:
    PtreeFstyleCastExpr(Encoding&, Ptree*, Ptree*);
    PtreeFstyleCastExpr(char*, Ptree*, Ptree*);
    int What();
    char* GetEncodedType();
    void Print(ostream&, int, int);
    Ptree* Translate(Walker*);
    void Typeof(Walker*, TypeInfo&);

private:
    char* type;
};

class PtreeClassSpec : public NonLeaf {
public:
    PtreeClassSpec(Ptree*, Ptree*, Ptree*);
    PtreeClassSpec(Ptree*, Ptree*, Ptree*, char*);
    int What();
    Ptree* Translate(Walker*);
    char* GetEncodedName();
    Ptree* GetComments();

private:
    char* encoded_name;
    Ptree* comments;

friend class Parser;
};

class PtreeEnumSpec : public NonLeaf {
public:
    PtreeEnumSpec(Ptree*);
    int What();
    Ptree* Translate(Walker*);
    char* GetEncodedName();

private:
    char* encoded_name;

friend class Parser;
};

class PtreeAccessSpec : public NonLeaf {
public:
    PtreeAccessSpec(Ptree*, Ptree*);
    int What();
    Ptree* Translate(Walker*);
};

class PtreeAccessDecl : public NonLeaf {
public:
    PtreeAccessDecl(Ptree*, Ptree*);
    int What();
    Ptree* Translate(Walker*);
};

class PtreeUserAccessSpec : public NonLeaf {
public:
    PtreeUserAccessSpec(Ptree*, Ptree*);
    int What();
    Ptree* Translate(Walker*);
};

class PtreeUserdefKeyword : public NonLeaf {
public:
    PtreeUserdefKeyword(Ptree*, Ptree*);
    int What();
};

#define PtreeStatementDecl(s)\
class Ptree##s##Statement : public NonLeaf {\
public:\
    Ptree##s##Statement(Ptree* p, Ptree* q) : NonLeaf(p, q) {}\
    int What();\
    Ptree* Translate(Walker*);\
}

PtreeStatementDecl(If);
PtreeStatementDecl(Switch);
PtreeStatementDecl(While);
PtreeStatementDecl(Do);
PtreeStatementDecl(For);
PtreeStatementDecl(Try);
PtreeStatementDecl(Break);
PtreeStatementDecl(Continue);
PtreeStatementDecl(Return);
PtreeStatementDecl(Goto);
PtreeStatementDecl(Case);
PtreeStatementDecl(Default);
PtreeStatementDecl(Label);
PtreeStatementDecl(Expr);

#undef PtreeStatementDecl

#define PtreeExprDecl(n)\
class Ptree##n##Expr : public NonLeaf {\
public:\
    Ptree##n##Expr(Ptree* p, Ptree* q) : NonLeaf(p, q) {}\
    int What();\
    Ptree* Translate(Walker*);\
    void Typeof(Walker*, TypeInfo&);\
}

PtreeExprDecl(Comma);
PtreeExprDecl(Assign);
PtreeExprDecl(Cond);
PtreeExprDecl(Infix);
PtreeExprDecl(Pm);
PtreeExprDecl(Cast);
PtreeExprDecl(Unary);
PtreeExprDecl(Throw);
PtreeExprDecl(Sizeof);
PtreeExprDecl(New);
PtreeExprDecl(Delete);
PtreeExprDecl(Array);
PtreeExprDecl(Funcall);
PtreeExprDecl(Postfix);
PtreeExprDecl(UserStatement);
PtreeExprDecl(DotMember);
PtreeExprDecl(ArrowMember);
PtreeExprDecl(Paren);
PtreeExprDecl(StaticUserStatement);

#undef PtreeExprDecl

#endif /* _ptree_h */
