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

#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "hash.h"
#include "ptree.h"
#include "buffer.h"

#if defined(_PARSE_VCC)
#define _MSC_VER	1100
#endif

#if defined(_MSC_VER) || defined(OS2) //kso: OS2
#include <assert.h>		// for assert in InitializeOtherKeywords
#endif

extern BOOL regularCpp;		// defined in main.cc
static void InitializeOtherKeywords();

#ifdef TEST

#ifdef __GNUG__
#define token(x)	(long)#x
#else
#define token(x)	(long)"x"
#endif

#else

#define token(x)	x

#endif

// class Lex

HashTable* Lex::user_keywords = nil;
Ptree* Lex::comments = nil;

Lex::Lex(Program* prog) : fifo(this)
{
    file = prog;
    prog->Rewind();
    last_token = '\n';
    tokenp = 0;
    token_len = 0;

    InitializeOtherKeywords();
}

char* Lex::Save()
{
    char* pos;
    int len;

    fifo.Peek(0, pos, len);
    return pos;
}

void Lex::Restore(char* pos)
{
    last_token = '\n';
    tokenp = 0;
    token_len = 0;
    fifo.Clear();
    Rewind(pos);
}

// ">>" is either the shift operator or double closing brackets.

void Lex::GetOnlyClosingBracket(Token& t)
{
    Restore(t.ptr + 1);
}

uint Lex::LineNumber(char* pos, char*& ptr, int& len)
{
    return file->LineNumber(pos, ptr, len);
}

int Lex::GetToken(Token& t)
{
    t.kind = fifo.Pop(t.ptr, t.len);
    return t.kind;
}

int Lex::LookAhead(int offset)
{
    return fifo.Peek(offset);
}

int Lex::LookAhead(int offset, Token& t)
{
    t.kind = fifo.Peek(offset, t.ptr, t.len);
    return t.kind;
}

char* Lex::TokenPosition()
{
    return (char*)file->Read(Tokenp());
}

char Lex::Ref(uint i)
{
    return file->Ref(i);
}

void Lex::Rewind(char* p)
{
    file->Rewind(p - file->Read(0));
}

bool Lex::RecordKeyword(char* keyword, int token)
{
    int index;
    char* str;

    if(keyword == nil)
	return FALSE;

    str = new(GC) char[strlen(keyword) + 1];
    strcpy(str, keyword);

    if(user_keywords == nil)
	user_keywords = new HashTable;

    if(user_keywords->AddEntry(str, (HashValue)token, &index) >= 0)
	return TRUE;
    else
	return bool(user_keywords->Peek(index) == (HashValue)token);
}

bool Lex::Reify(Ptree* t, unsigned int& value)
{
    if(t == nil || !t->IsLeaf())
	return FALSE;

    char* p = t->GetPosition();
    int len = t->GetLength();
    value = 0;
    if(len > 2 && *p == '0' && is_xletter(p[1])){
	for(int i = 2; i < len; ++i){
	    char c = p[i];
	    if(is_digit(c))
		value = value * 0x10 + (c - '0');
	    else if('A' <= c && c <= 'F')
		value = value * 0x10 + (c - 'A' + 10);
	    else if('a' <= c && c <= 'f')
		value = value * 0x10 + (c - 'a' + 10);
	    else if(is_int_suffix(c))
		break;
	    else
		return FALSE;
	}

	return TRUE;
    }
    else if(len > 0 && is_digit(*p)){
	for(int i = 0; i < len; ++i){
	    char c = p[i];
	    if(is_digit(c))
		value = value * 10 + c - '0';
	    else if(is_int_suffix(c))
		break;
	    else
		return FALSE;
	}

	return TRUE;
    }
    else
	return FALSE;
}

// Reify() doesn't interpret an escape character.

bool Lex::Reify(Ptree* t, char*& str)
{
    if(t == nil || !t->IsLeaf())
	return FALSE;

    char* p = t->GetPosition();
    int length = t->GetLength();
    if(*p != '"')
	return FALSE;
    else{
	str = new(GC) char[length];
	char* sp = str;
	for(int i = 1; i < length; ++i)
	    if(p[i] != '"'){
		*sp++ = p[i];
		if(p[i] == '\\' && i + 1 < length)
		    *sp++ = p[++i];
	    }
	    else
		while(++i < length && p[i] != '"')
		    ;

	*sp = '\0';
	return TRUE;
    }
}

// class TokenFifo

Lex::TokenFifo::TokenFifo(Lex* l)
{
    lex = l;
    size = 16;
    ring = new (GC) Slot[size];
    head = tail = 0;
}

Lex::TokenFifo::~TokenFifo()
{
    // delete [] ring;
}

void Lex::TokenFifo::Clear()
{
    head = tail = 0;
}

void Lex::TokenFifo::Push(int token, char* pos, int len)
{
    const int Plus = 16;
    ring[head].token = token;
    ring[head].pos = pos;
    ring[head].len = len;
    head = (head + 1) % size;
    if(head == tail){
	Slot* ring2 = new (GC) Slot[size + Plus];
        int i = 0;
	do{
	    ring2[i++] = ring[tail];
	    tail = (tail + 1) % size;
	} while(head != tail);
	head = i;
	tail = 0;
	size += Plus;
	// delete [] ring;
	ring = ring2;
    }
}

int Lex::TokenFifo::Pop(char*& pos, int& len)
{
    if(head == tail)
	return lex->ReadToken(pos, len);

    int t = ring[tail].token;
    pos = ring[tail].pos;
    len = ring[tail].len;
    tail = (tail + 1) % size;
    return t;
}

int Lex::TokenFifo::Peek(int offset)
{
    return ring[Peek2(offset)].token;
}

int Lex::TokenFifo::Peek(int offset, char*& pos, int& len)
{
    int cur = Peek2(offset);
    pos = ring[cur].pos;
    len = ring[cur].len;
    return ring[cur].token;
}

int Lex::TokenFifo::Peek2(int offset)
{
    int i;
    int cur = tail;

    for(i = 0; i <= offset; ++i){
	if(head == cur){
	    while(i++ <= offset){
		char* p;
		int   l;
		int t = lex->ReadToken(p, l);
		Push(t, p, l);
	    }

	    break;
	}

	cur = (cur + 1) % size;
    }

    return (tail + offset) % size;
}

/*
  Lexical Analyzer
*/

int Lex::ReadToken(char*& ptr, int& len)
{
    int t;

    for(;;){
	t = ReadLine();

        if(t == Ignore)
	    continue;

	last_token = t;

#if defined(__GNUG__) || defined(_GNUG_SYNTAX)
	if(t == ATTRIBUTE){
	    SkipAttributeToken();
	    continue;
	}
	else if(t == EXTENSION){
	    t = SkipExtensionToken(ptr, len);
	    if(t == Ignore)
		continue;
	    else
		return t;
	}
#endif
#if defined(_MSC_VER) || defined(OS2) //kso: OS2
        if(t == ASM){
            SkipAsmToken();
	    continue;
	}
        else if(t == DECLSPEC){
	    SkipDeclspecToken();
	    continue;
	}
#endif
	if(t != '\n')
	    break;
    }

    ptr = TokenPosition();
    len = TokenLen();
    return t;
}

//   SkipAttributeToken() skips __attribute__(...), ___asm__(...), ...

void Lex::SkipAttributeToken()
{
    char c;

    do{
	c = file->Get();
    }while(c != '(' && c != '\0');

    int i = 1;
    do{
	c = file->Get();
	if(c == '(')
	    ++i;
	else if(c == ')')
	    --i;
	else if(c == '\0')
	    break;
    } while(i > 0);
}

// SkipExtensionToken() skips __extension__(...).

int Lex::SkipExtensionToken(char*& ptr, int& len)
{
    ptr = TokenPosition();
    len = TokenLen();

    char c;

    do{
	c = file->Get();
    }while(is_blank(c) || c == '\n');

    if(c != '('){
	file->Unget();
	return Ignore;		// if no (..) follows, ignore __extension__
    }

    int i = 1;
    do{
	c = file->Get();
	if(c == '(')
	    ++i;
	else if(c == ')')
	    --i;
	else if(c == '\0')
	    break;
    } while(i > 0);

    return Identifier;	// regards it as the identifier __extension__
}

#if defined(_MSC_VER) || defined(OS2)  //kso: os2

#define CHECK_END_OF_INSTRUCTION(C, EOI) \
	if (C == '\0') return; \
	if (strchr(EOI, C)) { \
	    this->file->Unget(); \
	    return; \
	}

/* SkipAsmToken() skips __asm ...
   You can have the following :

   Just count the '{' and '}' and it should be ok
   __asm { mov ax,1
           mov bx,1 }

   Stop when EOL found. Note that the first ';' after
   an __asm instruction is an ASM comment !
   int v; __asm mov ax,1 __asm mov bx,1; v=1;

   Stop when '}' found
   if (cond) {__asm mov ax,1 __asm mov bx,1}

   and certainly more...
*/
void Lex::SkipAsmToken()
{
    char c;

    do{
	c = file->Get();
	CHECK_END_OF_INSTRUCTION(c, "");
    }while(is_blank(c) || c == '\n');

    if(c == '{'){
        int i = 1;
        do{
	    c = file->Get();
	    CHECK_END_OF_INSTRUCTION(c, "");
	    if(c == '{')
	        ++i;
	    else if(c == '}')
	        --i;
        } while(i > 0);
    }
    else{
        for(;;){
	    CHECK_END_OF_INSTRUCTION(c, "}\n");
	    c = file->Get();
        }
    }
}

//   SkipDeclspecToken() skips __declspec(...).

void Lex::SkipDeclspecToken()
{
    char c;

    do{
	c = file->Get();
	CHECK_END_OF_INSTRUCTION(c, "");
    }while(is_blank(c));

    if (c == '(') {
        int i = 1;
        do{
	    c = file->Get();
	    CHECK_END_OF_INSTRUCTION(c, "};");
	    if(c == '(')
	        ++i;
	    else if(c == ')')
	        --i;
        }while(i > 0);
    }
}

#undef CHECK_END_OF_INSTRUCTION

#endif /* _MSC_VER */

char Lex::GetNextNonWhiteChar()
{
    char c;

    for(;;){
        do{
	    c = file->Get();
        }while(is_blank(c));

        if(c != '\\')
	    break;

        c = file->Get();
        if(c != '\n' && c!= '\r') {
	    file->Unget();
	    break;
	}
    }

    return c;
}

int Lex::ReadLine()
{
    char c;
    uint top;

    c = GetNextNonWhiteChar();

    tokenp = top = file->GetCurPos();
    if(c == '\0'){
	file->Unget();
	return '\0';
    }
    else if(c == '\n')
	return '\n';
    else if(c == '#' && last_token == '\n'){
	if(ReadLineDirective())
	    return '\n';
	else{
	    file->Rewind(top + 1);
	    token_len = 1;
	    return SingleCharOp(c);
	}
    }
    else if(c == '\'' || c == '"'){
	if(c == '\''){
	    if(ReadCharConst(top))
		return token(CharConst);
	}
	else{
	    if(ReadStrConst(top))
		return token(StringL);
	}

	file->Rewind(top + 1);
	token_len = 1;
	return SingleCharOp(c);
    }
    else if(is_digit(c))
	return ReadNumber(c, top);
    else if(c == '.'){
	c = file->Get();
	if(is_digit(c))
	    return ReadFloat(top);
	else{
	    file->Unget();
	    return ReadSeparator('.', top);
	}
    }
    else if(is_letter(c))
	return ReadIdentifier(top);
    else
	return ReadSeparator(c, top);
}

bool Lex::ReadCharConst(uint top)
{
    char c;

    for(;;){
	c = file->Get();
	if(c == '\\'){
	    c = file->Get();
	    if(c == '\0')
		return FALSE;
	}
	else if(c == '\''){
	    token_len = int(file->GetCurPos() - top + 1);
	    return TRUE;
	}
	else if(c == '\n' || c == '\0')
	    return FALSE;
    }
}

/*
  If text is a sequence of string constants like:
	"string1" "string2"
  then the string constants are delt with as a single constant.
*/
bool Lex::ReadStrConst(uint top)
{
    char c;

    for(;;){
	c = file->Get();
	if(c == '\\'){
	    c = file->Get();
	    if(c == '\0')
		return FALSE;
	}
	else if(c == '"'){
	    uint pos = file->GetCurPos() + 1;
	    int nline = 0;
	    do{
		c = file->Get();
		if(c == '\n')
		    ++nline;
	    } while(is_blank(c) || c == '\n');

	    if(c == '"')
		/* line_number += nline; */ ;
	    else{
		token_len = int(pos - top);
		file->Rewind(pos);
		return TRUE;
	    }
	}
	else if(c == '\n' || c == '\0')
	    return FALSE;
    }
}

int Lex::ReadNumber(char c, uint top)
{
    char c2 = file->Get();

    if(c == '0' && is_xletter(c2)){
	do{
	    c = file->Get();
	} while(is_hexdigit(c));
	while(is_int_suffix(c))
	    c = file->Get();

	file->Unget();
	token_len = int(file->GetCurPos() - top + 1);
	return token(Constant);
    }

    while(is_digit(c2))
	c2 = file->Get();

    if(is_int_suffix(c2))
	do{
	    c2 = file->Get();
	}while(is_int_suffix(c2));
    else if(c2 == '.')
	return ReadFloat(top);
    else if(is_eletter(c2)){
	file->Unget();
	return ReadFloat(top);
    }

    file->Unget();
    token_len = int(file->GetCurPos() - top + 1);
    return token(Constant);
}

int Lex::ReadFloat(uint top)
{
    char c;

    do{
	c = file->Get();
    }while(is_digit(c));
    if(is_float_suffix(c))
	do{
	    c = file->Get();
	}while(is_float_suffix(c));
    else if(is_eletter(c)){
	uint p = file->GetCurPos();
	c = file->Get();
	if(c == '+' || c == '-'){
	     c = file->Get();
	     if(!is_digit(c)){
		file->Rewind(p);
		token_len = int(p - top);
		return token(Constant);
	    }
	}
	else if(!is_digit(c)){
	    file->Rewind(p);
	    token_len = int(p - top);
	    return token(Constant);
	}

	do{
	    c = file->Get();
	}while(is_digit(c));

	while(is_float_suffix(c))
	    c = file->Get();
    }

    file->Unget();
    token_len = int(file->GetCurPos() - top + 1);
    return token(Constant);
}

// ReadLineDirective() simply ignores a line beginning with '#'

bool Lex::ReadLineDirective()
{
    char c;

    do{
	c = file->Get();
    }while(c != '\n' && c != '\0');
    return TRUE;
}

int Lex::ReadIdentifier(uint top)
{
    char c;

    do{
	c = file->Get();
    }while(is_letter(c) || is_digit(c));

    uint len = file->GetCurPos() - top;
    token_len = int(len);
    file->Unget();

    return Screening((char*)file->Read(top), int(len));
}

/*
  This table is a list of reserved key words.
  Note: alphabetical order!
*/
static struct rw_table {
    char*	name;
    long	value;
} table[] = {
#if defined(__GNUG__) || defined(_GNUG_SYNTAX)
    { "__alignof__",	token(SIZEOF) },
    { "__asm__",	token(ATTRIBUTE) },
    { "__attribute__",	token(ATTRIBUTE) },
    { "__const",	token(CONST) },
    { "__const__",	token(Ignore) }, //kso: gcc supports this AFAIK
    { "__extension__",	token(EXTENSION) },
    { "__inline__",	token(INLINE) },
    { "__restrict",	token(Ignore) },
    { "__signed",	token(SIGNED) },
    { "__signed__",	token(SIGNED) },
#endif
    { "asm",		token(ATTRIBUTE) },
    { "auto",		token(AUTO) },
#if !defined(_MSC_VER) || (_MSC_VER >= 1100)
    { "bool",		token(BOOLEAN) },
#endif
    { "break",		token(BREAK) },
    { "case",		token(CASE) },
    { "catch",		token(CATCH) },
    { "char",		token(CHAR) },
    { "class",		token(CLASS) },
    { "const",		token(CONST) },
    { "continue",	token(CONTINUE) },
    { "default",	token(DEFAULT) },
    { "delete",		token(DELETE) },
    { "do",		token(DO) },
    { "double",		token(DOUBLE) },
    { "else",		token(ELSE) },
    { "enum",		token(ENUM) },
    { "extern",		token(EXTERN) },
    { "float",		token(FLOAT) },
    { "for",		token(FOR) },
    { "friend",		token(FRIEND) },
    { "goto",		token(GOTO) },
    { "if",		token(IF) },
    { "inline",		token(INLINE) },
    { "int",		token(INT) },
    { "long",		token(LONG) },
    { "metaclass",	token(METACLASS) },	// OpenC++
    { "mutable",	token(MUTABLE) },
    { "namespace",	token(NAMESPACE) },
    { "new",		token(NEW) },
    { "operator",	token(OPERATOR) },
    { "private",	token(PRIVATE) },
    { "protected",	token(PROTECTED) },
    { "public",		token(PUBLIC) },
    { "register",	token(REGISTER) },
    { "return",		token(RETURN) },
    { "short",		token(SHORT) },
    { "signed",		token(SIGNED) },
    { "sizeof",		token(SIZEOF) },
    { "static",		token(STATIC) },
    { "struct",		token(STRUCT) },
    { "switch",		token(SWITCH) },
    { "template",	token(TEMPLATE) },
    { "this",		token(THIS) },
    { "throw",		token(THROW) },
    { "try",		token(TRY) },
    { "typedef",	token(TYPEDEF) },
    { "typename",	token(CLASS) },	// it's not identical to class, but...
    { "union",		token(UNION) },
    { "unsigned",	token(UNSIGNED) },
    { "using",		token(USING) },
    { "virtual",	token(VIRTUAL) },
    { "void",		token(VOID) },
    { "volatile",	token(VOLATILE) },
    { "while",		token(WHILE) },
    /* NULL slot */
};

static void InitializeOtherKeywords()
{
    static BOOL done = FALSE;

    if(done)
	return;
    else
	done = TRUE;

    if(regularCpp)
	for(unsigned int i = 0; i < sizeof(table) / sizeof(table[0]); ++i)
	    if(table[i].value == METACLASS){
		table[i].value = Identifier;
		break;
	    }

#if defined(_MSC_VER) || defined(OS2)   //kso: OS2
    assert(Lex::RecordKeyword("cdecl", Ignore));
    assert(Lex::RecordKeyword("_cdecl", Ignore));
    assert(Lex::RecordKeyword("__cdecl", Ignore));

    assert(Lex::RecordKeyword("_fastcall", Ignore));
    assert(Lex::RecordKeyword("__fastcall", Ignore));

    assert(Lex::RecordKeyword("_based", Ignore));
    assert(Lex::RecordKeyword("__based", Ignore));

    assert(Lex::RecordKeyword("_asm", ASM));
    assert(Lex::RecordKeyword("__asm", ASM));

    assert(Lex::RecordKeyword("_inline", INLINE));
    assert(Lex::RecordKeyword("__inline", INLINE));

    assert(Lex::RecordKeyword("_stdcall", Ignore));
    assert(Lex::RecordKeyword("__stdcall", Ignore));

    assert(Lex::RecordKeyword("__declspec", DECLSPEC));

    assert(Lex::RecordKeyword("__int8",  CHAR));
    assert(Lex::RecordKeyword("__int16", SHORT));
    assert(Lex::RecordKeyword("__int32", INT));
    assert(Lex::RecordKeyword("__int64",  INT64));
#endif
#if defined(OS2)                                                //kso
    bool fRc;                                                   //kso
    fRc = Lex::RecordKeyword("_Cdecl", Ignore);  assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_Far16", Ignore);  assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_Far32", Ignore);  assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_Optlink", Ignore);assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_Pascal", Ignore); assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_Seg16", Ignore);  assert(fRc);   //kso
    fRc = Lex::RecordKeyword("_System", Ignore); assert(fRc);   //kso
#endif                                                          //kso
}

int Lex::Screening(char *identifier, int len)
{
    struct rw_table	*low, *high, *mid;
    int			c, token;

    low = table;
    high = &table[sizeof(table) / sizeof(table[0]) - 1];
    while(low <= high){
	mid = low + (high - low) / 2;
	if((c = strncmp(mid->name, identifier, len)) == 0)
	    if(mid->name[len] == '\0')
		return mid->value;
	    else
		high = mid - 1;
	else if(c < 0)
	    low = mid + 1;
	else
	    high = mid - 1;
    }

    if(user_keywords == nil)
	user_keywords = new HashTable;

    if(user_keywords->Lookup(identifier, len, (HashValue*)&token))
	return token;

    return token(Identifier);
}

int Lex::ReadSeparator(char c, uint top)
{
    char c1 = file->Get();

    token_len = 2;
    if(c1 == '='){
	switch(c){
	case '*' :
	case '/' :
	case '%' :
	case '+' :
	case '-' :
	case '&' :
	case '^' :
	case '|' :
	    return token(AssignOp);
	case '=' :
	case '!' :
	    return token(EqualOp);
	case '<' :
	case '>' :
	    return token(RelOp);
	default :
	    file->Unget();
	    token_len = 1;
	    return SingleCharOp(c);
	}
    }
    else if(c == c1){
	switch(c){
	case '<' :
	case '>' :
	    if(file->Get() != '='){
		file->Unget();
		return token(ShiftOp);
	    }
	    else{
		token_len = 3;
		return token(AssignOp);
	    }
	case '|' :
	    return token(LogOrOp);
	case '&' :
	    return token(LogAndOp);
	case '+' :
	case '-' :
	    return token(IncOp);
	case ':' :
	    return token(Scope);
	case '.' :
	    if(file->Get() == '.'){
		token_len = 3;
		return token(Ellipsis);
	    }
	    else
		file->Unget();
	case '/' :
	    return ReadComment(c1, top);
	default :
	    file->Unget();
	    token_len = 1;
	    return SingleCharOp(c);
	}
    }
    else if(c == '.' && c1 == '*')
	return token(PmOp);
    else if(c == '-' && c1 == '>')
	if(file->Get() == '*'){
	    token_len = 3;
	    return token(PmOp);
	}
	else{
	    file->Unget();
	    return token(ArrowOp);
	}
    else if(c == '/' && c1 == '*')
	return ReadComment(c1, top);
    else{
	file->Unget();
	token_len = 1;
	return SingleCharOp(c);
    }

    cerr << "*** An invalid character has been found! ("
	 << (int)c << ',' << (int)c1 << ")\n";
    return token(BadToken);
}

int Lex::SingleCharOp(unsigned char c)
{
			/* !"#$%&'()*+,-./0123456789:;<=>? */
    static char valid[] = "x   xx xxxxxxxx          xxxxxx";

    if('!' <= c && c <= '?' && valid[c - '!'] == 'x')
	return c;
    else if(c == '[' || c == ']' || c == '^')
	return c;
    else if('{' <= c && c <= '~')
	return c;
    else
	return token(BadToken);
}

int Lex::ReadComment(char c, uint top) {
    uint len = 0;
    if (c == '*')	// a nested C-style comment is prohibited.
	do {
	    c = file->Get();
	    if (c == '*') {
		c = file->Get();
		if (c == '/') {
		    len = 1;
		    break;
		}
		else
		    file->Unget();
	    }
	}while(c != '\0');
    else /* if (c == '/') */
	do {
	    c = file->Get();
	}while(c != '\n' && c != '\0');

    len += file->GetCurPos() - top;
    token_len = int(len);
    Leaf* node = new Leaf((char*)file->Read(top), int(len));
    comments = Ptree::Snoc(comments, node);
    return Ignore;
}

Ptree* Lex::GetComments() {
    Ptree* c = comments;
    comments = nil;
    return c;
}

Ptree* Lex::GetComments2() {
    return comments;
}

#ifdef TEST
#include <stdio.h>

main()
{
    int   i = 0;
    Token token;

    Lex lex(new ProgramFromStdin);
    for(;;){
//	int t = lex.GetToken(token);
	int t = lex.LookAhead(i++, token);
	if(t == 0)
	    break;
	else if(t < 128)
	    printf("%c (%x): ", t, t);
	else
	    printf("%-10.10s (%x): ", (char*)t, t);

	putchar('"');
	while(token.len-- > 0)
	    putchar(*token.ptr++);

	puts("\"");
    };
}
#endif

/*

line directive:
^"#"{blank}*{digit}+({blank}+.*)?\n

pragma directive:
^"#"{blank}*"pragma".*\n

Constant	{digit}+{int_suffix}*
		"0"{xletter}{hexdigit}+{int_suffix}*
		{digit}*\.{digit}+{float_suffix}*
		{digit}+\.{float_suffix}*
		{digit}*\.{digit}+"e"("+"|"-")*{digit}+{float_suffix}*
		{digit}+\."e"("+"|"-")*{digit}+{float_suffix}*
		{digit}+"e"("+"|"-")*{digit}+{float_suffix}*

CharConst	\'([^'\n]|\\[^\n])\'

StringL		\"([^"\n]|\\["\n])*\"

Identifier	{letter}+({letter}|{digit})*

AssignOp	*= /= %= += -= &= ^= <<= >>=

EqualOp		== !=

RelOp		<= >=

ShiftOp		<< >>

LogOrOp		||

LogAndOp	&&

IncOp		++ --

Scope		::

Ellipsis	...

PmOp		.* ->*

ArrowOp		->

others		!%^&*()-+={}|~[];:<>?,./

BadToken	others

*/
