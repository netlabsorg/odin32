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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#if !defined(_MSC_VER)
#include <sys/time.h>
#endif
#include "ptree.h"
#include "token.h"
#include "mop.h"

#if (defined(sun) && defined(SUNOS4)) || defined(SUNOS5)
extern "C" {
    int gettimeofday(struct timeval*, void*);
};
#endif

const int MAX = 32;

static Ptree** resultsArgs[MAX];
static int resultsIndex;

static int CountArgs(char* pat);
static char* SkipSpaces(char* pat);

bool Ptree::Match(Ptree* list, char* pattern, ...)
{
    va_list args;
    int n = CountArgs(pattern);
    if(n >= MAX)
	MopErrorMessage("Ptree::Match()", "bomb! too many arguments");

    va_start(args, pattern);
    for(int i = 0; i < n; ++i)
	resultsArgs[i] = va_arg(args, Ptree**);

    va_end(args);

    char* pat = pattern;
    resultsIndex = 0;
    pat = SkipSpaces(pat);
    pat = MatchPat(list, pat);
    if(pat == nil)
	return FALSE;
    else{
	pat = SkipSpaces(pat);
	if(*pat == '\0')
	    return TRUE;
	else{
	    MopWarningMessage("Ptree::Match()", "[ ] are forgot?");
	    MopMoreWarningMessage(pattern);
	    return FALSE;
	}
    }
}

static int CountArgs(char* pat)
{
    int n = 0;

    for(char c = *pat; c != '\0'; c = *++pat)
	if(c == '%'){
	    c = *++pat;
	    if(c == '?' || c == 'r')
		++n;
	}

    return n;
}

char* Ptree::MatchPat(Ptree* list, char* pat)
{
    switch(*pat){
    case '[' :		/* [] means nil */
	if(list != nil && list->IsLeaf())
	    return nil;
	else
	    return MatchList(list, pat + 1);
    case '%' :
	switch(pat[1]){
	case '?' :
	    *resultsArgs[resultsIndex++] = list;
	    return(pat + 2);
	case '*' :
	    return(pat + 2);
	case '_' :
	case 'r' :	/* %_ and %r must be appear in a list */
	    return nil;
	default :
	    break;
	}
    }

    if(list != nil && list->IsLeaf())
	return MatchWord(list, pat);
    else
	return nil;
}

char* Ptree::MatchList(Ptree* list, char* pat)
{
    char c, d;
    pat = SkipSpaces(pat);
    while((c = *pat) != '\0'){
	if(c == ']')
	    if(list == nil)
		return(pat + 1);
	    else
		return nil;
	else if(c == '%' && (d = pat[1], (d == 'r' || d == '_'))){
	    /* %r or %_ */
	    if(d == 'r') 
		*resultsArgs[resultsIndex++] = list;

	    list = nil;
	    pat = pat + 2;
	}
	else if(list == nil)
	    return nil;
	else{
	    pat = MatchPat(list->Car(), pat);
	    if(pat == nil)
		return nil;

	    list = list->Cdr();
	}

	pat = SkipSpaces(pat);
    }

    MopErrorMessage("Ptree::Match()", "unmatched bracket");
    return nil;
}

char* Ptree::MatchWord(Ptree* list, char* pat)
{
    char* str = list->GetPosition();
    int str_len = list->GetLength();

    for(int j = 0; ; ++pat){
	char c = *pat;
	switch(c){
	case '\0' :
	case ' ' :
	case '\t' :
	case '[' :
	case ']' :
	    if(j == str_len)
		return pat;
	    else
		return nil;
	case '%' :
	    c = *++pat;
	    switch(c){
	    case '[' :
	    case ']' :
	    case '%' :
		if(j >= str_len || c != str[j++])
		    return nil;

		break;
	    default :
		if(j == str_len)
		    return pat;
		else
		    return nil;
	    }
	    break;
	default :
	    if(j >= str_len || c != str[j++])
		return nil;
	}
    }
}

static char* SkipSpaces(char* pat)
{
    while(*pat == ' ' || *pat == '\t')
	++pat;

    return pat;
}

Ptree* Ptree::GenSym()
{
#if !defined(_MSC_VER)
    struct timeval time;
#endif
    static char head[] = "_sym";
    static int seed = 1;
    int len1, len2;

    IntegerToString(seed, len1);

#if !defined(_MSC_VER)
    gettimeofday(&time, NULL);
    uint rnum = (time.tv_sec * 10 + time.tv_usec / 100) & 0xffff;
#else
    static uint time = 0;
    time++;
    uint rnum = time & 0xffff;
#endif
    char* num = IntegerToString(rnum, len2);

    int size = len1 + len2 + sizeof(head) - 1 + 1;
    char* name = new (GC) char[size];
    memmove(name, head, sizeof(head) - 1);
    memmove(&name[sizeof(head) - 1], num, len2);
    name[sizeof(head) - 1 + len2] = '_';
    num = IntegerToString(seed++, len1);
    memmove(&name[sizeof(head) - 1 + len2 + 1], num, len1);
    return new Leaf(name, size);
}

// If you edit Make(), you should also edit MakeStatement().

Ptree* Ptree::Make(const char* pat, ...)
{
    va_list args;
    const int N = 4096;
    static char buf[N];
    char c;
    int len;
    char* ptr;
    Ptree* p;
    Ptree* q;
    int i = 0, j = 0;
    Ptree* result = nil;

    va_start(args, pat);
    while((c = pat[i++]) != '\0')
	if(c == '%'){
	    c = pat[i++];
	    if(c == '%')
		buf[j++] = c;
	    else if(c == 'd'){
		ptr = IntegerToString(va_arg(args, int), len);
		memmove(&buf[j], ptr, len);
		j += len;
	    }
	    else if(c == 's'){
		ptr = va_arg(args, char*);
		len = strlen(ptr);
		memmove(&buf[j], ptr, len);
		j += len;
	    }
	    else if(c == 'c')
		buf[j++] = va_arg(args, int);
	    else if(c == 'p'){
		p = va_arg(args, Ptree*);
		if(p == nil)
		    /* ignore */;
		else if(p->IsLeaf()){
		    memmove(&buf[j], p->GetPosition(), p->GetLength());
		    j += p->GetLength();
		}
		else{   
		    if(j > 0)
			q = List(new DupLeaf(buf, j), p);
		    else
			q = List(p);

		    j = 0;
		    result = Nconc(result, q);
		}
	    }
	    else
		MopErrorMessage("Ptree::Make()", "invalid format");
	}
	else
	    buf[j++] = c;

    va_end(args);

    if(j > 0)
	if(result == nil)
	    result = new DupLeaf(buf, j);
	else
	    result = Snoc(result, new DupLeaf(buf, j));

    return result;
}

/*
  MakeStatement() is identical to Make() except that the generated
  code is wrapped by a PtreeNonExpr object.
  This helps code-traverse functions such as WalkExpr() distinguish
  statements from expressions.

  Note: this version is perfectly identical to Make(). 97/3/26
*/
Ptree* Ptree::MakeStatement(const char* pat, ...)
{
    va_list args;
    const int N = 4096;
    static char buf[N];
    char c;
    int len;
    char* ptr;
    Ptree* p;
    Ptree* q;
    int i = 0, j = 0;
    Ptree* result = nil;

    va_start(args, pat);

    Class::WarnObsoleteness("Ptree::MakeStatement()", "Ptree::Make()");

    while((c = pat[i++]) != '\0')
	if(c == '%'){
	    c = pat[i++];
	    if(c == '%')
		buf[j++] = c;
	    else if(c == 'd'){
		ptr = IntegerToString(va_arg(args, int), len);
		memmove(&buf[j], ptr, len);
		j += len;
	    }
	    else if(c == 's'){
		ptr = va_arg(args, char*);
		len = strlen(ptr);
		memmove(&buf[j], ptr, len);
		j += len;
	    }
	    else if(c == 'c')
		buf[j++] = va_arg(args, int);
	    else if(c == 'p'){
		p = va_arg(args, Ptree*);
		if(p == nil)
		    /* ignore */;
		else if(p->IsLeaf()){
		    memmove(&buf[j], p->GetPosition(), p->GetLength());
		    j += p->GetLength();
		}
		else{   
		    if(j > 0)
			q = new DupLeaf(buf, j);
		    else
			q = nil;

		    j = 0;
		    result = Nconc(result, List(q, p));
		}
	    }
	    else
		MopErrorMessage("Ptree::MakeStatement()", "invalid format");
	}
	else
	    buf[j++] = c;

    va_end(args);

    if(j > 0)
	if(result == nil)
	    result = new DupLeaf(buf, j);
	else
	    result = Snoc(result, new DupLeaf(buf, j));

    return result;
}

bool Ptree::Reify(unsigned int& value)
{
    return Lex::Reify(this, value);
}

bool Ptree::Reify(char*& str)
{
    return Lex::Reify(this, str);
}

char* Ptree::IntegerToString(sint num, int& length)
{
    const int N = 16;
    static char buf[N];
    bool minus;

    int i = N - 1;
    if(num >= 0)
	minus = FALSE;
    else{
	minus = TRUE;
	num = -num;
    }

    buf[i--] = '\0';
    if(num == 0){
	buf[i] = '0';
	length = 1;
	return &buf[i];
    }
    else{
	while(num > 0){
	    buf[i--] = '0' + char(num % 10);
	    num /= 10;
	}

	if(minus)
	    buf[i--] = '-';

	length = N - 2 - i;
	return &buf[i + 1];
    }
}

Ptree* Ptree::qMake(char*)
{
    MopErrorMessage("Ptree::qMake()",
		    "the metaclass must be compiled by OpenC++.");
    return nil;
}

Ptree* Ptree::qMakeStatement(char*)
{
    MopErrorMessage("Ptree::qMakeStatement()",
		    "the metaclass must be compiled by OpenC++.");
    return nil;
}

// class PtreeHead	--- this is used to implement Ptree::qMake().

PtreeHead& PtreeHead::operator + (Ptree* p)
{
    ptree = Append(ptree, p);
    return *this;
}

PtreeHead& PtreeHead::operator + (const char* str)
{
    if(*str != '\0')
	ptree =  Append(ptree, (char*)str, strlen(str));

    return *this;
}

PtreeHead& PtreeHead::operator + (char* str)
{
    if(*str != '\0')
	ptree =  Append(ptree, str, strlen(str));

    return *this;
}

PtreeHead& PtreeHead::operator + (char c)
{
    ptree =  Append(ptree, &c, 1);
    return *this;
}

PtreeHead& PtreeHead::operator + (int n)
{
    int len;
    char* str = Ptree::IntegerToString(n, len);
    ptree =  Append(ptree, str, len);
    return *this;
}

Ptree* PtreeHead::Append(Ptree* lst, Ptree* tail)
{
    Ptree* last;
    Ptree* p;
    Ptree* q;

    if(tail == nil)
	return lst;

    if(!tail->IsLeaf() && tail->Length() == 1){
	tail = tail->Car();
	if(tail == nil)
	    return lst;
    }

    if(tail->IsLeaf() && lst != nil){
	last = Ptree::Last(lst);
	if(last != nil){
	    p = last->Car();
	    if(p != nil && p->IsLeaf()){
		q = new DupLeaf(p->GetPosition(), p->GetLength(),
				 tail->GetPosition(), tail->GetLength());
		last->SetCar(q);
		return lst;
	    }
	}
    }

    return Ptree::Snoc(lst, tail);
}

Ptree* PtreeHead::Append(Ptree* lst, char* str, int len)
{
    Ptree* last;
    Ptree* p;
    Ptree* q;

    if(lst != nil){
	last = Ptree::Last(lst);
	if(last != nil){
	    p = last->Car();
	    if(p != nil && p->IsLeaf()){
		q = new DupLeaf(p->GetPosition(), p->GetLength(),
				 str, len);
		last->SetCar(q);
		return lst;
	    }
	}
    }

    return Ptree::Snoc(lst, new DupLeaf(str, len));
}
