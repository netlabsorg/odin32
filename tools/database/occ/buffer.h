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

#ifndef _buffer_h
#define _buffer_h

#include <fstream.h>
#include "types.h"

class Ptree;

class Program : public Object {
public:
    Program(char *name) {
	replacement = nil;
	defaultname = name;
    }

    virtual ~Program() {}

    void Rewind() { index = 0; }
    void Rewind(uint i) { index = i; }
    uint GetSize() { return size; }
    void Unget() { --index; }

    char Ref(uint position) { return buf[position]; }
    void Set(char c, uint position) { buf[position] = c; }

    /* The location of the last character obtained by Get(). */
    uint GetCurPos() { return index - 1; }

    /* The location of the next character obtained by Get(). */
    uint GetNextPos() { return index; }

    /* The result of Read() must be the same for each call. */
    const char* Read(uint p) { return &buf[p]; }

    virtual char Get();

    void Subst(Ptree* newtext, Ptree* oldtext);
    void Insert(Ptree* pos, Ptree* before_text, Ptree* after_text);
    void Replace(char*, char*, Ptree*);
    void MinimumSubst(Ptree* newtext, Ptree* oldtext);

    uint LineNumber(char*, char*&, int&);

    void Write(ostream&, const char*);
    sint ReadLineDirective(uint, sint, uint&, int&);

private:
    bool MinimumSubst2(Ptree* newtext, Ptree* oldtext);

protected:
    char*	buf;
    uint	size, index;
    char	*defaultname;

private:
    class Replacement : public LightObject {
    public:
	Replacement(Replacement*, uint, uint, Ptree*);
	Replacement* next;
	uint startpos;
	uint endpos;
	Ptree* text;
    };

    Replacement* replacement;
};

class ProgramFile : public Program {
public:
    ProgramFile(ifstream&, char *filename = "unknown");
    ~ProgramFile();
};

class ProgramFromStdin : public Program {
public:
    ProgramFromStdin();
    ~ProgramFromStdin();
    char Get();

protected:
    uint buf_size;
};

// This class violates the sepcification for Read().

class ProgramString : public Program {
public:
    ProgramString();
    ~ProgramString();
    void Clear() { buf[0] = '\0'; }
    uint Length() { return str_length; }
    ProgramString& operator << (const char*);
    ProgramString& operator << (const char);

private:
    uint str_length;
};

#endif /* _buffer_h */
