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
   C++ Parser

   This parser is a LL(k) parser with ad hoc rules such as
   backtracking.

   r<name>() is the grammer rule for a non-terminal <name>.
   opt<name>() is the grammer fule for an optional non-terminal <name>.
   is<name>() looks ahead and returns TRUE if the next symbol is <name>.
*/

#include <iostream.h>
#include "parse.h"
#include "token.h"
#include "env.h"
#include "ptree.h"
#include "encoding.h"
#include "metaclass.h"
#include "walker.h"

#if defined(_PARSE_VCC)
#define _MSC_VER        1100
#endif

const int MaxErrors = 10;

Parser::Parser(Lex* l)
{
    lex = l;
    nerrors = 0;
}

bool Parser::ErrorMessage(const char* msg, Ptree* name, Ptree* where)
{
    if(where != nil){
        Ptree* head = where->Ca_ar();
        if(head != nil)
            ShowMessageHead(head->GetPosition());
    }

    cerr << msg;
    if(name != nil)
        name->Write(cerr);

    cerr << '\n';
    return bool(++nerrors < MaxErrors);
}

void Parser::WarningMessage(const char* msg, Ptree* name, Ptree* where)
{
    if(where != nil){
        Ptree* head = where->Ca_ar();
        if(head != nil)
            ShowMessageHead(head->GetPosition());
    }

    cerr << "warning: " << msg;
    if(name != nil)
        name->Write(cerr);

    cerr << '\n';
}

bool Parser::SyntaxError()
{
    Token t, t2;
    int i;

    lex->LookAhead(0, t);
    lex->LookAhead(1, t2);

    ShowMessageHead(t.ptr);
    cerr << "parse error before `";
    if(t.kind != '\0')
        for(i = 0; i < t.len; ++i)
            cerr << t.ptr[i];

    if(t2.kind != '\0'){
        cerr << ' ';
        for(i = 0; i < t2.len; ++i)
            cerr << t2.ptr[i];
    }

    cerr << "'\n";
    return bool(++nerrors < MaxErrors);
}

#ifdef SDS
uint Parser::LineNumber(char* pos, char*& fname, int& fname_len)
{
   int col_num, abs_len;

   return LineNumber(pos, fname, fname_len, col_num, abs_len);
}
#endif

#ifdef SDS
uint Parser::LineNumber(char* pos, char*& fname, int& fname_len, int& col_num, int& abs_len)
#else
uint Parser::LineNumber(char* pos, char*& fname, int& fname_len)
#endif
{
    #ifdef SDS
    uint line_number = lex->LineNumber(pos, fname, fname_len, col_num, abs_len);
    #else
    uint line_number = lex->LineNumber(pos, fname, fname_len);
    #endif
    if(fname_len > 1){
        if(fname[0] == '"') {
            ++fname;
            --fname_len;
        }

        if(fname[fname_len - 1] == '"')
            --fname_len;
    }

    return line_number;
}

void Parser::ShowMessageHead(char* pos)
{
    char* fname;
    int fname_len;

    uint line_number = LineNumber(pos, fname, fname_len);
    int i = 0;
    while(i < fname_len)
        cerr << fname[i++];

#if defined(_MSC_VER)
    cerr << '(' << line_number << ") : ";
#else
    cerr << ':' << line_number << ": ";
#endif
}

bool Parser::rProgram(Ptree*& def)
{
    while(lex->LookAhead(0) != '\0')
        if(rDefinition(def))
            return TRUE;
        else{
            Token tk;
            if(!SyntaxError())
                return FALSE;           // too many errors

            SkipTo(';');
            lex->GetToken(tk);  // ignore ';'
        }

    return FALSE;
}

/*
  definition
  : null.declaration
  | typedef
  | template.decl
  | metaclass.decl
  | linkage.spec
  | namespace.spec
  | using.declaration
  | extern.template.decl
  | declaration
*/
bool Parser::rDefinition(Ptree*& p)
{
    bool res;
    int t = lex->LookAhead(0);
    if(t == ';')
        res = rNullDeclaration(p);
    else if(t == TYPEDEF)
        res = rTypedef(p);
    else if(t == TEMPLATE)
        res = rTemplateDecl(p);
    else if(t == METACLASS)
        res = rMetaclassDecl(p);
    else if(t == EXTERN && lex->LookAhead(1) == StringL)
        res = rLinkageSpec(p);
    else if(t == EXTERN && lex->LookAhead(1) == TEMPLATE)
        res = rExternTemplateDecl(p);
    else if(t == NAMESPACE)
        res = rNamespaceSpec(p);
    else if(t == USING)
        res = rUsing(p);
    else {
        Ptree* c = lex->GetComments2();
        if (res = rDeclaration(p))
            Walker::SetDeclaratorComments(p, c);
    }

    lex->GetComments();
    return res;
}

bool Parser::rNullDeclaration(Ptree*& decl)
{
    Token tk;

    if(lex->GetToken(tk) != ';')
        return FALSE;

    decl = new PtreeDeclaration(nil, Ptree::List(nil, new Leaf(tk)));
    return TRUE;
}

/*
  typedef
  : TYPEDEF type.specifier declarators ';'
*/
bool Parser::rTypedef(Ptree*& def)
{
    Token tk;
    Ptree *type_name, *decl;
    Encoding type_encode;

    if(lex->GetToken(tk) != TYPEDEF)
        return FALSE;

    def = new PtreeTypedef(new LeafReserved(tk));
    if(!rTypeSpecifier(type_name, FALSE, type_encode))
        return FALSE;

    def = Ptree::Snoc(def, type_name);
    if(!rDeclarators(decl, type_encode, TRUE))
        return FALSE;

    if(lex->GetToken(tk) != ';')
        return FALSE;

    def = Ptree::Nconc(def, Ptree::List(decl, new Leaf(tk)));
    return TRUE;
}

/*
  type.specifier
  : {cv.qualify} (integral.or.class.spec | name) {cv.qualify}
*/
bool Parser::rTypeSpecifier(Ptree*& tspec, bool check, Encoding& encode)
{
    Ptree *cv_q, *cv_q2;

    if(!optCvQualify(cv_q) || !optIntegralTypeOrClassSpec(tspec, encode))
        return FALSE;

    if(tspec == nil){
        if(check){
            Token tk;
            lex->LookAhead(0, tk);
            if(!MaybeTypeNameOrClassTemplate(tk))
                return FALSE;
        }

        if(!rName(tspec, encode))
            return FALSE;
    }

    if(!optCvQualify(cv_q2))
        return FALSE;

    if(cv_q != nil){
        tspec = Ptree::Snoc(cv_q, tspec);
        if(cv_q2 != nil)
            tspec = Ptree::Nconc(tspec, cv_q2);
    }
    else if(cv_q2 != nil)
        tspec = Ptree::Cons(tspec, cv_q2);

    encode.CvQualify(cv_q, cv_q2);
    return TRUE;
}

// isTypeSpecifier() returns TRUE if the next is probably a type specifier.

bool Parser::isTypeSpecifier()
{
    int t = lex->LookAhead(0);
    if(t == Identifier || t == Scope
       ||t == CONST || t == VOLATILE
       || t == CHAR || t == INT || t == SHORT || t == LONG
       || t == SIGNED || t == UNSIGNED || t == FLOAT || t == DOUBLE
       || t == VOID || t == BOOLEAN
       || t == CLASS || t == STRUCT || t == UNION || t == ENUM
#if defined(_MSC_VER)
       || t == INT64
#endif
       )
        return TRUE;
    else
        return FALSE;
}

/*
  metaclass.decl
  : METACLASS Identifier {{':'} Identifier {'(' meta.arguments ')'}} ';'

  We allow two kinds of syntax:

  metaclass <metaclass> <class>(...);
  metaclass <metaclass>;
  metaclass <class> : <metaclass>(...);         // for backward compatibility
*/
bool Parser::rMetaclassDecl(Ptree*& decl)
{
    int t;
    Token tk1, tk2, tk3, tk4;
    Ptree* metaclass_name;

    if(lex->GetToken(tk1) != METACLASS)
        return FALSE;

    if(lex->GetToken(tk2) != Identifier)
        return FALSE;

    t = lex->GetToken(tk3);
    if(t == Identifier){
        metaclass_name = new Leaf(tk2);
        decl = new PtreeMetaclassDecl(new LeafReserved(tk1),
                                      Ptree::List(metaclass_name,
                                                  new Leaf(tk3)));
    }
    else if(t == ':'){
        if(lex->GetToken(tk4) != Identifier)
            return FALSE;

        metaclass_name = new Leaf(tk4);
        decl = new PtreeMetaclassDecl(new LeafReserved(tk1),
                                      Ptree::List(metaclass_name,
                                                  new Leaf(tk2)));
    }
    else if(t == ';'){
        metaclass_name = new Leaf(tk2);
        decl = new PtreeMetaclassDecl(new LeafReserved(tk1),
                                      Ptree::List(metaclass_name, nil,
                                                  new Leaf(tk3)));
        Metaclass::Load(metaclass_name);
        return TRUE;
    }
    else
        return FALSE;

    t = lex->GetToken(tk1);
    if(t == '('){
        Ptree* args;
        if(!rMetaArguments(args))
            return FALSE;

        if(lex->GetToken(tk2) != ')')
            return FALSE;

        decl = Ptree::Nconc(decl, Ptree::List(new Leaf(tk1), args,
                                              new Leaf(tk2)));
        t = lex->GetToken(tk1);
    }

    if(t == ';'){
        decl = Ptree::Snoc(decl, new Leaf(tk1));
        Metaclass::Load(metaclass_name);
        return TRUE;
    }
    else
        return FALSE;
}

/*
  meta.arguments : (anything but ')')*
*/
bool Parser::rMetaArguments(Ptree*& args)
{
    int t;
    Token tk;

    int n = 1;
    args = nil;
    for(;;){
        t = lex->LookAhead(0);
        if(t == '\0')
            return FALSE;
        else if(t == '(')
            ++n;
        else if(t == ')')
            if(--n <= 0)
                return TRUE;

        lex->GetToken(tk);
        args = Ptree::Snoc(args, new Leaf(tk));
    }
}

/*
  linkage.spec
  : EXTERN StringL definition
  |  EXTERN StringL linkage.body
*/
bool Parser::rLinkageSpec(Ptree*& spec)
{
    Token tk1, tk2;
    Ptree* body;

    if(lex->GetToken(tk1) != EXTERN)
        return FALSE;

    if(lex->GetToken(tk2) != StringL)
        return FALSE;

    spec = new PtreeLinkageSpec(new LeafEXTERN(tk1),
                                Ptree::List(new Leaf(tk2)));
    if(lex->LookAhead(0) == '{'){
        if(!rLinkageBody(body))
            return FALSE;
    }
    else
        if(!rDefinition(body))
            return FALSE;

    spec = Ptree::Snoc(spec, body);
    return TRUE;
}

/*
  namespace.spec
  : NAMESPACE Identifier definition
  | NAMESPACE { Identifier } linkage.body
*/
bool Parser::rNamespaceSpec(Ptree*& spec)
{
    Token tk1, tk2;
    Ptree* name;
    Ptree* body;

    if(lex->GetToken(tk1) != NAMESPACE)
        return FALSE;

    if(lex->LookAhead(0) == '{')
        name = nil;
    else
        if(lex->GetToken(tk2) == Identifier)
            name = new Leaf(tk2);
        else
            return FALSE;

    if(lex->LookAhead(0) == '{'){
        if(!rLinkageBody(body))
            return FALSE;
    }
    else
        if(!rDefinition(body))
            return FALSE;

    spec = new PtreeNamespaceSpec(new LeafNAMESPACE(tk1),
                                  Ptree::List(name, body));
    return TRUE;
}


/*
  using.declaration : USING { NAMESPACE } name ';'
*/
bool Parser::rUsing(Ptree*& decl)
{
    Token tk;
    Ptree* name;
    Encoding encode;

    if(lex->GetToken(tk) != USING)
        return FALSE;

    Ptree* using0 = new LeafUSING(tk);
    Ptree* using1;
    if (lex->LookAhead(0) != NAMESPACE)
        using1 = nil;
    else {
        lex->GetToken(tk);
        using1 = new LeafNAMESPACE(tk);
    }

    if (!rName(name, encode))
        return FALSE;

    if (lex->GetToken(tk) != ';')
        return FALSE;

    decl = new PtreeUsing(using0, using1, name, encode, new Leaf(tk));
    return TRUE;
}


/*
  linkage.body : '{' (definition)* '}'

  Note: this is also used to construct namespace.spec
*/
bool Parser::rLinkageBody(Ptree*& body)
{
    Token op, cp;
    Ptree* def;

    if(lex->GetToken(op) != '{')
        return FALSE;

    body = nil;
    while(lex->LookAhead(0) != '}'){
        if(!rDefinition(def)){
            if(!SyntaxError())
                return FALSE;           // too many errors

            SkipTo('}');
            lex->GetToken(cp);
            body = Ptree::List(new Leaf(op), nil, new Leaf(cp));
            return TRUE;                // error recovery
        }

        body = Ptree::Snoc(body, def);
    }

    lex->GetToken(cp);
    body = new PtreeBrace(new Leaf(op), body, new Leaf(cp));
    return TRUE;
}

/*
  template.decl
  : TEMPLATE '<' temp.arg.list '>' declaration
  | TEMPLATE declaration
  | TEMPLATE '<' '>' declaration

  The second case is an explicit template instantiation.  declaration must
  be a class declaration.  For example,

      template class Foo<int, char>;

  explicitly instantiates the template Foo with int and char.

  The third case is a specialization of a template function.  declaration
  must be a function template.  For example,

      template <> int count(String x) { return x.length; }
*/
bool Parser::rTemplateDecl(Ptree*& decl)
{
    Ptree *body;
    TemplateDeclKind kind = tdk_unknown;

    if(!rTemplateDecl2(decl, kind))
        return FALSE;

    if(!rDeclaration(body))
        return FALSE;

    // Repackage the decl and body depending upon what kind of template
    // declaration was observed.
    switch (kind) {
    case tdk_instantiation:
        // Repackage the decl as a PtreeTemplateInstantiation
        decl = body;
        // assumes that decl has the form: [nil [class ...] ;]
        if (Ptree::Length(decl) != 3)
            return FALSE;

        if (Ptree::First(decl) != nil)
            return FALSE;

        if (Ptree::Second(decl)->What() != ntClassSpec)
            return FALSE;

        if (!Ptree::Eq(Ptree::Third(decl), ';'))
            return FALSE;

        decl = new PtreeTemplateInstantiation(Ptree::Second(decl));
        break;
    case tdk_decl:
    case tdk_specialization:
        decl = Ptree::Snoc(decl, body);
        break;
    default:
        MopErrorMessage("rTemplateDecl()", "fatal");
        break;
    }

    return TRUE;
}

bool Parser::rTemplateDecl2(Ptree*& decl, TemplateDeclKind &kind)
{
    Token tk;
    Ptree *args;

    if(lex->GetToken(tk) != TEMPLATE)
        return FALSE;

    if(lex->LookAhead(0) != '<') {
        // template instantiation
        decl = nil;
        kind = tdk_instantiation;
        return TRUE;    // ignore TEMPLATE
    }

    decl = new PtreeTemplateDecl(new LeafReserved(tk));
    if(lex->GetToken(tk) != '<')
        return FALSE;

    decl = Ptree::Snoc(decl, new Leaf(tk));
    if(!rTempArgList(args))
        return FALSE;

    if(lex->GetToken(tk) != '>')
        return FALSE;

    decl = Ptree::Nconc(decl, Ptree::List(args, new Leaf(tk)));

    // ignore nested TEMPLATE
    while (lex->LookAhead(0) == TEMPLATE) {
        lex->GetToken(tk);
        if(lex->LookAhead(0) != '<')
            break;

        lex->GetToken(tk);
        if(!rTempArgList(args))
            return FALSE;

        if(lex->GetToken(tk) != '>')
            return FALSE;
    }

    if (args == nil)
        // template < > declaration
        kind = tdk_specialization;
    else
        // template < ... > declaration
        kind = tdk_decl;

    return TRUE;
}

/*
  temp.arg.list
  : empty
  | temp.arg.declaration (',' temp.arg.declaration)*
*/
bool Parser::rTempArgList(Ptree*& args)
{
    Token tk;
    Ptree* a;

    if(lex->LookAhead(0) == '>'){
        args = nil;
        return TRUE;
    }

    if(!rTempArgDeclaration(a))
        return FALSE;

    args = Ptree::List(a);
    while(lex->LookAhead(0) == ','){
        lex->GetToken(tk);
        args = Ptree::Snoc(args, new Leaf(tk));
        if(!rTempArgDeclaration(a))
            return FALSE;

        args = Ptree::Snoc(args, a);
    }

    return TRUE;
}

/*
  temp.arg.declaration
  : CLASS Identifier {'=' type.name}
  | type.specifier arg.declarator {'=' additive.expr}
  | template.decl2 CLASS Identifier {'=' type.name}
*/
bool Parser::rTempArgDeclaration(Ptree*& decl)
{
    Token tk1, tk2;

    int t0 = lex->LookAhead(0);
    if(t0 == CLASS && lex->LookAhead(1) == Identifier){
        lex->GetToken(tk1);
        lex->GetToken(tk2);
        Ptree* name = new Leaf(tk2);
        decl = Ptree::List(new Leaf(tk1), name);

        if(lex->LookAhead(0) == '='){
            Ptree* default_type;

            lex->GetToken(tk1);
            if(!rTypeName(default_type))
                return FALSE;

            decl = Ptree::Nconc(decl, Ptree::List(new Leaf(tk1),
                                                  default_type));
        }
    }
    else if (t0 == TEMPLATE) {
        TemplateDeclKind kind;
        if(!rTemplateDecl2(decl, kind))
            return FALSE;

        if (lex->GetToken(tk1) != CLASS || lex->GetToken(tk2) != Identifier)
            return FALSE;

        Ptree* cspec = new PtreeClassSpec(new LeafReserved(tk1),
                                          Ptree::Cons(new Leaf(tk2),nil),
                                          nil);
        decl = Ptree::Snoc(decl, cspec);
        if(lex->LookAhead(0) == '='){
            Ptree* default_type;
            lex->GetToken(tk1);
            if(!rTypeName(default_type))
                return FALSE;

            decl = Ptree::Nconc(decl, Ptree::List(new Leaf(tk1),
                                                  default_type));
        }
    }
    else{
        Ptree *type_name, *arg;
        Encoding type_encode, name_encode;
        if(!rTypeSpecifier(type_name, TRUE, type_encode))
            return FALSE;

        if(!rDeclarator(arg, kArgDeclarator, FALSE, type_encode, name_encode,
                        TRUE))
            return FALSE;

        decl = Ptree::List(type_name, arg);
        if(lex->LookAhead(0) == '='){
            Ptree* exp;
            lex->GetToken(tk1);
            if(!rAdditiveExpr(exp))
                return FALSE;

            decl = Ptree::Nconc(decl, Ptree::List(new Leaf(tk1), exp));
        }
    }

    return TRUE;
}

/*
   extern.template.decl
   : EXTERN TEMPLATE declaration
*/
bool Parser::rExternTemplateDecl(Ptree*& decl)
{
    Token tk1, tk2;
    Ptree* body;

    if(lex->GetToken(tk1) != EXTERN)
        return FALSE;

    if(lex->GetToken(tk2) != TEMPLATE)
        return FALSE;

    if(!rDeclaration(body))
        return FALSE;

    decl = new PtreeExternTemplate(new Leaf(tk1),
                                   Ptree::List(new Leaf(tk2), body));
    return TRUE;
}

/*
  declaration
  : integral.declaration
  | const.declaration
  | other.declaration

  decl.head
  : {member.spec} {storage.spec} {member.spec} {cv.qualify}

  integral.declaration
  : integral.decl.head declarators (';' | function.body)
  | integral.decl.head ';'
  | integral.decl.head ':' expression ';'

  integral.decl.head
  : decl.head integral.or.class.spec {cv.qualify}

  other.declaration
  : decl.head name {cv.qualify} declarators (';' | function.body)
  | decl.head name constructor.decl (';' | function.body)
  | FRIEND name ';'

  const.declaration
  : cv.qualify {'*'} Identifier '=' expression {',' declarators} ';'

  Note: if you modify this function, look at declaration.statement, too.
  Note: this regards a statement like "T (a);" as a constructor
        declaration.  See isConstructorDecl().
*/
bool Parser::rDeclaration(Ptree*& statement)
{
    Ptree *mem_s, *storage_s, *cv_q, *integral, *head;
    Encoding type_encode;

    if(!optMemberSpec(mem_s) || !optStorageSpec(storage_s))
        return FALSE;

    if(mem_s == nil)
        head = nil;
    else
        head = mem_s;   // mem_s is a list.

    if(storage_s != nil)
        head = Ptree::Snoc(head, storage_s);

    if(mem_s == nil)
        if(optMemberSpec(mem_s))
            head = Ptree::Nconc(head, mem_s);
        else
            return FALSE;

    if(!optCvQualify(cv_q)
       || !optIntegralTypeOrClassSpec(integral, type_encode))
        return FALSE;

    if(integral != nil)
        return rIntegralDeclaration(statement, type_encode,
                                    head, integral, cv_q);
    else{
        type_encode.Clear();
        int t = lex->LookAhead(0);
        if(cv_q != nil && ((t == Identifier && lex->LookAhead(1) == '=')
                           || t == '*'))
            return rConstDeclaration(statement, type_encode, head, cv_q);
        else
            return rOtherDeclaration(statement, type_encode,
                                     mem_s, cv_q, head);
    }
}

bool Parser::rIntegralDeclaration(Ptree*& statement, Encoding& type_encode,
                                  Ptree* head, Ptree* integral, Ptree* cv_q)
{
    Token tk;
    Ptree *cv_q2, *decl;

    if(!optCvQualify(cv_q2))
        return FALSE;

    if(cv_q != nil)
        if(cv_q2 == nil)
            integral = Ptree::Snoc(cv_q, integral);
        else
            integral = Ptree::Nconc(cv_q, Ptree::Cons(integral, cv_q2));
    else if(cv_q2 != nil)
        integral = Ptree::Cons(integral, cv_q2);

    type_encode.CvQualify(cv_q, cv_q2);
    switch(lex->LookAhead(0)){
    case ';' :
        lex->GetToken(tk);
        statement = new PtreeDeclaration(head, Ptree::List(integral,
                                                           new Leaf(tk)));
        return TRUE;
    case ':' :  // bit field
        lex->GetToken(tk);
        if(!rExpression(decl))
            return FALSE;

        decl = Ptree::List(Ptree::List(new Leaf(tk), decl));
        if(lex->GetToken(tk) != ';')
            return FALSE;

        statement = new PtreeDeclaration(head, Ptree::List(integral, decl,
                                                           new Leaf(tk)));
        return TRUE;
    default :
        if(!rDeclarators(decl, type_encode, TRUE))
            return FALSE;

        if(lex->LookAhead(0) == ';'){
            lex->GetToken(tk);
            statement = new PtreeDeclaration(head, Ptree::List(integral, decl,
                                                               new Leaf(tk)));
            return TRUE;
        }
        else{
            Ptree* body;
            if(!rFunctionBody(body))
                return FALSE;

            if(Ptree::Length(decl) != 1)
                return FALSE;

            statement = new PtreeDeclaration(head,
                                             Ptree::List(integral,
                                                         decl->Car(), body));
            return TRUE;
        }
    }
}

bool Parser::rConstDeclaration(Ptree*& statement, Encoding&,
                               Ptree* head, Ptree* cv_q)
{
    Ptree* decl;
    Token tk;
    Encoding type_encode;

    type_encode.SimpleConst();
    if(!rDeclarators(decl, type_encode, FALSE))
        return FALSE;

    if(lex->LookAhead(0) != ';')
        return FALSE;

    lex->GetToken(tk);
    statement = new PtreeDeclaration(head, Ptree::List(cv_q, decl,
                                                       new Leaf(tk)));
    return TRUE;
}

bool Parser::rOtherDeclaration(Ptree*& statement, Encoding& type_encode,
                               Ptree* mem_s, Ptree* cv_q, Ptree* head)
{
    Ptree *type_name, *decl, *cv_q2;
    Token tk;

    if(!rName(type_name, type_encode))
        return FALSE;

    if(cv_q == nil && isConstructorDecl()){
        Encoding ftype_encode;
        if(!rConstructorDecl(decl, ftype_encode))
            return FALSE;

        decl = Ptree::List(new PtreeDeclarator(type_name, decl,
                                               ftype_encode, type_encode,
                                               type_name));
        type_name = nil;
    }
    else if(mem_s != nil && lex->LookAhead(0) == ';'){
        // FRIEND name ';'
        if(Ptree::Length(mem_s) == 1 && mem_s->Car()->What() == FRIEND){
            lex->GetToken(tk);
            statement = new PtreeDeclaration(head, Ptree::List(type_name,
                                                               new Leaf(tk)));
            return TRUE;
        }
        else
            return FALSE;
    }
    else{
        if(!optCvQualify(cv_q2))
            return FALSE;

        if(cv_q != nil)
            if(cv_q2 == nil)
                type_name = Ptree::Snoc(cv_q, type_name);
            else
                type_name = Ptree::Nconc(cv_q, Ptree::Cons(type_name, cv_q2));
        else if(cv_q2 != nil)
            type_name = Ptree::Cons(type_name, cv_q2);

        type_encode.CvQualify(cv_q, cv_q2);
        if(!rDeclarators(decl, type_encode, FALSE))
            return FALSE;
    }

    if(lex->LookAhead(0) == ';'){
        lex->GetToken(tk);
        statement = new PtreeDeclaration(head, Ptree::List(type_name, decl,
                                                           new Leaf(tk)));
    }
    else{
        Ptree* body;
        if(!rFunctionBody(body))
            return FALSE;

        if(Ptree::Length(decl) != 1)
            return FALSE;

        statement = new PtreeDeclaration(head, Ptree::List(type_name,
                                                           decl->Car(), body));
    }

    return TRUE;
}

/*
  This returns TRUE for an declaration like:
        T (a);
  even if a is not a type name.  This is a bug according to the ANSI
  specification, but I believe none says "T (a);" for a variable
  declaration.
*/
bool Parser::isConstructorDecl()
{
    if(lex->LookAhead(0) != '(')
        return FALSE;
    else{
        int t = lex->LookAhead(1);
        if(t == '*' || t == '&' || t == '(')
            return FALSE;       // declarator
        else if(t == CONST || t == VOLATILE)
            return TRUE;        // constructor or declarator
        else if(isPtrToMember(1))
            return FALSE;       // declarator (::*)
        else
            return TRUE;        // maybe constructor
    }
}

/*
  ptr.to.member
  : {'::'} (identifier {'<' any* '>'} '::')+ '*'
*/
bool Parser::isPtrToMember(int i)
{
    int t0 = lex->LookAhead(i++);

    if(t0 == Scope)
        t0 = lex->LookAhead(i++);

    while(t0 == Identifier){
        int t = lex->LookAhead(i++);
        if(t == '<'){
            int n = 1;
            while(n > 0){
                int u = lex->LookAhead(i++);
                if(u == '<')
                    ++n;
                else if(u == '>')
                    --n;
                else if(u == '('){
                    int m = 1;
                    while(m > 0){
                        int v = lex->LookAhead(i++);
                        if(v == '(')
                            ++m;
                        else if(v == ')')
                            --m;
                        else if(v == '\0' || v == ';' || v == '}')
                            return FALSE;
                    }
                }
                else if(u == '\0' || u == ';' || u == '}')
                    return FALSE;
            }

            t = lex->LookAhead(i++);
        }

        if(t != Scope)
            return FALSE;

        t0 = lex->LookAhead(i++);
        if(t0 == '*')
            return TRUE;
    }

    return FALSE;
}

/*
  member.spec
  : (FRIEND | INLINE | VIRTUAL | userdef.keyword)+
*/
bool Parser::optMemberSpec(Ptree*& p)
{
    Token tk;
    Ptree* lf;
    int t = lex->LookAhead(0);

    p = nil;
    while(t == FRIEND || t == INLINE || t == VIRTUAL || t == UserKeyword5){
        if(t == UserKeyword5){
            if(!rUserdefKeyword(lf))
                return FALSE;
        }
        else{
            lex->GetToken(tk);
            if(t == INLINE)
                lf = new LeafINLINE(tk);
            else if(t == VIRTUAL)
                lf = new LeafVIRTUAL(tk);
            else
                lf = new LeafFRIEND(tk);
        }

        p = Ptree::Snoc(p, lf);
        t = lex->LookAhead(0);
    }

    return TRUE;
}

/*
  storage.spec : STATIC | EXTERN | AUTO | REGISTER | MUTABLE
*/
bool Parser::optStorageSpec(Ptree*& p)
{
    int t = lex->LookAhead(0);
    if(t == STATIC || t == EXTERN || t == AUTO || t == REGISTER
       || t == MUTABLE){
        Token tk;
        lex->GetToken(tk);
        switch(t){
        case STATIC :
            p = new LeafSTATIC(tk);
            break;
        case EXTERN :
            p = new LeafEXTERN(tk);
            break;
        case AUTO :
            p = new LeafAUTO(tk);
            break;
        case REGISTER :
            p = new LeafREGISTER(tk);
            break;
        case MUTABLE :
            p = new LeafMUTABLE(tk);
            break;
        default :
            MopErrorMessage("optStorageSpec()", "fatal");
            break;
        }
    }
    else
        p = nil;        // no storage specifier

    return TRUE;
}

/*
  cv.qualify : (CONST | VOLATILE)+
*/
bool Parser::optCvQualify(Ptree*& cv)
{
    Ptree* p = nil;
    for(;;){
        int t = lex->LookAhead(0);
        if(t == CONST || t == VOLATILE){
            Token tk;
            lex->GetToken(tk);
            switch(t){
            case CONST :
                p = Ptree::Snoc(p, new LeafCONST(tk));
                break;
            case VOLATILE :
                p = Ptree::Snoc(p, new LeafVOLATILE(tk));
                break;
            default :
                MopErrorMessage("optCvQualify()", "fatal");
                break;
            }
        }
        else
            break;
    }

    cv = p;
    return TRUE;
}

/*
  integral.or.class.spec
  : (CHAR | INT | SHORT | LONG | SIGNED | UNSIGNED | FLOAT | DOUBLE
     | VOID | BOOLEAN)+
  | class.spec
  | enum.spec

  Note: if editing this, see also isTypeSpecifier().
*/
bool Parser::optIntegralTypeOrClassSpec(Ptree*& p, Encoding& encode)
{
    bool is_integral;
    int t;
    char type = ' ', flag = ' ';

    is_integral = FALSE;
    p = nil;
    for(;;){
        t = lex->LookAhead(0);
        if(t == CHAR || t == INT || t == SHORT || t == LONG || t == SIGNED
           || t == UNSIGNED || t == FLOAT || t == DOUBLE || t == VOID
           || t == BOOLEAN
#if defined(_MSC_VER)
           || t == INT64
#endif
           ){
            Token tk;
            Ptree* kw;
            lex->GetToken(tk);
            switch(t){
            case CHAR :
                type = 'c';
                kw = new LeafCHAR(tk);
                break;
            case INT :
#if defined(_MSC_VER)
            case INT64 : // an int64 is *NOT* an int but...
#endif
                if(type != 's' && type != 'l' && type != 'j' && type != 'r')
                    type = 'i';

                kw = new LeafINT(tk);
                break;
            case SHORT :
                type = 's';
                kw = new LeafSHORT(tk);
                break;
            case LONG :
                if(type == 'l')
                    type = 'j';         // long long
                else if(type == 'd')
                    type = 'r';         // double long
                else
                    type = 'l';

                kw = new LeafLONG(tk);
                break;
            case SIGNED :
                flag = 'S';
                kw = new LeafSIGNED(tk);
                break;
            case UNSIGNED :
                flag = 'U';
                kw = new LeafUNSIGNED(tk);
                break;
            case FLOAT :
                type = 'f';
                kw = new LeafFLOAT(tk);
                break;
            case DOUBLE :
                if(type == 'l')
                    type = 'r';         // long double
                else
                    type = 'd';

                kw = new LeafDOUBLE(tk);
                break;
            case VOID :
                type = 'v';
                kw = new LeafVOID(tk);
                break;
            case BOOLEAN :
                type = 'b';
                kw = new LeafBOOLEAN(tk);
                break;
            default :
                MopErrorMessage("optIntegralTypeOrClassSpec()", "fatal");
                kw = nil;
                break;
            }

            p = Ptree::Snoc(p, kw);
            is_integral = TRUE;
        }
        else
            break;
    }

    if(is_integral){
        if(flag == 'S' && type != 'c')
            flag = ' ';

        if(flag != ' ')
            encode.Append(flag);

        if(type == ' ')
            type = 'i';         // signed, unsigned

        encode.Append(type);
        return TRUE;
    }

    if(t == CLASS || t == STRUCT || t == UNION || t == UserKeyword)
        return rClassSpec(p, encode);
    else if(t == ENUM)
        return rEnumSpec(p, encode);
    else{
        p = nil;
        return TRUE;
    }
}

/*
  constructor.decl
  : '(' {arg.decl.list} ')' {cv.qualify} {throw.decl}
  {member.initializers} {'=' Constant}
*/
bool Parser::rConstructorDecl(Ptree*& constructor, Encoding& encode)
{
    Token op, cp;
    Ptree *args, *cv, *throw_decl, *mi;

    if(lex->GetToken(op) != '(')
        return FALSE;

    if(lex->LookAhead(0) == ')'){
        args = nil;
        encode.StartFuncArgs();
        encode.Void();
        encode.EndFuncArgs();
    }
    else
        if(!rArgDeclList(args, encode))
            return FALSE;

    lex->GetToken(cp);
    constructor = Ptree::List(new Leaf(op), args, new Leaf(cp));
    optCvQualify(cv);
    if(cv != nil){
        encode.CvQualify(cv);
        constructor = Ptree::Nconc(constructor, cv);
    }

    optThrowDecl(throw_decl);   // ignore in this version

    if(lex->LookAhead(0) == ':')
        if(rMemberInitializers(mi))
            constructor = Ptree::Snoc(constructor, mi);
        else
            return FALSE;

    if(lex->LookAhead(0) == '='){
        Token eq, zero;
        lex->GetToken(eq);
        if(lex->GetToken(zero) != Constant)
            return FALSE;

        constructor = Ptree::Nconc(constructor,
                                   Ptree::List(new Leaf(eq), new Leaf(zero)));
    }

    encode.NoReturnType();
    return TRUE;
}

/*
  throw.decl : THROW '(' (name {','})* {name} ')'
*/
bool Parser::optThrowDecl(Ptree*& throw_decl)
{
    Token tk;
    int t;
    Ptree* p = nil;

    if(lex->LookAhead(0) == THROW){
        lex->GetToken(tk);
        p = Ptree::Snoc(p, new LeafReserved(tk));

        if(lex->GetToken(tk) != '(')
            return FALSE;

        p = Ptree::Snoc(p, new Leaf(tk));

        for(;;){
            Ptree* q;
            Encoding encode;
            t = lex->LookAhead(0);
            if(t == '\0')
                return FALSE;
            else if(t == ')')
                break;
            else if(rName(q, encode))
                p = Ptree::Snoc(p, q);
            else
                return FALSE;

            if(lex->LookAhead(0) == ','){
                lex->GetToken(tk);
                p = Ptree::Snoc(p, new Leaf(tk));
            }
            else
                break;
        }

        if(lex->GetToken(tk) != ')')
            return FALSE;

        p = Ptree::Snoc(p, new Leaf(tk));
    }

    throw_decl = p;
    return TRUE;
}

/*
  declarators : declarator.with.init (',' declarator.with.init)*

  is_statement changes the behavior of rArgDeclListOrInit().
*/
bool Parser::rDeclarators(Ptree*& decls, Encoding& type_encode,
                          bool should_be_declarator, bool is_statement)
{
    Ptree* d;
    Token tk;
    Encoding encode;

    decls = nil;
    for(;;){
        encode.Reset(type_encode);
        if(!rDeclaratorWithInit(d, encode, should_be_declarator, is_statement))
            return FALSE;

        decls = Ptree::Snoc(decls, d);
        if(lex->LookAhead(0) == ','){
            lex->GetToken(tk);
            decls = Ptree::Snoc(decls, new Leaf(tk));
        }
        else
            return TRUE;
    };
}

/*
  declarator.with.init
  : ':' expression
  | declarator {'=' initialize.expr | ':' expression}
*/
bool Parser::rDeclaratorWithInit(Ptree*& dw, Encoding& type_encode,
                                 bool should_be_declarator,
                                 bool is_statement)
{
    Ptree *d, *e;
    Token tk;
    Encoding name_encode;

    if(lex->LookAhead(0) == ':'){       // bit field
        lex->GetToken(tk);
        if(!rExpression(e))
            return FALSE;

        dw = Ptree::List(new Leaf(tk), e);
        return TRUE;
    }
    else{
        if(!rDeclarator(d, kDeclarator, FALSE, type_encode, name_encode,
                        should_be_declarator, is_statement))
            return FALSE;

        int t = lex->LookAhead(0);
        if(t == '='){
            lex->GetToken(tk);
            if(!rInitializeExpr(e))
                return FALSE;

            dw = Ptree::Nconc(d, Ptree::List(new Leaf(tk), e));
            return TRUE;
        }
        else if(t == ':'){              // bit field
            lex->GetToken(tk);
            if(!rExpression(e))
                return FALSE;

            dw = Ptree::Nconc(d, Ptree::List(new Leaf(tk), e));
            return TRUE;
        }
        else{
            dw = d;
            return TRUE;
        }
    }
}

/*
  declarator
  : (ptr.operator)* (name | '(' declarator ')')
        ('[' comma.expression ']')* {func.args.or.init}

  func.args.or.init
  : '(' arg.decl.list.or.init ')' {cv.qualify} {throw.decl}
  {member.initializers}

  Note: We assume that '(' declarator ')' is followed by '(' or '['.
        This is to avoid accepting a function call F(x) as a pair of
        a type F and a declarator x.  This assumption is ignored
        if should_be_declarator is true.

  Note: An argument declaration list and a function-style initializer
        take a different Ptree structure.
        e.g.
            int f(char) ==> .. [f ( [[[char] nil]] )]
            Point f(1)  ==> .. [f [( [1] )]]

  Note: is_statement changes the behavior of rArgDeclListOrInit().
*/
bool Parser::rDeclarator(Ptree*& decl, DeclKind kind, bool recursive,
                         Encoding& type_encode, Encoding& name_encode,
                         bool should_be_declarator, bool is_statement)
{
    return rDeclarator2(decl, kind, recursive, type_encode, name_encode,
                        should_be_declarator, is_statement, nil);
}

bool Parser::rDeclarator2(Ptree*& decl, DeclKind kind, bool recursive,
                          Encoding& type_encode, Encoding& name_encode,
                          bool should_be_declarator, bool is_statement,
                          Ptree** declared_name)
{
    Encoding recursive_encode;
    Ptree *d;
    int t;
    bool recursive_decl = FALSE;
    Ptree *declared_name0 = nil;

    if(declared_name == nil)
        declared_name = &declared_name0;

    if(!optPtrOperator(d, type_encode))
        return FALSE;

    t = lex->LookAhead(0);
    if(t == '('){
        Token op, cp;
        Ptree* decl2;
        lex->GetToken(op);
        recursive_decl = TRUE;
        if(!rDeclarator2(decl2, kind, TRUE, recursive_encode, name_encode,
                         TRUE, FALSE, declared_name))
            return FALSE;

        if(lex->GetToken(cp) != ')')
            return FALSE;

        if(!should_be_declarator)
            if(kind == kDeclarator && d == nil){
                t = lex->LookAhead(0);
                if(t != '[' && t != '(')
                    return FALSE;
            }

        d = Ptree::Snoc(d, Ptree::List(new Leaf(op), decl2, new Leaf(cp)));
    }
    else if(kind != kCastDeclarator
           && (kind == kDeclarator || t == Identifier || t == Scope)){
        // if this is an argument declarator, "int (*)()" is valid.
        Ptree* name;
        if(rName(name, name_encode))
            d = Ptree::Snoc(d, name);
        else
            return FALSE;

        *declared_name = name;
    }
    else
        name_encode.Clear();    // empty

    for(;;){
        t = lex->LookAhead(0);
        if(t == '('){           // function
            Encoding args_encode;
            Token op, cp;
            Ptree *args, *cv, *throw_decl, *mi;
            bool is_args = TRUE;

            lex->GetToken(op);
            if(lex->LookAhead(0) == ')'){
                args = nil;
                args_encode.StartFuncArgs();
                args_encode.Void();
                args_encode.EndFuncArgs();
            }
            else
                if(!rArgDeclListOrInit(args, is_args, args_encode,
                                       is_statement))
                    return FALSE;

            if(lex->GetToken(cp) != ')')
                return FALSE;

            if(is_args){
                d = Ptree::Nconc(d, Ptree::List(new Leaf(op), args,
                                                new Leaf(cp)));
                optCvQualify(cv);
                if(cv != nil){
                    args_encode.CvQualify(cv);
                    d = Ptree::Nconc(d, cv);
                }
            }
            else
                d = Ptree::Snoc(d, Ptree::List(new Leaf(op), args,
                                               new Leaf(cp)));

            if(!args_encode.IsEmpty())
                type_encode.Function(args_encode);

            optThrowDecl(throw_decl);   // ignore in this version

            if(lex->LookAhead(0) == ':')
                if(rMemberInitializers(mi))
                    d = Ptree::Snoc(d, mi);
                else
                    return FALSE;

            break;              // "T f(int)(char)" is invalid.
        }
        else if(t == '['){      // array
            Token ob, cb;
            Ptree* expr;
            lex->GetToken(ob);
            if(lex->LookAhead(0) == ']')
                expr = nil;
            else
                if(!rCommaExpression(expr))
                    return FALSE;

            if(lex->GetToken(cb) != ']')
                return FALSE;

            type_encode.Array();
            d = Ptree::Nconc(d, Ptree::List(new Leaf(ob), expr,
                                            new Leaf(cb)));
        }
        else
            break;
    }

    if(recursive_decl)
        type_encode.Recursion(recursive_encode);

    if(recursive)
        decl = d;
    else
        if(d == nil)
            decl = new PtreeDeclarator(type_encode, name_encode,
                                       *declared_name);
        else
            decl = new PtreeDeclarator(d, type_encode, name_encode,
                                       *declared_name);

    return TRUE;
}

/*
  ptr.operator
  : (('*' | '&' | ptr.to.member) {cv.qualify})+
*/
bool Parser::optPtrOperator(Ptree*& ptrs, Encoding& encode)
{
    ptrs = nil;
    for(;;){
        int t = lex->LookAhead(0);
        if(t != '*' && t != '&' && !isPtrToMember(0))
            break;
        else{
            Ptree *op, *cv;
            if(t == '*' || t == '&'){
                Token tk;
                lex->GetToken(tk);
                op = new Leaf(tk);
                encode.PtrOperator(t);
            }
            else
                if(!rPtrToMember(op, encode))
                    return FALSE;

            ptrs = Ptree::Snoc(ptrs, op);
            optCvQualify(cv);
            if(cv != nil){
                ptrs = Ptree::Nconc(ptrs, cv);
                encode.CvQualify(cv);
            }
        }
    }

    return TRUE;
}

/*
  member.initializers
  : ':' member.init (',' member.init)*
*/
bool Parser::rMemberInitializers(Ptree*& init)
{
    Token tk;
    Ptree* m;

    if(lex->GetToken(tk) != ':')
        return FALSE;

    init = Ptree::List(new Leaf(tk));
    if(!rMemberInit(m))
        return FALSE;

    init = Ptree::Snoc(init, m);
    while(lex->LookAhead(0) == ','){
        lex->GetToken(tk);
        init = Ptree::Snoc(init, new Leaf(tk));
        if(!rMemberInit(m))
            return FALSE;

        init = Ptree::Snoc(init, m);
    }

    return TRUE;
}

/*
  member.init
  : name '(' function.arguments ')'
*/
bool Parser::rMemberInit(Ptree*& init)
{
    Ptree *name, *args;
    Token tk1, tk2;
    Encoding encode;

    if(!rName(name, encode))
        return FALSE;

    if(!name->IsLeaf())
        name = new PtreeName(name, encode);

    if(lex->GetToken(tk1) != '(')
        return FALSE;

    if(!rFunctionArguments(args))
        return FALSE;

    if(lex->GetToken(tk2) != ')')
        return FALSE;

    init = Ptree::List(name, new Leaf(tk1), args, new Leaf(tk2));
    return TRUE;
}

/*
  name : {'::'} name2 ('::' name2)*

  name2
  : Identifier {template.args}
  | '~' Identifier
  | OPERATOR operator.name {template.args}

  Don't use this function for parsing an expression
  It always regards '<' as the beginning of template arguments.
*/
bool Parser::rName(Ptree*& name, Encoding& encode)
{
    Token tk, tk2;
    int t;
    int length = 0;

    if(lex->LookAhead(0) == Scope){
        lex->GetToken(tk);
        name = Ptree::List(new Leaf(tk));
        encode.GlobalScope();
        ++length;
    }
    else
        name = nil;

    for(;;){
        t = lex->GetToken(tk);
        if(t == Identifier){
            Ptree* n = new Leaf(tk);
            t = lex->LookAhead(0);
            if(t == '<'){
                Ptree* args;
                Encoding args_encode;
                if(!rTemplateArgs(args, args_encode))
                    return FALSE;

                encode.Template(n, args_encode);
                ++length;
                n = Ptree::List(n, args);
                t = lex->LookAhead(0);
            }
            else{
                encode.SimpleName(n);
                ++length;
            }

            if(t == Scope){
                lex->GetToken(tk);
                name = Ptree::Nconc(name, Ptree::List(n, new Leaf(tk)));
            }
            else{
                if(name == nil)
                    name = n;
                else
                    name = Ptree::Snoc(name, n);

                if(length > 1)
                    encode.Qualified(length);

                return TRUE;
            }
        }
        else if(t == '~'){
            if(lex->LookAhead(0) != Identifier)
                return FALSE;

            lex->GetToken(tk2);
            Ptree* class_name = new Leaf(tk2);
            Ptree* dt = Ptree::List(new Leaf(tk), class_name);
            if(name == nil)
                name = dt;
            else
                name = Ptree::Snoc(name, dt);

            encode.Destructor(class_name);
            if(length > 0)
                encode.Qualified(length + 1);

            return TRUE;
        }
        else if(t == OPERATOR){
            Ptree* op;
            Ptree* opf;
            if(!rOperatorName(op, encode))
                return FALSE;

            t = lex->LookAhead(0);
            if(t != '<')
                opf = Ptree::List(new LeafReserved(tk), op);
            else {
                Ptree* args;
                Encoding args_encode;
                if(!rTemplateArgs(args, args_encode))
                    return FALSE;

                // here, I must merge args_encode into encode.
                // I'll do it in future. :p

                opf = Ptree::List(new LeafReserved(tk), op, args);
            }

            if(name == nil)
                name = opf;
            else
                name = Ptree::Snoc(name, opf);

            if(length > 0)
                encode.Qualified(length + 1);

            return TRUE;
        }
        else
            return FALSE;
    }
}

/*
  operator.name
  : '+' | '-' | '*' | '/' | '%' | '^' | '&' | '|' | '~'
  | '!' | '=' | '<' | '>' | AssignOp | ShiftOp | EqualOp
  | RelOp | LogAndOp | LogOrOp | IncOp | ',' | PmOp | ArrowOp
  | NEW {'[' ']'}
  | DELETE {'[' ']'}
  | '(' ')'
  | '[' ']'
  | cast.operator.name
*/
bool Parser::rOperatorName(Ptree*& name, Encoding& encode)
{
    Token tk;

    int t = lex->LookAhead(0);
    if(t == '+' || t == '-' || t == '*' || t == '/' || t == '%' || t == '^'
       || t == '&' || t == '|' || t == '~' || t == '!' || t == '=' || t == '<'
       || t == '>' || t == AssignOp || t == ShiftOp || t == EqualOp
       || t == RelOp || t == LogAndOp || t == LogOrOp || t == IncOp
       || t == ',' || t == PmOp || t == ArrowOp){
        lex->GetToken(tk);
        name = new Leaf(tk);
        encode.SimpleName(name);
        return TRUE;
    }
    else if(t == NEW || t == DELETE){
        lex->GetToken(tk);
        if(lex->LookAhead(0) != '['){
            name = new LeafReserved(tk);
            encode.SimpleName(name);
            return TRUE;
        }
        else{
            name = Ptree::List(new LeafReserved(tk));
            lex->GetToken(tk);
            name = Ptree::Snoc(name, new Leaf(tk));
            if(lex->GetToken(tk) != ']')
                return FALSE;

            name = Ptree::Snoc(name, new Leaf(tk));
            if(t == NEW)
                encode.AppendWithLen("new[]", 5);
            else
                encode.AppendWithLen("delete[]", 8);

            return TRUE;
        }
    }
    else if(t == '('){
        lex->GetToken(tk);
        name = Ptree::List(new Leaf(tk));
        if(lex->GetToken(tk) != ')')
            return FALSE;

        encode.AppendWithLen("()", 2);
        name = Ptree::Snoc(name, new Leaf(tk));
        return TRUE;
    }
    else if(t == '['){
        lex->GetToken(tk);
        name = Ptree::List(new Leaf(tk));
        if(lex->GetToken(tk) != ']')
            return FALSE;

        encode.AppendWithLen("[]", 2);
        name = Ptree::Snoc(name, new Leaf(tk));
        return TRUE;
    }
    else
        return rCastOperatorName(name, encode);
}

/*
  cast.operator.name
  : {cv.qualify} (integral.or.class.spec | name) {cv.qualify}
    {(ptr.operator)*}
*/
bool Parser::rCastOperatorName(Ptree*& name, Encoding& encode)
{
    Ptree *cv1, *cv2, *type_name, *ptr;
    Encoding type_encode;

    if(!optCvQualify(cv1))
        return FALSE;

    if(!optIntegralTypeOrClassSpec(type_name, type_encode))
        return FALSE;

    if(type_name == nil){
        type_encode.Clear();
        if(!rName(type_name, type_encode))
            return FALSE;
    }

    if(!optCvQualify(cv2))
        return FALSE;

    if(cv1 != nil)
        if(cv2 == nil)
            type_name = Ptree::Snoc(cv1, type_name);
        else
            type_name = Ptree::Nconc(cv1, Ptree::Cons(type_name, cv2));
    else if(cv2 != nil)
        type_name = Ptree::Cons(type_name, cv2);

    type_encode.CvQualify(cv1, cv2);

    if(!optPtrOperator(ptr, type_encode))
        return FALSE;

    encode.CastOperator(type_encode);
    if(ptr == nil){
        name = type_name;
        return TRUE;
    }
    else{
        name = Ptree::List(type_name, ptr);
        return TRUE;
    }
}

/*
  ptr.to.member
  : {'::'} (identifier {template.args} '::')+ '*'
*/
bool Parser::rPtrToMember(Ptree*& ptr_to_mem, Encoding& encode)
{
    Token tk;
    Ptree *p, *n;
    Encoding pm_encode;
    int length = 0;

    if(lex->LookAhead(0) == Scope){
        lex->GetToken(tk);
        p = Ptree::List(new Leaf(tk));
        pm_encode.GlobalScope();
        ++length;
    }
    else
        p = nil;

    for(;;){
        if(lex->GetToken(tk) == Identifier)
            n = new Leaf(tk);
        else
            return FALSE;

        int t = lex->LookAhead(0);
        if(t == '<'){
            Ptree* args;
            Encoding args_encode;
            if(!rTemplateArgs(args, args_encode))
                return FALSE;

            pm_encode.Template(n, args_encode);
            ++length;
            n = Ptree::List(n, args);
            t = lex->LookAhead(0);
        }
        else{
            pm_encode.SimpleName(n);
            ++length;
        }

        if(lex->GetToken(tk) != Scope)
            return FALSE;

        p = Ptree::Nconc(p, Ptree::List(n, new Leaf(tk)));
        if(lex->LookAhead(0) == '*'){
            lex->GetToken(tk);
            p = Ptree::Snoc(p, new Leaf(tk));
            break;
        }
    }

    ptr_to_mem = p;
    encode.PtrToMember(pm_encode, length);
    return TRUE;
}

/*
  template.args
  : '<' '>'
  | '<' template.argument {',' template.argument} '>'

  template.argument
  : type.name
  | logical.or.expr
*/
bool Parser::rTemplateArgs(Ptree*& temp_args, Encoding& encode)
{
    Token tk1, tk2;
    Encoding type_encode;

    if(lex->GetToken(tk1) != '<')
        return FALSE;

    // in case of Foo<>
    if(lex->LookAhead(0) == '>') {
        lex->GetToken(tk2);
        temp_args = Ptree::List(new Leaf(tk1), new Leaf(tk2));
        return TRUE;
    }

    Ptree* args = nil;
    for(;;){
        Ptree* a;
        char* pos = lex->Save();
        type_encode.Clear();
        if(rTypeName(a, type_encode))
            encode.Append(type_encode);
        else{
            lex->Restore(pos);
            if(!rLogicalOrExpr(a, TRUE))
                return FALSE;

            encode.ValueTempParam();
        }

        args = Ptree::Snoc(args, a);
        switch(lex->GetToken(tk2)){
        case '>' :
            temp_args = Ptree::List(new Leaf(tk1), args, new Leaf(tk2));
            return TRUE;
        case ',' :
            args = Ptree::Snoc(args, new Leaf(tk2));
            break;
        case ShiftOp :
            if(*tk2.ptr == '>'){
                lex->GetOnlyClosingBracket(tk2);
                temp_args = Ptree::List(new Leaf(tk1), args,
                                        new Leaf(tk2.ptr, 1));
                return TRUE;
            }

        default :
            return FALSE;
        }
    }
}

/*
  arg.decl.list.or.init
    : arg.decl.list
    | function.arguments

  This rule accepts function.arguments to parse declarations like:
        Point p(1, 3);
  "(1, 3)" is arg.decl.list.or.init.

  If maybe_init is true, we first examine whether tokens construct
  function.arguments.  This ordering is significant if tokens are
        Point p(s, t);
  s and t can be type names or variable names.
*/
bool Parser::rArgDeclListOrInit(Ptree*& arglist, bool& is_args,
                                Encoding& encode, bool maybe_init)
{
    char* pos = lex->Save();
    if(maybe_init) {
        if(rFunctionArguments(arglist))
            if(lex->LookAhead(0) == ')') {
                is_args = FALSE;
                encode.Clear();
                return TRUE;
            }

        lex->Restore(pos);
        return(is_args = rArgDeclList(arglist, encode));
    }
    else
        if(is_args = rArgDeclList(arglist, encode))
            return TRUE;
        else{
            lex->Restore(pos);
            encode.Clear();
            return rFunctionArguments(arglist);
        }
}

/*
  arg.decl.list
    : empty
    | arg.declaration ( ',' arg.declaration )* {{ ',' } Ellipses}
*/
bool Parser::rArgDeclList(Ptree*& arglist, Encoding& encode)
{
    Ptree* list;
    Ptree* d;
    int t;
    Token tk;
    Encoding arg_encode;

    encode.StartFuncArgs();
    list = nil;
    for(;;){
        arg_encode.Clear();
        t = lex->LookAhead(0);
        if(t == ')'){
            if(list == nil)
                encode.Void();

            arglist = list;
            break;
        }
        else if(t == Ellipsis){
            lex->GetToken(tk);
            encode.EllipsisArg();
            arglist = Ptree::Snoc(list, new Leaf(tk));
            break;
        }
        else if(rArgDeclaration(d, arg_encode)){
            encode.Append(arg_encode);
            list = Ptree::Snoc(list, d);
            t = lex->LookAhead(0);
            if(t == ','){
                lex->GetToken(tk);
                list = Ptree::Snoc(list, new Leaf(tk));
            }
            else if(t != ')' && t != Ellipsis)
                return FALSE;
        }
        else{
            arglist = nil;
            return FALSE;
        }
    }

    encode.EndFuncArgs();
    return TRUE;
}

/*
  arg.declaration
    : {userdef.keyword | REGISTER} type.specifier arg.declarator
      {'=' expression}
*/
bool Parser::rArgDeclaration(Ptree*& decl, Encoding& encode)
{
    Ptree *header, *type_name, *arg, *e;
    Token tk;
    Encoding name_encode;

    switch(lex->LookAhead(0)){
    case REGISTER :
        lex->GetToken(tk);
        header = new LeafREGISTER(tk);
        break;
    case UserKeyword :
        if(!rUserdefKeyword(header))
            return FALSE;
        break;
    default :
        header = nil;
        break;
    }

    if(!rTypeSpecifier(type_name, TRUE, encode))
        return FALSE;

    if(!rDeclarator(arg, kArgDeclarator, FALSE, encode, name_encode, TRUE))
        return FALSE;

    if(header == nil)
        decl = Ptree::List(type_name, arg);
    else
        decl = Ptree::List(header, type_name, arg);

    int t = lex->LookAhead(0);
    if(t == '='){
        lex->GetToken(tk);
        if(!rInitializeExpr(e))
            return FALSE;

        decl = Ptree::Nconc(decl, Ptree::List(new Leaf(tk), e));
    }

    return TRUE;
}

/*
  initialize.expr
  : expression
  | '{' initialize.expr (',' initialize.expr)* {','} '}'
*/
bool Parser::rInitializeExpr(Ptree*& exp)
{
    Token tk;
    Ptree *e, *elist;

    if(lex->LookAhead(0) != '{')
        return rExpression(exp);
    else{
        lex->GetToken(tk);
        Ptree* ob = new Leaf(tk);
        elist = nil;
        int t = lex->LookAhead(0);
        while(t != '}'){
            if(!rInitializeExpr(e)){
                if(!SyntaxError())
                    return FALSE;       // too many errors

                SkipTo('}');
                lex->GetToken(tk);
                exp = Ptree::List(ob, nil, new Leaf(tk));
                return TRUE;            // error recovery
            }

            elist = Ptree::Snoc(elist, e);
            t = lex->LookAhead(0);
            if(t == '}')
                break;
            else if(t == ','){
                lex->GetToken(tk);
                elist = Ptree::Snoc(elist, new Leaf(tk));
                t = lex->LookAhead(0);
            }
            else{
                if(!SyntaxError())
                    return FALSE;       // too many errors

                SkipTo('}');
                lex->GetToken(tk);
                exp = Ptree::List(ob, nil, new Leaf(tk));
                return TRUE;            // error recovery
            }
        }

        lex->GetToken(tk);
        exp = new PtreeBrace(ob, elist, new Leaf(tk));
        return TRUE;
    }
}

/*
  function.arguments
  : empty
  | expression (',' expression)*

  This assumes that the next token following function.arguments is ')'.
*/
bool Parser::rFunctionArguments(Ptree*& args)
{
    Ptree* exp;
    Token tk;

    args = nil;
    if(lex->LookAhead(0) == ')')
        return TRUE;

    for(;;){
        if(!rExpression(exp))
            return FALSE;

        args = Ptree::Snoc(args, exp);
        if(lex->LookAhead(0) != ',')
            return TRUE;
        else{
            lex->GetToken(tk);
            args = Ptree::Snoc(args, new Leaf(tk));
        }
    }
}

/*
  enum.spec
  : ENUM Identifier
  | ENUM {Identifier} '{' {enum.body} '}'
*/
bool Parser::rEnumSpec(Ptree*& spec, Encoding& encode)
{
    Token tk, tk2;
    Ptree* body;

    if(lex->GetToken(tk) != ENUM)
        return FALSE;

    spec = new PtreeEnumSpec(new Leaf(tk));
    int t = lex->GetToken(tk);
    if(t == Identifier){
        Ptree* name = new Leaf(tk);
        encode.SimpleName(name);
        ((PtreeEnumSpec*)spec)->encoded_name = encode.Get();
        spec = Ptree::Snoc(spec, name);
        if(lex->LookAhead(0) == '{')
            t = lex->GetToken(tk);
        else
            return TRUE;
    }
    else{
        encode.NoName();
        ((PtreeEnumSpec*)spec)->encoded_name = encode.Get();
        spec = Ptree::Snoc(spec, nil);
    }

    if(t != '{')
        return FALSE;

    if(lex->LookAhead(0) == '}')
        body = nil;
    else
        if(!rEnumBody(body))
            return FALSE;

    if(lex->GetToken(tk2) != '}')
        return FALSE;

    spec = Ptree::Snoc(spec, new PtreeBrace(new Leaf(tk), body,
                                            new Leaf(tk2)));
    return TRUE;
}

/*
  enum.body
  : Identifier {'=' expression} (',' Identifier {'=' expression})* {','}
*/
bool Parser::rEnumBody(Ptree*& body)
{
    Token tk, tk2;
    Ptree *name, *exp;

    body = nil;
    for(;;){
        if(lex->LookAhead(0) == '}')
            return TRUE;

        if(lex->GetToken(tk) != Identifier)
            return FALSE;

        if(lex->LookAhead(0, tk2) != '=')
            name = new Leaf(tk);
        else{
            lex->GetToken(tk2);
            if(!rExpression(exp)){
                if(!SyntaxError())
                    return FALSE;       // too many errors

                SkipTo('}');
                body = nil;             // empty
                return TRUE;            // error recovery
            }

            name = Ptree::List(new Leaf(tk), new Leaf(tk2), exp);
        }

        if(lex->LookAhead(0) != ','){
            body = Ptree::Snoc(body, name);
            return TRUE;
        }
        else{
            lex->GetToken(tk);
            body = Ptree::Nconc(body, Ptree::List(name, new Leaf(tk)));
        }
    }
}

/*
  class.spec
  : {userdef.keyword} class.key class.body
  | {userdef.keyword} class.key name {class.body}
  | {userdef.keyword} class.key name ':' base.specifiers class.body

  class.key
  : CLASS | STRUCT | UNION
*/
bool Parser::rClassSpec(Ptree*& spec, Encoding& encode)
{
    Ptree *head, *bases, *body, *name;
    Token tk;

    head = nil;
    if(lex->LookAhead(0) == UserKeyword)
        if(!rUserdefKeyword(head))
            return FALSE;

    int t = lex->GetToken(tk);
    if(t != CLASS && t != STRUCT && t != UNION)
        return FALSE;

    Ptree* comments = lex->GetComments();
    spec = new PtreeClassSpec(new LeafReserved(tk), nil, comments);
    if(head != nil)
        spec = new PtreeClassSpec(head, spec, comments);

    if(lex->LookAhead(0) == '{'){
        encode.NoName();
        spec = Ptree::Snoc(spec, Ptree::List(nil, nil));
    }
    else{
        if(!rName(name, encode))
            return FALSE;

        spec = Ptree::Snoc(spec, name);
        t = lex->LookAhead(0);
        if(t == ':'){
            if(!rBaseSpecifiers(bases))
                return FALSE;

            spec = Ptree::Snoc(spec, bases);
        }
        else if(t == '{')
            spec = Ptree::Snoc(spec, nil);
        else{
            ((PtreeClassSpec*)spec)->encoded_name = encode.Get();
            return TRUE;        // class.key Identifier
        }
    }

    ((PtreeClassSpec*)spec)->encoded_name = encode.Get();
    if(!rClassBody(body))
        return FALSE;

    spec = Ptree::Snoc(spec, body);
    return TRUE;
}

/*
  base.specifiers
  : ':' base.specifier (',' base.specifier)*

  base.specifier
  : {{VIRTUAL} (PUBLIC | PROTECTED | PRIVATE) {VIRTUAL}} name
*/
bool Parser::rBaseSpecifiers(Ptree*& bases)
{
    Token tk;
    int t;
    Ptree* name;
    Encoding encode;

    if(lex->GetToken(tk) != ':')
        return FALSE;

    bases = Ptree::List(new Leaf(tk));
    for(;;){
        Ptree* super = nil;
        t = lex->LookAhead(0);
        if(t == VIRTUAL){
            lex->GetToken(tk);
            super = Ptree::Snoc(super, new LeafVIRTUAL(tk));
            t = lex->LookAhead(0);
        }

        if(t == PUBLIC || t == PROTECTED || t == PRIVATE){ /* kso | -> || */
            Ptree* lf;
            switch(lex->GetToken(tk)){
            case PUBLIC :
                lf = new LeafPUBLIC(tk);
                break;
            case PROTECTED :
                lf = new LeafPROTECTED(tk);
                break;
            case PRIVATE :
                lf = new LeafPRIVATE(tk);
                break;
            default :
                MopErrorMessage("rBaseSpecifiers()", "fatal");
                lf = nil;
                break;
            }

            super = Ptree::Snoc(super, lf);
            t = lex->LookAhead(0);
        }

        if(t == VIRTUAL){
            lex->GetToken(tk);
            super = Ptree::Snoc(super, new LeafVIRTUAL(tk));
        }

        encode.Clear();
        if(!rName(name, encode))
            return FALSE;

        if(!name->IsLeaf())
            name = new PtreeName(name, encode);

        super = Ptree::Snoc(super, name);
        bases = Ptree::Snoc(bases, super);
        if(lex->LookAhead(0) != ',')
            return TRUE;
        else{
            lex->GetToken(tk);
            bases = Ptree::Snoc(bases, new Leaf(tk));
        }
    }
}

/*
  class.body : '{' (class.members)* '}'
*/
bool Parser::rClassBody(Ptree*& body)
{
    Token tk;
    Ptree *mems, *m;

    if(lex->GetToken(tk) != '{')
        return FALSE;

    Ptree* ob = new Leaf(tk);
    mems = nil;
    while(lex->LookAhead(0) != '}'){
        if(!rClassMember(m)){
            if(!SyntaxError())
                return FALSE;   // too many errors

            SkipTo('}');
            lex->GetToken(tk);
            body = Ptree::List(ob, nil, new Leaf(tk));
            return TRUE;        // error recovery
        }

        lex->GetComments();
        mems = Ptree::Snoc(mems, m);
    }

    lex->GetToken(tk);
    body = new PtreeClassBody(ob, mems, new Leaf(tk));
    return TRUE;
}

/*
  class.member
  : (PUBLIC | PROTECTED | PRIVATE) ':'
  | user.access.spec
  | ';'
  | type.def
  | template.decl
  | using.declaration
  | metaclass.decl
  | declaration
  | access.decl

  Note: if you modify this function, see ClassWalker::TranslateClassSpec()
  as well.
*/
bool Parser::rClassMember(Ptree*& mem)
{
    Token tk1, tk2;

    int t = lex->LookAhead(0);
    if(t == PUBLIC || t == PROTECTED || t == PRIVATE){
        Ptree* lf;
        switch(lex->GetToken(tk1)){
        case PUBLIC :
            lf = new LeafPUBLIC(tk1);
            break;
        case PROTECTED :
            lf = new LeafPROTECTED(tk1);
            break;
        case PRIVATE :
            lf = new LeafPRIVATE(tk1);
            break;
        default :
            MopErrorMessage("rClassMember()", "fatal");
            lf = nil;
            break;
        }

        if(lex->GetToken(tk2) != ':')
            return FALSE;

        mem = new PtreeAccessSpec(lf, Ptree::List(new Leaf(tk2)));
        return TRUE;
    }
    else if(t == UserKeyword4)
        return rUserAccessSpec(mem);
    else if(t == ';')
        return rNullDeclaration(mem);
    else if(t == TYPEDEF)
        return rTypedef(mem);
    else if(t == TEMPLATE)
        return rTemplateDecl(mem);
    else if(t == USING)
        return rUsing(mem);
    else if(t == METACLASS)
        return rMetaclassDecl(mem);
    else{
        char* pos = lex->Save();
        Ptree* comments = lex->GetComments2();
        if(rDeclaration(mem)) {
            Walker::SetDeclaratorComments(mem, comments);
            return TRUE;
        }

        lex->Restore(pos);
        return rAccessDecl(mem);
    }
}

/*
  access.decl
  : name ';'            e.g. <qualified class>::<member name>;
*/
bool Parser::rAccessDecl(Ptree*& mem)
{
    Ptree* name;
    Encoding encode;
    Token tk;

    if(!rName(name, encode))
        return FALSE;

    if(lex->GetToken(tk) != ';')
        return FALSE;

    mem = new PtreeAccessDecl(new PtreeName(name, encode),
                               Ptree::List(new Leaf(tk)));
    return TRUE;
}

/*
  user.access.spec
  : UserKeyword4 ':'
  | UserKeyword4 '(' function.arguments ')' ':'
*/
bool Parser::rUserAccessSpec(Ptree*& mem)
{
    Token tk1, tk2, tk3, tk4;
    Ptree* args;

    if(lex->GetToken(tk1) != UserKeyword4)
        return FALSE;

    int t = lex->GetToken(tk2);
    if(t == ':'){
        mem = new PtreeUserAccessSpec(new Leaf(tk1),
                                      Ptree::List(new Leaf(tk2)));
        return TRUE;
    }
    else if(t == '('){
        if(!rFunctionArguments(args))
            return FALSE;

        if(lex->GetToken(tk3) != ')')
            return FALSE;

        if(lex->GetToken(tk4) != ':')
            return FALSE;

        mem = new PtreeUserAccessSpec(new Leaf(tk1),
                                      Ptree::List(new Leaf(tk2), args,
                                                  new Leaf(tk3),
                                                  new Leaf(tk4)));
        return TRUE;
    }
    else
        return FALSE;
}

/*
  comma.expression
  : expression
  | comma.expression ',' expression     (left-to-right)
*/
bool Parser::rCommaExpression(Ptree*& exp)
{
    Token tk;
    Ptree *right;

    if(!rExpression(exp))
        return FALSE;

    while(lex->LookAhead(0) == ','){
        lex->GetToken(tk);
        if(!rExpression(right))
            return FALSE;

        exp = new PtreeCommaExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  expression
  : conditional.expr {(AssignOp | '=') expression}      right-to-left
*/
bool Parser::rExpression(Ptree*& exp)
{
    Token tk;
    Ptree *left, *right;

    if(!rConditionalExpr(left))
        return FALSE;

    int t = lex->LookAhead(0);
    if(t != '=' && t != AssignOp)
        exp = left;
    else{
        lex->GetToken(tk);
        if(!rExpression(right))
            return FALSE;

        exp = new PtreeAssignExpr(left, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  conditional.expr
  : logical.or.expr {'?' comma.expression ':' conditional.expr}  right-to-left
*/
bool Parser::rConditionalExpr(Ptree*& exp)
{
    Token tk1, tk2;
    Ptree *then, *otherwise;

    if(!rLogicalOrExpr(exp, FALSE))
        return FALSE;

    if(lex->LookAhead(0) == '?'){
        lex->GetToken(tk1);
        if(!rCommaExpression(then))
            return FALSE;

        if(lex->GetToken(tk2) != ':')
            return FALSE;

        if(!rConditionalExpr(otherwise))
            return FALSE;

        exp = new PtreeCondExpr(exp, Ptree::List(new Leaf(tk1), then,
                                                 new Leaf(tk2), otherwise));
    }

    return TRUE;
}

/*
  logical.or.expr
  : logical.and.expr
  | logical.or.expr LogOrOp logical.and.expr            left-to-right
*/
bool Parser::rLogicalOrExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rLogicalAndExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == LogOrOp){
        lex->GetToken(tk);
        if(!rLogicalAndExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  logical.and.expr
  : inclusive.or.expr
  | logical.and.expr LogAndOp inclusive.or.expr
*/
bool Parser::rLogicalAndExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rInclusiveOrExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == LogAndOp){
        lex->GetToken(tk);
        if(!rInclusiveOrExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  inclusive.or.expr
  : exclusive.or.expr
  | inclusive.or.expr '|' exclusive.or.expr
*/
bool Parser::rInclusiveOrExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rExclusiveOrExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == '|'){
        lex->GetToken(tk);
        if(!rExclusiveOrExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  exclusive.or.expr
  : and.expr
  | exclusive.or.expr '^' and.expr
*/
bool Parser::rExclusiveOrExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rAndExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == '^'){
        lex->GetToken(tk);
        if(!rAndExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  and.expr
  : equality.expr
  | and.expr '&' equality.expr
*/
bool Parser::rAndExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rEqualityExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == '&'){
        lex->GetToken(tk);
        if(!rEqualityExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  equality.expr
  : relational.expr
  | equality.expr EqualOp relational.expr
*/
bool Parser::rEqualityExpr(Ptree*& exp, bool temp_args)
{
    Token tk;
    Ptree *right;

    if(!rRelationalExpr(exp, temp_args))
        return FALSE;

    while(lex->LookAhead(0) == EqualOp){
        lex->GetToken(tk);
        if(!rRelationalExpr(right, temp_args))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  relational.expr
  : shift.expr
  | relational.expr (RelOp | '<' | '>') shift.expr
*/
bool Parser::rRelationalExpr(Ptree*& exp, bool temp_args)
{
    int t;
    Token tk;
    Ptree *right;

    if(!rShiftExpr(exp))
        return FALSE;

    while(t = lex->LookAhead(0),
          (t == RelOp || t == '<' || (t == '>' && !temp_args))){
        lex->GetToken(tk);
        if(!rShiftExpr(right))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  shift.expr
  : additive.expr
  | shift.expr ShiftOp additive.expr
*/
bool Parser::rShiftExpr(Ptree*& exp)
{
    Token tk;
    Ptree *right;

    if(!rAdditiveExpr(exp))
        return FALSE;

    while(lex->LookAhead(0) == ShiftOp){
        lex->GetToken(tk);
        if(!rAdditiveExpr(right))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  additive.expr
  : multiply.expr
  | additive.expr ('+' | '-') multiply.expr
*/
bool Parser::rAdditiveExpr(Ptree*& exp)
{
    int t;
    Token tk;
    Ptree *right;

    if(!rMultiplyExpr(exp))
        return FALSE;

    while(t = lex->LookAhead(0), (t == '+' || t == '-')){
        lex->GetToken(tk);
        if(!rMultiplyExpr(right))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  multiply.expr
  : pm.expr
  | multiply.expr ('*' | '/' | '%') pm.expr
*/
bool Parser::rMultiplyExpr(Ptree*& exp)
{
    int t;
    Token tk;
    Ptree *right;

    if(!rPmExpr(exp))
        return FALSE;

    while(t = lex->LookAhead(0), (t == '*' || t == '/' || t == '%')){
        lex->GetToken(tk);
        if(!rPmExpr(right))
            return FALSE;

        exp = new PtreeInfixExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  pm.expr       (pointer to member .*, ->*)
  : cast.expr
  | pm.expr PmOp cast.expr
*/
bool Parser::rPmExpr(Ptree*& exp)
{
    Token tk;
    Ptree *right;

    if(!rCastExpr(exp))
        return FALSE;

    while(lex->LookAhead(0) == PmOp){
        lex->GetToken(tk);
        if(!rCastExpr(right))
            return FALSE;

        exp = new PtreePmExpr(exp, Ptree::List(new Leaf(tk), right));
    }

    return TRUE;
}

/*
  cast.expr
  : unary.expr
  | '(' type.name ')' cast.expr
*/
bool Parser::rCastExpr(Ptree*& exp)
{
    if(lex->LookAhead(0) != '(')
        return rUnaryExpr(exp);
    else{
        Token tk1, tk2;
        Ptree* tname;
        char* pos = lex->Save();
        lex->GetToken(tk1);
        if(rTypeName(tname))
            if(lex->GetToken(tk2) == ')')
                if(rCastExpr(exp)){
                    exp = new PtreeCastExpr(new Leaf(tk1),
                                            Ptree::List(tname, new Leaf(tk2),
                                                        exp));
                    return TRUE;
                }

        lex->Restore(pos);
        return rUnaryExpr(exp);
    }
}

/*
  type.name
  : type.specifier cast.declarator
*/
bool Parser::rTypeName(Ptree*& tname)
{
    Encoding type_encode;
    return rTypeName(tname, type_encode);
}

bool Parser::rTypeName(Ptree*& tname, Encoding& type_encode)
{
    Ptree *type_name, *arg;
    Encoding name_encode;

    if(!rTypeSpecifier(type_name, TRUE, type_encode))
        return FALSE;

    if(!rDeclarator(arg, kCastDeclarator, FALSE, type_encode, name_encode,
                    FALSE))
        return FALSE;

    tname = Ptree::List(type_name, arg);
    return TRUE;
}

/*
  unary.expr
  : postfix.expr
  | ('*' | '&' | '+' | '-' | '!' | '~' | IncOp) cast.expr
  | sizeof.expr
  | allocate.expr
  | throw.expression
*/
bool Parser::rUnaryExpr(Ptree*& exp)
{
    int t = lex->LookAhead(0);
    if(t == '*' || t == '&' || t == '+' || t == '-' || t == '!'
       || t == '~' || t == IncOp){
        Token tk;
        Ptree* right;

        lex->GetToken(tk);
        if(!rCastExpr(right))
            return FALSE;

        exp = new PtreeUnaryExpr(new Leaf(tk), Ptree::List(right));
        return TRUE;
    }
    else if(t == SIZEOF)
        return rSizeofExpr(exp);
    else if(t == THROW)
        return rThrowExpr(exp);
    else if(isAllocateExpr(t))
        return rAllocateExpr(exp);
    else
        return rPostfixExpr(exp);
}

/*
  throw.expression
  : THROW {expression}
*/
bool Parser::rThrowExpr(Ptree*& exp)
{
    Token tk;
    Ptree* e;

    if(lex->GetToken(tk) != THROW)
        return FALSE;

    int t = lex->LookAhead(0);
    if(t == ':' || t == ';')
        e = nil;
    else
        if(!rExpression(e))
            return FALSE;

    exp = new PtreeThrowExpr(new LeafReserved(tk), Ptree::List(e));
    return TRUE;
}

/*
  sizeof.expr
  : SIZEOF unary.expr
  | SIZEOF '(' type.name ')'
*/
bool Parser::rSizeofExpr(Ptree*& exp)
{
    Token tk;
    Ptree* unary;

    if(lex->GetToken(tk) != SIZEOF)
        return FALSE;

    if(lex->LookAhead(0) == '('){
        Ptree* tname;
        Token op, cp;

        char* pos = lex->Save();
        lex->GetToken(op);
        if(rTypeName(tname))
            if(lex->GetToken(cp) == ')'){
                exp = new PtreeSizeofExpr(new Leaf(tk),
                                          Ptree::List(new Leaf(op), tname,
                                                      new Leaf(cp)));
                return TRUE;
            }

        lex->Restore(pos);
    }

    if(!rUnaryExpr(unary))
        return FALSE;

    exp = new PtreeSizeofExpr(new Leaf(tk), Ptree::List(unary));
    return TRUE;
}

bool Parser::isAllocateExpr(int t)
{
    if(t == UserKeyword)
        return TRUE;
    else{
        if(t == Scope)
            t = lex->LookAhead(1);

        if(t == NEW || t == DELETE)
            return TRUE;
        else
            return FALSE;
    }
}

/*
  allocate.expr
  : {Scope | userdef.keyword} NEW allocate.type
  | {Scope} DELETE {'[' ']'} cast.expr
*/
bool Parser::rAllocateExpr(Ptree*& exp)
{
    Token tk;
    Ptree* head = nil;

    bool ukey = FALSE;
    int t = lex->LookAhead(0);
    if(t == Scope){
        lex->GetToken(tk);
        head = new Leaf(tk);
    }
    else if(t == UserKeyword){
        if(!rUserdefKeyword(head))
            return FALSE;

        ukey = TRUE;
    }

    t = lex->GetToken(tk);
    if(t == DELETE){
        Ptree* obj;
        if(ukey)
            return FALSE;

        if(head == nil)
            exp = new PtreeDeleteExpr(new LeafReserved(tk), nil);
        else
            exp = new PtreeDeleteExpr(head,
                                      Ptree::List(new LeafReserved(tk)));

        if(lex->LookAhead(0) == '['){
            lex->GetToken(tk);
            exp = Ptree::Snoc(exp, new Leaf(tk));
            if(lex->GetToken(tk) != ']')
                return FALSE;

            exp = Ptree::Snoc(exp, new Leaf(tk));
        }

        if(!rCastExpr(obj))
            return FALSE;

        exp = Ptree::Snoc(exp, obj);
        return TRUE;
    }
    else if(t == NEW){
        Ptree *atype;
        if(head == nil)
            exp = new PtreeNewExpr(new LeafReserved(tk), nil);
        else
            exp = new PtreeNewExpr(head, Ptree::List(new LeafReserved(tk)));

        if(!rAllocateType(atype))
            return FALSE;

        exp = Ptree::Nconc(exp, atype);
        return TRUE;
    }
    else
        return FALSE;
}

/*
  userdef.keyword
  : [UserKeyword | UserKeyword5] {'(' function.arguments ')'}
*/
bool Parser::rUserdefKeyword(Ptree*& ukey)
{
    Token tk;

    int t = lex->GetToken(tk);
    if(t != UserKeyword && t != UserKeyword5)
        return FALSE;

    if(lex->LookAhead(0) != '(')
        ukey = new PtreeUserdefKeyword(new Leaf(tk), nil);
    else{
        Ptree* args;
        Token op, cp;
        lex->GetToken(op);
        if(!rFunctionArguments(args))
            return FALSE;

        if(lex->GetToken(cp) != ')')
            return FALSE;

        ukey = new PtreeUserdefKeyword(new Leaf(tk),
                        Ptree::List(new Leaf(op), args, new Leaf(cp)));
    }

    return TRUE;
}

/*
  allocate.type
  : {'(' function.arguments ')'} type.specifier new.declarator
    {allocate.initializer}
  | {'(' function.arguments ')'} '(' type.name ')' {allocate.initializer}
*/
bool Parser::rAllocateType(Ptree*& atype)
{
    Token op, cp;
    Ptree *tname, *init, *exp;

    if(lex->LookAhead(0) != '(')
        atype = Ptree::List(nil);
    else{
        lex->GetToken(op);

        char* pos = lex->Save();
        if(rTypeName(tname))
            if(lex->GetToken(cp) == ')')
                if(lex->LookAhead(0) != '('){
                    atype = Ptree::List(nil, Ptree::List(new Leaf(op), tname,
                                                         new Leaf(cp)));
                    if(!isTypeSpecifier())
                        return TRUE;
                }
                else if(rAllocateInitializer(init)){
                    atype = Ptree::List(nil,
                                        Ptree::List(new Leaf(op), tname,
                                                    new Leaf(cp)),
                                        init);
                    // the next token cannot be '('
                    if(lex->LookAhead(0) != '(')
                        return TRUE;
                }

        // if we reach here, we have to process '(' function.arguments ')'.
        lex->Restore(pos);
        if(!rFunctionArguments(exp))
            return FALSE;

        if(lex->GetToken(cp) != ')')
            return FALSE;

        atype = Ptree::List(Ptree::List(new Leaf(op), exp, new Leaf(cp)));
    }

    if(lex->LookAhead(0) == '('){
        lex->GetToken(op);
        if(!rTypeName(tname))
            return FALSE;

        if(lex->GetToken(cp) != ')')
            return FALSE;

        atype = Ptree::Snoc(atype, Ptree::List(new Leaf(op), tname,
                                               new Leaf(cp)));
    }
    else{
        Ptree* decl;
        Encoding type_encode;
        if(!rTypeSpecifier(tname, FALSE, type_encode))
            return FALSE;

        if(!rNewDeclarator(decl, type_encode))
            return FALSE;

        atype = Ptree::Snoc(atype, Ptree::List(tname, decl));
    }

    if(lex->LookAhead(0) == '('){
        if(!rAllocateInitializer(init))
            return FALSE;

        atype = Ptree::Snoc(atype, init);
    }

    return TRUE;
}

/*
  new.declarator
  : empty
  | ptr.operator
  | {ptr.operator} ('[' comma.expression ']')+
*/
bool Parser::rNewDeclarator(Ptree*& decl, Encoding& encode)
{
    decl = nil;
    if(lex->LookAhead(0) != '[')
        if(!optPtrOperator(decl, encode))
            return FALSE;

    while(lex->LookAhead(0) == '['){
        Token ob, cb;
        Ptree* exp;
        lex->GetToken(ob);
        if(!rCommaExpression(exp))
            return FALSE;

        if(lex->GetToken(cb) != ']')
            return FALSE;

        encode.Array();
        decl = Ptree::Nconc(decl, Ptree::List(new Leaf(ob), exp,
                                              new Leaf(cb)));
    }

    if(decl == nil)
        decl = new PtreeDeclarator(encode);
    else
        decl = new PtreeDeclarator(decl, encode);

    return TRUE;
}

/*
  allocate.initializer
  : '(' {initialize.expr (',' initialize.expr)* } ')'
*/
bool Parser::rAllocateInitializer(Ptree*& init)
{
    Token op, cp;

    if(lex->GetToken(op) != '(')
        return FALSE;

    if(lex->LookAhead(0) == ')'){
        lex->GetToken(cp);
        init = Ptree::List(new Leaf(op), nil, new Leaf(cp));
        return TRUE;
    }

    init = nil;
    for(;;){
        Ptree* exp;
        if(!rInitializeExpr(exp))
            return FALSE;

        init = Ptree::Snoc(init, exp);
        if(lex->LookAhead(0) != ',')
            break;
        else{
            Token tk;
            lex->GetToken(tk);
            init = Ptree::Snoc(init, new Leaf(tk));
        }
    }

    lex->GetToken(cp);
    init = Ptree::List(new Leaf(op), init, new Leaf(cp));
    return TRUE;
}

/*
  postfix.exp
  : primary.exp
  | postfix.expr '[' comma.expression ']'
  | postfix.expr '(' function.arguments ')'
  | postfix.expr '.' var.name
  | postfix.expr ArrowOp var.name
  | postfix.expr IncOp
  | openc++.postfix.expr

  openc++.postfix.expr
  : postfix.expr '.' userdef.statement
  | postfix.expr ArrowOp userdef.statement

  Note: function-style casts are accepted as function calls.
*/
bool Parser::rPostfixExpr(Ptree*& exp)
{
    Ptree* e;
    Token cp, op;
    int t, t2;

    if(!rPrimaryExpr(exp))
        return FALSE;

    for(;;){
        switch(lex->LookAhead(0)){
        case '[' :
            lex->GetToken(op);
            if(!rCommaExpression(e))
                return FALSE;

            if(lex->GetToken(cp) != ']')
                return FALSE;

            exp = new PtreeArrayExpr(exp, Ptree::List(new Leaf(op),
                                                      e, new Leaf(cp)));
            break;
        case '(' :
            lex->GetToken(op);
            if(!rFunctionArguments(e))
                return FALSE;

            if(lex->GetToken(cp) != ')')
                return FALSE;

            exp = new PtreeFuncallExpr(exp, Ptree::List(new Leaf(op),
                                                        e, new Leaf(cp)));
            break;
        case IncOp :
            lex->GetToken(op);
            exp = new PtreePostfixExpr(exp, Ptree::List(new Leaf(op)));
            break;
        case '.' :
        case ArrowOp :
            t2 = lex->GetToken(op);
            t = lex->LookAhead(0);
            if(t == UserKeyword || t == UserKeyword2 || t == UserKeyword3){
                if(!rUserdefStatement(e))
                    return FALSE;

                exp = new PtreeUserStatementExpr(exp,
                                                 Ptree::Cons(new Leaf(op), e));
                break;
            }
            else{
                if(!rVarName(e))
                    return FALSE;

                if(t2 == '.')
                    exp = new PtreeDotMemberExpr(exp,
                                                 Ptree::List(new Leaf(op), e));
                else
                    exp = new PtreeArrowMemberExpr(exp,
                                                Ptree::List(new Leaf(op), e));
                break;
            }
        default :
            return TRUE;
        }
    }
}

/*
  primary.exp
  : Constant
  | CharConst
  | StringL
  | THIS
  | var.name
  | '(' comma.expression ')'
  | integral.or.class.spec '(' function.arguments ')'
  | openc++.primary.exp

  openc++.primary.exp
  : var.name '::' userdef.statement
*/
bool Parser::rPrimaryExpr(Ptree*& exp)
{
    Token tk, tk2;
    Ptree* exp2;
    Encoding cast_type_encode;

    switch(lex->LookAhead(0)){
    case Constant : case CharConst : case StringL :
        lex->GetToken(tk);
        exp = new Leaf(tk);
        return TRUE;
    case THIS :
        lex->GetToken(tk);
        exp = new LeafThis(tk);
        return TRUE;
    case '(' :
        lex->GetToken(tk);
        if(!rCommaExpression(exp2))
            return FALSE;

        if(lex->GetToken(tk2) != ')')
            return FALSE;

        exp = new PtreeParenExpr(new Leaf(tk),
                                 Ptree::List(exp2, new Leaf(tk2)));
        return TRUE;
    default :
        if(!optIntegralTypeOrClassSpec(exp, cast_type_encode))
            return FALSE;

        if(exp != nil){         // if integral.or.class.spec
            if(lex->GetToken(tk) != '(')
                return FALSE;

            if(!rFunctionArguments(exp2))
                return FALSE;

            if(lex->GetToken(tk2) != ')')
                return FALSE;

            exp = new PtreeFstyleCastExpr(cast_type_encode, exp,
                                          Ptree::List(new Leaf(tk), exp2,
                                                      new Leaf(tk2)));
            return TRUE;
        }
        else{
            if(!rVarName(exp))
                return FALSE;

            if(lex->LookAhead(0) == Scope){
                lex->GetToken(tk);
                if(!rUserdefStatement(exp2))
                    return FALSE;

                exp = new PtreeStaticUserStatementExpr(exp,
                                        Ptree::Cons(new Leaf(tk), exp2));
            }

            return TRUE;
        }
    }
}

/*
  userdef.statement
  : UserKeyword '(' function.arguments ')' compound.statement
  | UserKeyword2 '(' arg.decl.list ')' compound.statement
  | UserKeyword3 '(' expr.statement {comma.expression} ';'
                        {comma.expression} ')' compound.statement
*/
bool Parser::rUserdefStatement(Ptree*& st)
{
    Token tk, tk2, tk3, tk4;
    Ptree *keyword, *exp, *body, *exp2, *exp3;
    Encoding dummy_encode;

    int t = lex->GetToken(tk);
    if(lex->GetToken(tk2) != '(')
        return FALSE;

    switch(t){
    case UserKeyword :
        keyword = new LeafReserved(tk);
        if(!rFunctionArguments(exp))
            return FALSE;
        goto rest;
    case UserKeyword2 :
        keyword = new LeafUserKeyword2(tk);
        if(!rArgDeclList(exp, dummy_encode))
            return FALSE;
    rest:
        if(lex->GetToken(tk3) != ')')
            return FALSE;

        if(!rCompoundStatement(body))
            return FALSE;

        st = Ptree::List(keyword, new Leaf(tk2), exp, new Leaf(tk3),
                         body);
        return TRUE;
    case UserKeyword3 :
        if(!rExprStatement(exp))
            return FALSE;

        if(lex->LookAhead(0) == ';')
            exp2 = nil;
        else
            if(!rCommaExpression(exp2))
                return FALSE;

        if(lex->GetToken(tk3) != ';')
            return FALSE;

        if(lex->LookAhead(0) == ')')
            exp3 = nil;
        else
            if(!rCommaExpression(exp3))
                return FALSE;

        if(lex->GetToken(tk4) != ')')
            return FALSE;

        if(!rCompoundStatement(body))
            return FALSE;

        st = Ptree::List(new Leaf(tk), new Leaf(tk2), exp, exp2,
                         new Leaf(tk3), exp3, new Leaf(tk4), body);
        return TRUE;
    default :
        return FALSE;
    }
}

/*
  var.name : {'::'} name2 ('::' name2)*

  name2
  : Identifier {template.args}
  | '~' Identifier
  | OPERATOR operator.name

  if var.name ends with a template type, the next token must be '('
*/
bool Parser::rVarName(Ptree*& name)
{
    Encoding encode;

    if(rVarNameCore(name, encode)){
        if(!name->IsLeaf())
            name = new PtreeName(name, encode);

        return TRUE;
    }
    else
        return FALSE;
}

bool Parser::rVarNameCore(Ptree*& name, Encoding& encode)
{
    Token tk;
    int length = 0;

    if(lex->LookAhead(0) == Scope){
        lex->GetToken(tk);
        name = Ptree::List(new Leaf(tk));
        encode.GlobalScope();
        ++length;
    }
    else
        name = nil;

    for(;;){
        int t = lex->GetToken(tk);
        if(t == Identifier){
            Ptree* n = new LeafName(tk);
            if(isTemplateArgs()){
                Ptree* args;
                Encoding args_encode;
                if(!rTemplateArgs(args, args_encode))
                    return FALSE;

                encode.Template(n, args_encode);
                ++length;
                n = Ptree::List(n, args);
            }
            else{
                encode.SimpleName(n);
                ++length;
            }

            if(moreVarName()){
                lex->GetToken(tk);
                name = Ptree::Nconc(name, Ptree::List(n, new Leaf(tk)));
            }
            else{
                if(name == nil)
                    name = n;
                else
                    name = Ptree::Snoc(name, n);

                if(length > 1)
                    encode.Qualified(length);

                return TRUE;
            }
        }
        else if(t == '~'){
            Token tk2;
            if(lex->LookAhead(0) != Identifier)
                return FALSE;

            lex->GetToken(tk2);
            Ptree* class_name = new Leaf(tk2);
            Ptree* dt = Ptree::List(new Leaf(tk), class_name);
            if(name == nil)
                name = dt;
            else
                name = Ptree::Snoc(name, dt);

            encode.Destructor(class_name);
            if(length > 0)
                encode.Qualified(length + 1);

            return TRUE;
        }
        else if(t == OPERATOR){
            Ptree* op;
            if(!rOperatorName(op, encode))
                return FALSE;

            Ptree* opf = Ptree::List(new LeafReserved(tk), op);
            if(name == nil)
                name = opf;
            else
                name = Ptree::Snoc(name, opf);

            if(length > 0)
                encode.Qualified(length + 1);

            return TRUE;
        }
        else
            return FALSE;
    }
}

bool Parser::moreVarName()
{
    if(lex->LookAhead(0) == Scope){
        int t = lex->LookAhead(1);
        if(t == Identifier || t == '~' || t == OPERATOR)
            return TRUE;
    }

    return FALSE;
}

/*
  template.args : '<' any* '>'

  template.args must be followed by '(' or '::'
*/
bool Parser::isTemplateArgs()
{
    int i = 0;
    int t = lex->LookAhead(i++);
    if(t == '<'){
        int n = 1;
        while(n > 0){
            int u = lex->LookAhead(i++);
            if(u == '<')
                ++n;
            else if(u == '>')
                --n;
            else if(u == '('){
                int m = 1;
                while(m > 0){
                    int v = lex->LookAhead(i++);
                    if(v == '(')
                        ++m;
                    else if(v == ')')
                        --m;
                    else if(v == '\0' || v == ';' || v == '}')
                        return FALSE;
                }
            }
            else if(u == '\0' || u == ';' || u == '}')
                return FALSE;
        }

        t = lex->LookAhead(i);
        return bool(t == Scope || t == '(');
    }

    return FALSE;
}

/*
  function.body  : compound.statement
*/
bool Parser::rFunctionBody(Ptree*& body)
{
    return rCompoundStatement(body);
}

/*
  compound.statement
  : '{' (statement)* '}'
*/
bool Parser::rCompoundStatement(Ptree*& body)
{
    Token ob, cb;

    if(lex->GetToken(ob) != '{')
        return FALSE;

    Ptree* sts = nil;
    while(lex->LookAhead(0) != '}'){
        Ptree* st;
        if(!rStatement(st)){
            if(!SyntaxError())
                return FALSE;   // too many errors

            SkipTo('}');
            lex->GetToken(cb);
            body = Ptree::List(new Leaf(ob), nil, new Leaf(cb));
            return TRUE;        // error recovery
        }

        lex->GetComments();
        sts = Ptree::Snoc(sts, st);
    }

    if(lex->GetToken(cb) != '}')
        return FALSE;

    body = new PtreeBlock(new Leaf(ob), sts, new Leaf(cb));
    return TRUE;
}

/*
  statement
  : compound.statement
  | typedef
  | if.statement
  | switch.statement
  | while.statement
  | do.statement
  | for.statement
  | try.statement
  | BREAK ';'
  | CONTINUE ';'
  | RETURN { comma.expression } ';'
  | GOTO Identifier ';'
  | CASE expression ':' statement
  | DEFAULT ':' statement
  | Identifier ':' statement
  | expr.statement
*/
bool Parser::rStatement(Ptree*& st)
{
    Token tk1, tk2, tk3;
    Ptree *st2, *exp;
    int k;

    switch(k = lex->LookAhead(0)){
    case '{' :
        return rCompoundStatement(st);
    case TYPEDEF :
        return rTypedef(st);
    case IF :
        return rIfStatement(st);
    case SWITCH :
        return rSwitchStatement(st);
    case WHILE :
        return rWhileStatement(st);
    case DO :
        return rDoStatement(st);
    case FOR :
        return rForStatement(st);
    case TRY :
        return rTryStatement(st);
    case BREAK :
    case CONTINUE :
        lex->GetToken(tk1);
        if(lex->GetToken(tk2) != ';')
            return FALSE;

        if(k == BREAK)
            st = new PtreeBreakStatement(new LeafReserved(tk1),
                                         Ptree::List(new Leaf(tk2)));
        else
            st = new PtreeContinueStatement(new LeafReserved(tk1),
                                            Ptree::List(new Leaf(tk2)));
        return TRUE;
    case RETURN :
        lex->GetToken(tk1);
        if(lex->LookAhead(0) == ';'){
            lex->GetToken(tk2);
            st = new PtreeReturnStatement(new LeafReserved(tk1),
                                          Ptree::List(new Leaf(tk2)));
            return TRUE;
        }
        else{
            if(!rCommaExpression(exp))
                return FALSE;

            if(lex->GetToken(tk2) != ';')
                return FALSE;

            st = new PtreeReturnStatement(new LeafReserved(tk1),
                                          Ptree::List(exp, new Leaf(tk2)));
            return TRUE;
        }
    case GOTO :
        lex->GetToken(tk1);
        if(lex->GetToken(tk2) != Identifier)
            return FALSE;

        if(lex->GetToken(tk3) != ';')
            return FALSE;

        st = new PtreeGotoStatement(new LeafReserved(tk1),
                                    Ptree::List(new Leaf(tk2), new Leaf(tk3)));
        return TRUE;
    case CASE :
        lex->GetToken(tk1);
        if(!rExpression(exp))
            return FALSE;

        if(lex->GetToken(tk2) != ':')
            return FALSE;

        if(!rStatement(st2))
            return FALSE;

        st = new PtreeCaseStatement(new LeafReserved(tk1),
                                    Ptree::List(exp, new Leaf(tk2), st2));
        return TRUE;
    case DEFAULT :
        lex->GetToken(tk1);
        if(lex->GetToken(tk2) != ':')
            return FALSE;

        if(!rStatement(st2))
            return FALSE;

        st = new PtreeDefaultStatement(new LeafReserved(tk1),
                                       Ptree::List(new Leaf(tk2), st2));
        return TRUE;
    case Identifier :
        if(lex->LookAhead(1) == ':'){   // label statement
            lex->GetToken(tk1);
            lex->GetToken(tk2);
            if(!rStatement(st2))
                return FALSE;

            st = new PtreeLabelStatement(new Leaf(tk1),
                                         Ptree::List(new Leaf(tk2), st2));
            return TRUE;
        }
        // don't break here!
    default :
        return rExprStatement(st);
    }
}

/*
  if.statement
  : IF '(' comma.expression ')' statement { ELSE statement }
*/
bool Parser::rIfStatement(Ptree*& st)
{
    Token tk1, tk2, tk3, tk4;
    Ptree *exp, *then, *otherwise;

    if(lex->GetToken(tk1) != IF)
        return FALSE;

    if(lex->GetToken(tk2) != '(')
        return FALSE;

    if(!rCommaExpression(exp))
        return FALSE;

    if(lex->GetToken(tk3) != ')')
        return FALSE;

    if(!rStatement(then))
        return FALSE;

    st = new PtreeIfStatement(new LeafReserved(tk1),
                              Ptree::List(new Leaf(tk2), exp, new Leaf(tk3),
                                          then));
    if(lex->LookAhead(0) == ELSE){
        lex->GetToken(tk4);
        if(!rStatement(otherwise))
            return FALSE;

        st = Ptree::Nconc(st, Ptree::List(new Leaf(tk4), otherwise));
    }

    return TRUE;
}

/*
  switch.statement
  : SWITCH '(' comma.expression ')' statement
*/
bool Parser::rSwitchStatement(Ptree*& st)
{
    Token tk1, tk2, tk3;
    Ptree *exp, *body;

    if(lex->GetToken(tk1) != SWITCH)
        return FALSE;

    if(lex->GetToken(tk2) != '(')
        return FALSE;

    if(!rCommaExpression(exp))
        return FALSE;

    if(lex->GetToken(tk3) != ')')
        return FALSE;

    if(!rStatement(body))
        return FALSE;

    st = new PtreeSwitchStatement(new LeafReserved(tk1),
                                  Ptree::List(new Leaf(tk2), exp,
                                              new Leaf(tk3), body));
    return TRUE;
}

/*
  while.statement
  : WHILE '(' comma.expression ')' statement
*/
bool Parser::rWhileStatement(Ptree*& st)
{
    Token tk1, tk2, tk3;
    Ptree *exp, *body;

    if(lex->GetToken(tk1) != WHILE)
        return FALSE;

    if(lex->GetToken(tk2) != '(')
        return FALSE;

    if(!rCommaExpression(exp))
        return FALSE;

    if(lex->GetToken(tk3) != ')')
        return FALSE;

    if(!rStatement(body))
        return FALSE;

    st = new PtreeWhileStatement(new LeafReserved(tk1),
                                 Ptree::List(new Leaf(tk2), exp,
                                              new Leaf(tk3), body));
    return TRUE;
}

/*
  do.statement
  : DO statement WHILE '(' comma.expression ')' ';'
*/
bool Parser::rDoStatement(Ptree*& st)
{
    Token tk0, tk1, tk2, tk3, tk4;
    Ptree *exp, *body;

    if(lex->GetToken(tk0) != DO)
        return FALSE;

    if(!rStatement(body))
        return FALSE;

    if(lex->GetToken(tk1) != WHILE)
        return FALSE;

    if(lex->GetToken(tk2) != '(')
        return FALSE;

    if(!rCommaExpression(exp))
        return FALSE;

    if(lex->GetToken(tk3) != ')')
        return FALSE;

    if(lex->GetToken(tk4) != ';')
        return FALSE;

    st = new PtreeDoStatement(new LeafReserved(tk0),
                                 Ptree::List(body, new LeafReserved(tk1),
                                             new Leaf(tk2), exp,
                                             new Leaf(tk3), new Leaf(tk4)));
    return TRUE;
}

/*
  for.statement
  : FOR '(' expr.statement {comma.expression} ';' {comma.expression} ')'
    statement
*/
bool Parser::rForStatement(Ptree*& st)
{
    Token tk1, tk2, tk3, tk4;
    Ptree *exp1, *exp2, *exp3, *body;

    if(lex->GetToken(tk1) != FOR)
        return FALSE;

    if(lex->GetToken(tk2) != '(')
        return FALSE;

    if(!rExprStatement(exp1))
        return FALSE;

    if(lex->LookAhead(0) == ';')
        exp2 = nil;
    else
        if(!rCommaExpression(exp2))
            return FALSE;

    if(lex->GetToken(tk3) != ';')
        return FALSE;

    if(lex->LookAhead(0) == ')')
        exp3 = nil;
    else
        if(!rCommaExpression(exp3))
            return FALSE;

    if(lex->GetToken(tk4) != ')')
        return FALSE;

    if(!rStatement(body))
        return FALSE;


    st = new PtreeForStatement(new LeafReserved(tk1),
                               Ptree::List(new Leaf(tk2), exp1, exp2,
                                           new Leaf(tk3), exp3,
                                           new Leaf(tk4), body));
    return TRUE;
}

/*
  try.statement
  : TRY compound.statement (exception.handler)+ ';'

  exception.handler
  : CATCH '(' (arg.declaration | Ellipsis) ')' compound.statement
*/
bool Parser::rTryStatement(Ptree*& st)
{
    Token tk, op, cp;
    Ptree *body, *handler;

    if(lex->GetToken(tk) != TRY)
        return FALSE;

    if(!rCompoundStatement(body))
        return FALSE;

    st = new PtreeTryStatement(new LeafReserved(tk), Ptree::List(body));

    do{
        if(lex->GetToken(tk) != CATCH)
            return FALSE;

        if(lex->GetToken(op) != '(')
            return FALSE;

        if(lex->LookAhead(0) == Ellipsis){
            lex->GetToken(cp);
            handler = new Leaf(cp);
        }
        else{
            Encoding encode;
            if(!rArgDeclaration(handler, encode))
                return FALSE;
        }

        if(lex->GetToken(cp) != ')')
            return FALSE;

        if(!rCompoundStatement(body))
            return FALSE;

        st = Ptree::Snoc(st, Ptree::List(new LeafReserved(tk),
                                         new Leaf(op), handler, new Leaf(cp),
                                         body));
    } while(lex->LookAhead(0) == CATCH);
    return TRUE;
}

/*
  expr.statement
  : ';'
  | declaration.statement
  | comma.expression ';'
  | openc++.postfix.expr
  | openc++.primary.exp
*/
bool Parser::rExprStatement(Ptree*& st)
{
    Token tk;

    if(lex->LookAhead(0) == ';'){
        lex->GetToken(tk);
        st = new PtreeExprStatement(nil, Ptree::List(new Leaf(tk)));
        return TRUE;
    }
    else{
        char* pos = lex->Save();
        if(rDeclarationStatement(st))
            return TRUE;
        else{
            Ptree* exp;
            lex->Restore(pos);
            if(!rCommaExpression(exp))
                return FALSE;

            if(exp->IsA(ntUserStatementExpr, ntStaticUserStatementExpr)){
                st = exp;
                return TRUE;
            }

            if(lex->GetToken(tk) != ';')
                return FALSE;

            st = new PtreeExprStatement(exp, Ptree::List(new Leaf(tk)));
            return TRUE;
        }
    }
}

/*
  declaration.statement
  : decl.head integral.or.class.spec {cv.qualify} {declarators} ';'
  | decl.head name {cv.qualify} declarators ';'
  | const.declaration

  decl.head
  : {storage.spec} {cv.qualify}

  const.declaration
  : cv.qualify {'*'} Identifier '=' expression {',' declarators} ';'

  Note: if you modify this function, take a look at rDeclaration(), too.
*/
bool Parser::rDeclarationStatement(Ptree*& statement)
{
    Ptree *storage_s, *cv_q, *integral;
    Encoding type_encode;

    if(!optStorageSpec(storage_s)
       || !optCvQualify(cv_q)
       || !optIntegralTypeOrClassSpec(integral, type_encode))
        return FALSE;

    Ptree* head = nil;
    if(storage_s != nil)
        head = Ptree::Snoc(head, storage_s);

    if(integral != nil)
        return rIntegralDeclStatement(statement, type_encode, integral,
                                      cv_q, head);
    else{
        type_encode.Clear();
        int t = lex->LookAhead(0);
        if(cv_q != nil && ((t == Identifier && lex->LookAhead(1) == '=')
                           || t == '*'))
            return rConstDeclaration(statement, type_encode, head, cv_q);
        else
            return rOtherDeclStatement(statement, type_encode, cv_q, head);
    }
}

/*
  integral.decl.statement
  : decl.head integral.or.class.spec {cv.qualify} {declarators} ';'
*/
bool Parser::rIntegralDeclStatement(Ptree*& statement, Encoding& type_encode,
                                    Ptree* integral, Ptree* cv_q, Ptree* head)
{
    Ptree *cv_q2, *decl;
    Token tk;

    if(!optCvQualify(cv_q2))
        return FALSE;

    if(cv_q != nil)
        if(cv_q2 == nil)
            integral = Ptree::Snoc(cv_q, integral);
        else
            integral = Ptree::Nconc(cv_q, Ptree::Cons(integral, cv_q2));
    else if(cv_q2 != nil)
        integral = Ptree::Cons(integral, cv_q2);

    type_encode.CvQualify(cv_q, cv_q2);
    if(lex->LookAhead(0) == ';'){
        lex->GetToken(tk);
        statement = new PtreeDeclaration(head, Ptree::List(integral,
                                                           new Leaf(tk)));
        return TRUE;
    }
    else{
        if(!rDeclarators(decl, type_encode, FALSE, TRUE))
            return FALSE;

        if(lex->GetToken(tk) != ';')
            return FALSE;

        statement = new PtreeDeclaration(head, Ptree::List(integral, decl,
                                                           new Leaf(tk)));
        return TRUE;
    }
}

/*
   other.decl.statement
   :decl.head name {cv.qualify} declarators ';'
*/
bool Parser::rOtherDeclStatement(Ptree*& statement, Encoding& type_encode,
                                 Ptree* cv_q, Ptree* head)
{
    Ptree *type_name, *cv_q2, *decl;
    Token tk;

    if(!rName(type_name, type_encode))
        return FALSE;

    if(!optCvQualify(cv_q2))
        return FALSE;

    if(cv_q != nil)
        if(cv_q2 == nil)
            type_name = Ptree::Snoc(cv_q, type_name);
        else
            type_name = Ptree::Nconc(cv_q, Ptree::Cons(type_name, cv_q2));
    else if(cv_q2 != nil)
        type_name = Ptree::Cons(type_name, cv_q2);

    type_encode.CvQualify(cv_q, cv_q2);
    if(!rDeclarators(decl, type_encode, FALSE, TRUE))
        return FALSE;

    if(lex->GetToken(tk) != ';')
        return FALSE;

    statement = new PtreeDeclaration(head, Ptree::List(type_name, decl,
                                                       new Leaf(tk)));
    return TRUE;
}

bool Parser::MaybeTypeNameOrClassTemplate(Token&)
{
    return TRUE;
}

void Parser::SkipTo(int token)
{
    Token tk;

    for(;;){
        int t = lex->LookAhead(0);
        if(t == token || t == '\0')
            break;
        else
            lex->GetToken(tk);
    }
}

#ifdef TEST

#include "buffer.h"
#include "walker.h"

main()
{
    ProgramFromStdin    prog;
    Lex                 lex(&prog);
    Parser              parse(&lex);
    Walker              w(&parse);
    Ptree*              def;

    while(parse.rProgram(def)){
        def->Display2(cout);
        w.Translate(def);
    }

    cerr << parse.NumOfErrors() << " errors\n";
}
#endif
