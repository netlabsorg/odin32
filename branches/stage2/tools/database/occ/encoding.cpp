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
  This encoding is also interpreted by TypeInfo.  If you modify this
  file, check typeinfo.{h,cc} out as well.

  'b' boolean
  'c' char
  'i' int (signed, unsigned)
  's' short (short int)
  'l' long (long int)
  'j' long long
  'f' float
  'd' double
  'r' long double
  'v' void

  'T' template class (e.g. Foo<int,char> ==> T[3]Foo[2]ic.  [2] means
      the length of "ic".  It doesn't mean the number of template
      arguments.
  'e' ...
  '?' no return type.  the return type of constructors
  '*' non-type template parameter

  'S' singned
  'U' unsigned
  'C' const
  'V' volatile

  'P' pointer
  'R' reference
  'A' []
  'F' function (e.g. char foo(int) ==> Fi_c)
  'M' pointer to member (e.g. Type::* ==> M[4]Type)

  'Q' qualified class (e.g. X::YY ==> Q[2][1]X[2]YY, ::YY ==> Q[2][0][2]YY)

  [x] means (0x80 + x)
  '0' means :: (global scope)

  Special function names:

  operator + ==> +
  operator new[] ==> new[]
  operator <type> ==> @<encoded type>		cast operator
*/

#include <string.h>
#include <iostream.h>
#include "encoding.h"
#include "token.h"
#include "ptree.h"
#include "env.h"
#include "mop.h"
#include "typeinfo.h"

Ptree* Encoding::bool_t = new LeafBOOLEAN("bool", 4);
Ptree* Encoding::char_t = new LeafCHAR("char", 4);
Ptree* Encoding::int_t = new LeafINT("int", 3);
Ptree* Encoding::short_t = new LeafSHORT("short", 5);
Ptree* Encoding::long_t = new LeafLONG("long", 4);
Ptree* Encoding::float_t = new LeafFLOAT("float", 5);
Ptree* Encoding::double_t = new LeafDOUBLE("double", 6);
Ptree* Encoding::void_t = new LeafVOID("void", 4);

Ptree* Encoding::signed_t = new LeafSIGNED("signed", 6);
Ptree* Encoding::unsigned_t = new LeafUNSIGNED("unsigned", 8);
Ptree* Encoding::const_t = new LeafCONST("const", 5);
Ptree* Encoding::volatile_t = new LeafVOLATILE("volatile", 8);

Ptree* Encoding::operator_name = new LeafReserved("operator", 8);
Ptree* Encoding::new_operator = new LeafReserved("new", 3);
Ptree* Encoding::anew_operator = new LeafReserved("new[]", 5);
Ptree* Encoding::delete_operator = new LeafReserved("delete", 6);
Ptree* Encoding::adelete_operator = new LeafReserved("delete[]", 8);

Ptree* Encoding::star = new Leaf("*", 1);
Ptree* Encoding::ampersand = new Leaf("&", 1);
Ptree* Encoding::comma = new Leaf(",", 1);
Ptree* Encoding::dots = new Leaf("...", 3);
Ptree* Encoding::scope = new Leaf("::", 2);
Ptree* Encoding::tilder = new Leaf("~", 1);
Ptree* Encoding::left_paren = new Leaf("(", 1);
Ptree* Encoding::right_paren = new Leaf(")", 1);
Ptree* Encoding::left_bracket = new Leaf("[", 1);
Ptree* Encoding::right_bracket = new Leaf("]", 1);
Ptree* Encoding::left_angle = new Leaf("<", 1);
Ptree* Encoding::right_angle = new Leaf(">", 1);

const int DigitOffset = 0x80;

void Encoding::Reset(Encoding& e)
{
    len = e.len;
    if(len > 0)
	memmove(name, e.name, len);
}

char* Encoding::Get()
{
    if(len == 0)
	return nil;
    else{
	char* s = new (GC) char[len + 1];
	memmove(s, name, len);
	s[len] = '\0';
	return s;
    }
}

void Encoding::Print(ostream& s, char* p)
{
    unsigned char* ptr = (unsigned char*)p;
    for(;*ptr != '\0'; ++ptr)
	if(*ptr < 0x80)
	    s << char(*ptr);
	else
	    s << char(*ptr - 0x80 + '0');
}

// GetBaseName() returns "Foo" if ENCODE is "Q[2][1]X[3]Foo", for example.
// If an error occurs, the function returns nil.

char* Encoding::GetBaseName(char* encode, int& len, Environment*& env)
{
    if(encode == nil){
	len = 0;
	return nil;
    }

    Environment* e = env;
    unsigned char* p = (unsigned char*)encode;
    if(*p == 'Q'){
	int n = p[1] - 0x80;
	p += 2;
	while(n-- > 1){
	    int m = *p++;
	    if(m == 'T')
		m = GetBaseNameIfTemplate(p, e);
	    else if(m < 0x80){		// error?
		len = 0;
		return nil;
	    }
	    else{			// class name
		m -= 0x80;
		if(m <= 0){		// if global scope (e.g. ::Foo)
		    if(e != nil)
			e = e->GetBottom();
		}
		else
		    e = ResolveTypedefName(e, (char*)p, m);
	    }

	    p += m;
	}

	env = e;
    }

    if(*p == 'T'){		// template class
	int m = p[1] - 0x80;
	int n = p[m + 2] - 0x80;
	len = m + n + 3;
	return (char*)p;
    }
    else if(*p < 0x80){		// error?
	len = 0;
	return nil;
    }
    else{
	len = *p - 0x80;
	return (char*)p + 1;
    }
}

Environment* Encoding::ResolveTypedefName(Environment* env,
					  char* name, int len)
{
    TypeInfo tinfo;
    Bind* bind;
    Class* c = nil;

    if(env != nil)
	if (env->LookupType(name, len, bind) && bind != nil)
	    switch(bind->What()){
	    case Bind::isClassName :
		c = bind->ClassMetaobject();
		break;
	    case Bind::isTypedefName :
		bind->GetType(tinfo, env);
		c = tinfo.ClassMetaobject();
		/* if (c == nil) */
		    env = nil;
		break;
	    default :
		break;
	    }
	else {
	    env = env->LookupNamespace(name, len);
	    /* env is nil if name is an unknown typedef name or namespace.
	     */
	}

    if(c != nil)
	return c->GetEnvironment();
    else
	return env;
}

int Encoding::GetBaseNameIfTemplate(unsigned char* name, Environment*& e)
{
    int m = name[0] - 0x80;
    if(m <= 0)
	return name[1] - 0x80 + 2;

    Bind* b;
    if(e != nil && e->LookupType((char*)&name[1], m, b))
	if(b != nil && b->What() == Bind::isTemplateClass){
	    Class* c = b->ClassMetaobject();
	    if(c != nil){
		e = c->GetEnvironment();
		return m + (name[m + 1] - 0x80) + 2;
	    }
	}

    // the template name was not found.
    e = nil;
    return m + (name[m + 1] - 0x80) + 2;
}

unsigned char* Encoding::GetTemplateArguments(unsigned char* name, int& len)
{
    int m = name[0] - 0x80;
    if(m <= 0){
	len = name[1] - 0x80;
	return &name[2];
    }
    else{
	len = name[m + 1] - 0x80;
	return &name[m + 2];
    }
}

void Encoding::CvQualify(Ptree* cv1, Ptree* cv2)
{
    bool c = FALSE, v = FALSE;
    if(cv1 != nil && !cv1->IsLeaf())
	while(cv1 != nil){
	    int kind = cv1->Car()->What();
	    cv1 = cv1->Cdr();
	    if(kind == CONST)
		c = TRUE;
	    else if(kind == VOLATILE)
		v = TRUE;
	}

    if(cv2 != nil && !cv2->IsLeaf())
	while(cv2 != nil){
	    int kind = cv2->Car()->What();
	    cv2 = cv2->Cdr();
	    if(kind == CONST)
		c = TRUE;
	    else if(kind == VOLATILE)
		v = TRUE;
	}

    if(v)
	Insert('V');

    if(c)
	Insert('C');
}

void Encoding::GlobalScope()
{
    Append(DigitOffset);
}

// SimpleName() is also used for operator names

void Encoding::SimpleName(Ptree* id)
{
    AppendWithLen(id->GetPosition(), id->GetLength());
}

// NoName() generates a internal name for no-name enum and class
// declarations.

void Encoding::NoName()
{
    static int i = 0;
    static unsigned char name[] = "`0000";
    int n = i++;
    name[1] = n / 1000 + '0';
    name[2] = (n / 100) % 10 + '0';
    name[3] = (n / 10) % 10 + '0';
    name[4] = n % 10 + '0';
    AppendWithLen((char*)name, 5);
}

void Encoding::Template(Ptree* name, Encoding& args)
{
    Append('T');
    SimpleName(name);
    AppendWithLen(args);
}

void Encoding::Qualified(int n)
{
    if(len + 1 >= MaxNameLen)
	MopErrorMessage("Encoding::Qualified()",
			"too long encoded name");

    memmove(name + 2, name, len);
    len += 2;
    name[0] = 'Q';
    name[1] = (unsigned char)(DigitOffset + n);
}

void Encoding::Destructor(Ptree* class_name)
{
    int len = class_name->GetLength();
    Append((unsigned char)(DigitOffset + len + 1));
    Append('~');
    Append(class_name->GetPosition(), len);
}

void Encoding::PtrOperator(int t)
{
    if(t == '*')
	Insert('P');
    else
	Insert('R');
}

void Encoding::PtrToMember(Encoding& encode, int n)
{
    if(n < 2)
	Insert((char*)encode.name, encode.len);
    else{
	Insert((char*)encode.name, encode.len);
	Insert((unsigned char)(DigitOffset + n));
	Insert('Q');
    }

    Insert('M');
}

void Encoding::CastOperator(Encoding& type)
{
    Append((unsigned char)(DigitOffset + type.len + 1));
    Append('@');
    Append((char*)type.name, type.len);
}

void Encoding::Insert(unsigned char c)
{
    if(len >= MaxNameLen)
	MopErrorMessage("Encoding::Insert()",
			"too long encoded name");

    if(len > 0)
	memmove(name + 1, name, len);

    ++len;
    name[0] = c;
}

void Encoding::Insert(char* str, int n)
{
    if(len + n >= MaxNameLen)
	MopErrorMessage("Encoding::Insert()",
			"too long encoded name");

    if(len > 0)
	memmove(&name[n], name, len);

    memmove(name, str, n);
    len += n;
}

void Encoding::Append(unsigned char c)
{
    if(len >= MaxNameLen)
	MopErrorMessage("Encoding::Append()",
			"too long encoded name");

    name[len++] = c;
}

void Encoding::Append(char* str, int n)
{
    if(len + n >= MaxNameLen)
	MopErrorMessage("Encoding::Append(char*,int)",
			"too long encoded name");

    memmove(&name[len], str, n);
    len += n;
}

void Encoding::AppendWithLen(char* str, int n)
{
    if(len + n + 1 >= MaxNameLen)
	MopErrorMessage("Encoding::AppendWithLen()",
			"too long encoded name");

    name[len++] = (unsigned char)(DigitOffset + n);
    memmove(&name[len], str, n);
    len += n;
}

Ptree* Encoding::MakePtree(unsigned char*& encoded, Ptree* decl)
{
    Ptree* cv;
    Ptree* typespec = nil;
    if(decl != nil)
	decl = Ptree::List(decl);

    for(;;){
	cv = nil;
	switch(*encoded++){
	case 'b' :
	    typespec = Ptree::Snoc(typespec, bool_t);
	    goto finish;
	case 'c' :
	    typespec = Ptree::Snoc(typespec, char_t);
	    goto finish;
	case 'i' :
	    typespec = Ptree::Snoc(typespec, int_t);
	    goto finish;
	case 's' :
	    typespec = Ptree::Snoc(typespec, short_t);
	    goto finish;
	case 'l' :
	    typespec = Ptree::Snoc(typespec, long_t);
	    goto finish;
	    break;
	case 'j' :
	    typespec = Ptree::Nconc(typespec, Ptree::List(long_t, long_t));
	    goto finish;
	    break;
	case 'f' :
	    typespec = Ptree::Snoc(typespec, float_t);
	    goto finish;
	    break;
	case 'd' :
	    typespec = Ptree::Snoc(typespec, double_t);
	    goto finish;
	    break;
	case 'r' :
	    typespec = Ptree::Nconc(typespec, Ptree::List(long_t, double_t));
	    goto finish;
	case 'v' :
	    typespec = Ptree::Snoc(typespec, void_t);
	    goto finish;
	case 'e' :
	    return dots;
	case '?' :
	    goto finish;
	case 'Q' :
	    typespec = Ptree::Snoc(typespec, MakeQname(encoded));
	    goto finish;
	case 'S' :
	    typespec = Ptree::Snoc(typespec, signed_t);
	    break;
	case 'U' :
	    typespec = Ptree::Snoc(typespec, unsigned_t);
	    break;
	case 'C' :
	    if(*encoded == 'V'){
		++encoded;
		cv = Ptree::List(const_t, volatile_t);
	    }
	    else
		cv = Ptree::List(const_t);

	    goto const_or_volatile;
	case 'V' :
	    cv = Ptree::List(volatile_t);
	const_or_volatile :
	    switch(*encoded) {
	    case 'M' :
	    case 'P' :
	    case 'R' :
		decl = Ptree::Nconc(cv, decl);
		break;
	    case 'F' :
		++encoded;
		goto cv_function;
	    default :
		typespec = Ptree::Nconc(cv, typespec);
		break;
	    }
	    break;
	case 'M' :
	    {
		Ptree* ptr;
		if(*encoded == 'Q')
		    ptr = MakeQname(++encoded);
		else
		    ptr = MakeLeaf(encoded);

		ptr = Ptree::List(ptr, scope, star);
		decl = Ptree::Cons(ptr, decl);
	    }

	    goto pointer_or_reference;
	case 'P' :
	    decl = Ptree::Cons(star, decl);
	    goto pointer_or_reference;
	case 'R' :
	    decl = Ptree::Cons(ampersand, decl);
	pointer_or_reference :
	    if(*encoded == 'A' || *encoded == 'F')
		decl = Ptree::List(Ptree::List(left_paren, decl,
					       right_paren));

	    break;
	case 'A' :
	    decl = Ptree::Nconc(decl, Ptree::List(left_bracket,
						  right_bracket));
	    break;
	case 'F' :
	cv_function :
	    {
		Ptree* args = nil;
		while(*encoded != '\0'){
		    if(*encoded == '_'){
			++encoded;
			break;
		    }
		    else if(*encoded == 'v'){
			encoded += 2;
			break;
		    }

		    if(args != nil)
			args = Ptree::Snoc(args, comma);

		    args = Ptree::Snoc(args, MakePtree(encoded, nil));
		}

		decl = Ptree::Nconc(decl, Ptree::List(left_paren, args,
						      right_paren));
		if(cv != nil)
		    decl = Ptree::Nconc(decl, cv);
	    }
	    break;
	case '\0' :
	    goto finish;
	case 'T' :
	    {
    		Ptree* tlabel = MakeLeaf(encoded);      
	    	Ptree* args = nil;
	    	int n = *encoded++ - DigitOffset;
		unsigned char* stop = encoded + n;
		while(encoded < stop){
		    if(args != nil)
			args = Ptree::Snoc(args, comma);

		    args = Ptree::Snoc(args, MakePtree(encoded, nil));
		}

		tlabel = Ptree::List(tlabel,
				Ptree::List(left_angle, args, right_angle));
		typespec = Ptree::Nconc(typespec, tlabel);
		goto finish;
	    }
	case '*' :
	    goto error;
	default :
	    if(*--encoded >= DigitOffset){
		if(typespec == nil)
		    typespec = MakeLeaf(encoded);
		else
		    typespec = Ptree::Snoc(typespec, MakeLeaf(encoded));

		goto finish;
	    }
	error :
	    MopErrorMessage("TypeInfo::MakePtree()",
			    "sorry, cannot handle this type");
	    break;
	}
    }

finish :
    return Ptree::List(typespec, decl);
}

Ptree* Encoding::MakeQname(unsigned char*& encoded)
{
    int n = *encoded++ - DigitOffset;
    Ptree* qname = nil;
    while(n-- > 0){
	Ptree* leaf = MakeLeaf(encoded);
	if(leaf != nil)
	    qname = Ptree::Snoc(qname, leaf);

	if(n > 0)
	    qname = Ptree::Snoc(qname, scope);
    }

    return qname;
}

Ptree* Encoding::MakeLeaf(unsigned char*& encoded)
{
    Ptree* leaf;
    int len = *encoded++ - DigitOffset;
    if(len > 0)
	leaf = new Leaf((char*)encoded, len);
    else
	leaf = nil;

    encoded += len;
    return leaf;
}

bool Encoding::IsSimpleName(unsigned char* p)
{
    return *p >= DigitOffset;
}

Ptree* Encoding::NameToPtree(char* name, int len)
{
    if(name == nil)
	return nil;

    if(name[0] == 'n'){
	if(len == 5 && strncmp(name, "new[]", 5) == 0)
	    return Ptree::List(operator_name, anew_operator);
	else if(len == 3 && strncmp(name, "new", 3) == 0)
	    return Ptree::List(operator_name, new_operator);
    }
    else if(name[0] == 'd'){
	if(len == 8 && strncmp(name, "delete[]", 8) == 0)
	    return Ptree::List(operator_name, adelete_operator);
	else if(len == 6 && strncmp(name, "delete", 6) == 0)
	    return Ptree::List(operator_name, delete_operator);
    }
    else if(name[0] == '~')
	return Ptree::List(tilder, new Leaf(&name[1], len - 1));
    else if(name[0] == '@'){		// cast operator
	unsigned char* encoded = (unsigned char*)&name[1];
	return Ptree::List(operator_name, MakePtree(encoded, nil));
    }

    if(is_letter(name[0]))
	return new Leaf(name, len);
    else
	return Ptree::List(operator_name, new Leaf(name, len));
}

