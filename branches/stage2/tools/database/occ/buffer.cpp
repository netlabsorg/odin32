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

#include <string.h>
#include <stdlib.h>
#include "buffer.h"
#include "token.h"
#include "ptree-core.h"
#include "mop.h"

#if defined(_PARSE_VCC)
#define _MSC_VER	1100
#endif

char Program::Get()
{
    if(buf[index] == '\0')
	return buf[index];
    else
	return buf[index++];
}

void Program::Subst(Ptree* newtext, Ptree* oldtext)
{
    Replace(oldtext->LeftMost(), oldtext->RightMost(), newtext);
}

void Program::Insert(Ptree* pos, Ptree* before_text, Ptree* after_text)
{
    char* p;

    if(before_text != nil){
	p = pos->LeftMost();
	Replace(p, p, before_text);
    }

    if(after_text != nil){
	p = pos->RightMost();
	Replace(p, p, after_text);
    }
}

void Program::MinimumSubst(Ptree* newtext, Ptree* oldtext)
{
    if(MinimumSubst2(newtext, oldtext))
	Subst(newtext, oldtext);
}

bool Program::MinimumSubst2(Ptree* newtext, Ptree* oldtext)
{
    int what;
    if(oldtext == newtext)
	return FALSE;
    else if(oldtext == nil || newtext == nil)
	return TRUE;
    else if(what = newtext->What(),
	    (what == ntExprStatement || what == ntTypedef))
	return TRUE;
    else if(oldtext->IsLeaf() || newtext->IsLeaf())
	return TRUE;
    else if(oldtext->Car() == nil && oldtext->Cdr() == nil)
	return TRUE;
    else if(oldtext == newtext->Cdr()){
	Insert(oldtext, newtext->Car(), nil);
	return FALSE;
    }
    else if(oldtext->Car() != nil && oldtext->Car() == newtext->Second()){
	Insert(oldtext->Car(), newtext->Car(), nil);
	newtext = newtext->ListTail(2);
	if(MinimumSubst2(newtext, oldtext->Cdr()))
	    if(oldtext->Cdr() == nil)
		Insert(oldtext->Car(), nil, newtext);
	    else
		Subst(newtext, oldtext->Cdr());

	return FALSE;
    }
    else{
	bool dirty1 = MinimumSubst2(newtext->Car(), oldtext->Car());
	bool dirty2 = MinimumSubst2(newtext->Cdr(), oldtext->Cdr());
	if(dirty1 == dirty2)
	    return dirty1;
	else if(dirty1)
	    if(oldtext->Cdr() == nil && newtext->Cdr() == nil)
		return TRUE;
	    else if(oldtext->Car() == nil)
		Insert(oldtext->Cdr(), newtext->Car(), nil);
	    else
		Subst(newtext->Car(), oldtext->Car());
	else
	    if(oldtext->Car() == nil && newtext->Car() == nil)
		return TRUE;
	    else if(oldtext->Cdr() == nil)
		Insert(oldtext->Car(), nil, newtext->Cdr());
	    else
		Subst(newtext->Cdr(), oldtext->Cdr());

	return FALSE;
    }
}

void Program::Replace(char* startpos, char* endpos, Ptree* text)
{
    if(startpos == nil || endpos == nil)
	return;

    uint start = uint(startpos - buf);
    uint end = uint(endpos - buf);
    Replacement* p = replacement;
    if(p == nil)
	replacement = new Replacement(nil, start, end, text);
    else if (start < p->startpos)
	replacement = new Replacement(p, start, end, text);
    else if(p->next == nil)
	p->next = new Replacement(nil, start, end, text);
    else{
	for(; p->next != nil; p = p->next)
	    if(start < p->next->startpos)
		break;

	p->next = new Replacement(p->next, start, end, text);
    }
}

/*
  LineNumber() returns the line number of the line pointed to by PTR.
*/
uint Program::LineNumber(char* ptr, char*& filename, int& filename_length)
{
    sint n;
    int  len;
    uint name;

    int nline = 0;
    uint pos = uint(ptr - buf);
    if(pos > size){
	// error?
	filename = defaultname;
	filename_length = strlen(defaultname);
	return 0;
    }

    sint line_number = -1;
    filename_length = 0;

    while(pos > 0){
	switch(Ref(--pos)){
	case '\n' :
	    ++nline;
	    break;
	case '#' :
	    len = 0;
	    n = ReadLineDirective(pos, -1, name, len);
	    if(n >= 0){			// unless #pragma
		if(line_number < 0)
		    line_number = n + nline;

		if(len > 0 && filename_length == 0){
		    filename = (char*)Read(name);
		    filename_length = len;
		}
	    }
	    break;
	}

	if(line_number >= 0 && filename_length > 0)
	    return line_number;
    }

    if(filename_length == 0){
	filename = defaultname;
	filename_length = strlen(defaultname);
    }

    if(line_number < 0)
	line_number = nline + 1;

    return line_number;
}

/*
  Write() saves the program as a file named FILE_NAME.
  This assumes that the first line of the program is
  a # line directive.
*/
void Program::Write(ostream& out, const char* file_name)
{
    Replacement* rep = replacement;
    uint pos;
    uint nlines = 1;
    uint line_number = 1;
    uint i = 0;
    char c;

    uint filename = 0;
    int filename_length = 0;

    if(Ref(i) == '#')
	line_number = ReadLineDirective(i, (sint)line_number,
					filename, filename_length);

    for(; rep != nil; rep = rep->next){
	pos = rep->startpos;
	while(i < pos){
	    c = Ref(i++);
	    if(c == '\0'){
		--i;
		break;
	    }

	    out << c;
	    if(c == '\n'){
		++nlines;
		++line_number;
		if(Ref(i) == '#')
		    line_number = ReadLineDirective(i, (sint)line_number,
						    filename, filename_length);
	    }
	}

	if(i > 0 && Ref(i - 1) != '\n'){
	    out << '\n';
	    ++nlines;
	}

#if defined(_MSC_VER) || defined(IRIX_CC)
	out << "#line " << nlines + 1 << " \"" << file_name << "\"\n";
#else
	out << "# " << nlines + 1 << " \"" << file_name << "\"\n";
#endif
	++nlines;
	nlines += rep->text->Write(out);
	pos = rep->endpos;
	if(rep->next != nil && rep->next->startpos <= pos){
	    rep = rep->next;
	    out << '\n';
	    ++nlines;
	    nlines += rep->text->Write(out);
	    if(rep->endpos > pos)
		pos = rep->endpos;
	}

	while(i < pos){
	    c = Ref(i++);
	    if(c == '\0'){
		--i;
		break;
	    }
	    else if(c == '\n'){
		++line_number;
		if(Ref(i) == '#')
		    line_number = ReadLineDirective(i, (sint)line_number,
						    filename, filename_length);
	    }
	}

#if defined(_MSC_VER) || defined(IRIX_CC)
	out << "\n#line " << line_number << ' ';
#else
	out << "\n# " << line_number << ' ';
#endif
	++nlines;
	if(filename_length > 0)
	    for(int j = 0; j < filename_length; ++j)
		out << (char)Ref(filename + j);
	else
	    out << '"' << defaultname << '"';

	out << '\n';
	++nlines;
    }

    while((c = Ref(i++)) != '\0'){
	out << c;
	if(c == '\n')
	    ++nlines;
    }

#if defined(_MSC_VER) || defined(IRIX_CC)
    out << "\n#line " << nlines + 2 << " \"" << file_name << "\"\n";
#else
    out << "\n# " << nlines + 2 << " \"" << file_name << "\"\n";
#endif
    Class::FinalizeAll(out);
    opcxx_ListOfMetaclass::FinalizeAll(out);
}

sint Program::ReadLineDirective(uint i, sint line_number,
				uint& filename, int& filename_length)
{
    char c;

    do{
	c = Ref(++i);
    }while(is_blank(c));

#if defined(_MSC_VER) || defined(IRIX_CC)
    if(i + 4 <= GetSize() && strncmp(Read(i), "line", 4) == 0){
	i += 3;
	do{
	    c = Ref(++i);
	}while(is_blank(c));
    }
#endif

    if(is_digit(c)){		/* # <line> <file> */
	uint num = c - '0';
	for(;;){
	    c = Ref(++i);
	    if(is_digit(c))
		num = num * 10 + c - '0';
	    else
		break;
	}

	line_number = num - 1;	/* line_number'll be incremented soon */

	if(is_blank(c)){
	    do{
		c = Ref(++i);
	    }while(is_blank(c));
	    if(c == '"'){
		uint fname_start = i;
		do{
		    c = Ref(++i);
		} while(c != '"');
		if(i > fname_start + 2){
		    filename = fname_start;
		    filename_length = int(i - fname_start + 1);
		}
	    }
	}
    }

    return line_number;
}

// class Program::Replacement

Program::Replacement::Replacement(Replacement* n, uint st,
				  uint ed, Ptree* t)
{
    next = n;
    startpos = st;
    endpos = ed;
    text = t;
}

// subclasses

ProgramFile::ProgramFile(ifstream& f, char *filename)
: Program(filename)
{
    f.seekg(0, ios::end);
    size = f.tellg();
    f.seekg(0);

    buf = new char[size + 1];
    f.read(buf, int(size));
#if defined(_WIN32)
    // Under win31/95/NT, the pair CR-LF is replaced by LF,
    // implying a smallest size
    size = f.gcount();
#endif
    buf[size] = '\0';
    index = 0;
}

ProgramFile::~ProgramFile()
{
    delete [] buf;
    buf = nil;
}

ProgramFromStdin::ProgramFromStdin()
: Program("stdin")
{
    const int Size = 512 * 1024;
    buf = new char[Size];
    buf_size = Size;
    index = size = 0;
}

ProgramFromStdin::~ProgramFromStdin()
{
    delete [] buf;
    buf = nil;
}

char ProgramFromStdin::Get()
{
    if(size >= buf_size){
	cerr << "ProgramFromStdin: sorry, out of memory\n";
	exit(1);
    }

    if(index >= size){
	int c = cin.get();
	if(c == EOF)
	    c = '\0';

	buf[size++] = c;
    }

    if(buf[index] == '\0')
	return buf[index];
    else
	return buf[index++];
}

// class ProgramString

const int Quantum = 4;
const int BufSize = 1 << Quantum;

ProgramString::ProgramString()
: Program("unknown")
{
    buf = new (GC) char[BufSize];
    buf[0] = '\0';
    size = BufSize;
    index = str_length = 0;
}

ProgramString::~ProgramString()
{
//  delete [] buf;
    buf = nil;
}

ProgramString& ProgramString::operator << (const char* str)
{
    int len = strlen(str) + 1;

    if(str_length + len < size){
	memmove(&buf[str_length], str, len);
	str_length += len - 1;
    }
    else{
	size = (str_length + len + BufSize) & ~(BufSize - 1);
	char* newbuf = new (GC) char[size];
	memmove(newbuf, buf, size_t(str_length));
	memmove(&newbuf[str_length], str, len);
//	delete [] buf;
	buf = newbuf;
	str_length += len - 1;
    }

    return *this;
}

ProgramString& ProgramString::operator << (const char c)
{
    if(str_length + 2 < size){
	buf[str_length] = c;
	buf[++str_length] = '\0';
    }
    else{
	size = (str_length + 2 + BufSize) & ~(BufSize - 1);
	char* newbuf = new (GC) char[size];
	memmove(newbuf, buf, size_t(str_length));
	newbuf[str_length] = c;
	newbuf[++str_length] = '\0';
//	delete [] buf;
	buf = newbuf;
    }

    return *this;
}
