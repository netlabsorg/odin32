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

#ifndef _ptree_core_h
#define _ptree_core_h

#include "types.h"

#define NIL	((Ptree*)0)

class ostream;
class ProgramString;
class Walker;
class TypeInfo;

#ifdef __opencxx
metaclass QuoteClass Ptree;           // get qMake() available
#endif

class OCXXMOP Ptree : public LightObject {
public:
    virtual bool IsLeaf() = nil;
    bool Eq(char);
    bool Eq(char*);
    bool Eq(const char*, int);
    bool Eq(Ptree* p) { return Eq(this, p); }

    void Display();
    void Display2(ostream&);
    virtual void Print(ostream&, int, int) = nil;
    int Write(ostream&);
    virtual int Write(ostream&, int) = nil;
    void PrintIndent(ostream&, int);

    char* ToString();
    virtual void WritePS(ProgramString&) = nil;

    char* GetPosition() { return data.leaf.position; }
    int GetLength() { return data.leaf.length; }

    Ptree* Car() { return data.nonleaf.child; }
    Ptree* Cdr() { return data.nonleaf.next; }
    Ptree* Cadr() { return Cdr()->Car(); }
    Ptree* Cddr() { return Cdr()->Cdr(); }
    Ptree* Ca_ar();
    void SetCar(Ptree* p) { data.nonleaf.child = p; }
    void SetCdr(Ptree* p) { data.nonleaf.next = p; }

    char* LeftMost();
    char* RightMost();

    virtual int What();
    bool IsA(int);
    bool IsA(int, int);
    bool IsA(int, int, int);

    virtual Ptree* Translate(Walker*);
    virtual void Typeof(Walker*, TypeInfo&);

    virtual char* GetEncodedType();
    virtual char* GetEncodedName();

    Ptree* Last() { return Last(this); }
    Ptree* First() { return First(this); }
    Ptree* Rest() { return Rest(this); }
    Ptree* Second() { return Second(this); }
    Ptree* Third() { return Third(this); }
    Ptree* Nth(int n) { return Nth(this, n); }
    int Length() { return Length(this); }
    Ptree* ListTail(int n) { return ListTail(this, n); }

// static members

    static bool Eq(Ptree*, char);
    static bool Eq(Ptree*, char*);
    static bool Eq(Ptree*, char*, int);
    static bool Eq(Ptree*, Ptree*);
    static bool Equiv(Ptree*, Ptree*);
    static bool Equal(Ptree*, Ptree*);
    static Ptree* Last(Ptree*);
    static Ptree* First(Ptree*);
    static Ptree* Rest(Ptree*);
    static Ptree* Second(Ptree*);
    static Ptree* Third(Ptree*);
    static Ptree* Nth(Ptree*, int);
    static int Length(Ptree*);
    static Ptree* ListTail(Ptree*, int);

    static Ptree* Cons(Ptree*, Ptree*);
    static Ptree* List();
    static Ptree* List(Ptree*);
    static Ptree* List(Ptree*, Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*, Ptree*,
		       Ptree*);
    static Ptree* List(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*, Ptree*,
		       Ptree*, Ptree*);
    static Ptree* CopyList(Ptree*);
    static Ptree* Append(Ptree*, Ptree*);
    static Ptree* ReplaceAll(Ptree*, Ptree*, Ptree*);
    static Ptree* Subst(Ptree*, Ptree*, Ptree*);
    static Ptree* Subst(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* Subst(Ptree*, Ptree*, Ptree*, Ptree*,
			Ptree*, Ptree*, Ptree*);
    static Ptree* ShallowSubst(Ptree*, Ptree*, Ptree*);
    static Ptree* ShallowSubst(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* ShallowSubst(Ptree*, Ptree*, Ptree*, Ptree*,
			       Ptree*, Ptree*, Ptree*);
    static Ptree* ShallowSubst(Ptree*, Ptree*, Ptree*, Ptree*,
			       Ptree*, Ptree*, Ptree*, Ptree*, Ptree*);
    static Ptree* SubstSublist(Ptree*, Ptree*, Ptree*);

    /* they cause side-effect */
    static Ptree* Snoc(Ptree*, Ptree*);
    static Ptree* Nconc(Ptree*, Ptree*);
    static Ptree* Nconc(Ptree*, Ptree*, Ptree*);

// in pattern.cc
public:
    static bool Match(Ptree*, char*, ...);
    static Ptree* Make(const char* pat, ...);
    static Ptree* MakeStatement(const char* pat, ...);
    static Ptree* GenSym();

    static Ptree* qMake(char*);
    static Ptree* qMakeStatement(char*);

    bool Reify(unsigned int&);
    bool Reify(char*&);

    static char* IntegerToString(sint, int&);	// not documented

private:
    static char* MatchPat(Ptree*, char*);
    static char* MatchList(Ptree*, char*);
    static char* MatchWord(Ptree*, char*);

public:
    // if this is TRUE, Print() shows an encoded type and name.
    static bool show_encoded;

protected:
    union {
	struct {
	    Ptree* child;
	    Ptree* next;
	} nonleaf;
	struct {
	    char* position;
	    int  length;
	} leaf;
    }data;

    friend class NonLeaf;
};

inline ostream& operator << (ostream& s, Ptree* p)
{
    p->Write(s);
    return s;
}

// in ptree.cc

class OCXXMOP PtreeIter : public LightObject {
public:
    PtreeIter(Ptree* p) { ptree = p; }
    Ptree* operator () () { return Pop(); }
    Ptree* Pop();
    bool Next(Ptree*&);
    void Reset(Ptree* p) { ptree = p; }

    Ptree* operator * () { return This(); }
    Ptree* operator ++ () { Pop(); return This(); }
    Ptree* operator ++ (int) { return Pop(); }	// postfix
    Ptree* This();
    bool Empty() { return bool(ptree == nil); }

private:
    Ptree* ptree;
};

class OCXXMOP PtreeArray : public LightObject {
public:
    PtreeArray(int = 8);
    uint Number() { return num; }
    Ptree*& operator [] (uint index) { return Ref(index); }
    Ptree*& Ref(uint index);
    void Append(Ptree*);
    void Clear() { num = 0; }
    Ptree* All();

private:
    uint num, size;
    Ptree** array;
    Ptree* default_buf[8];
};

// PtreeHead is used to implement Ptree::qMake()
// The implementation is in pattern.cc

class OCXXMOP PtreeHead : public LightObject {
public:
    PtreeHead() { ptree = nil; }
    operator Ptree* () { return ptree; }
    PtreeHead& operator + (Ptree*);
    PtreeHead& operator + (const char*);
    PtreeHead& operator + (char*);
    PtreeHead& operator + (char);
    PtreeHead& operator + (int);

private:
    Ptree* Append(Ptree*, Ptree*);
    Ptree* Append(Ptree*, char*, int);

private:
    Ptree* ptree;
};

// error messages

void OCXXMOP MopErrorMessage(char* method_name, char* msg);
void OCXXMOP MopErrorMessage2(char* msg1, char* msg2);
void OCXXMOP MopWarningMessage(char* method_name, char* msg);
void OCXXMOP MopWarningMessage2(char* msg1, char* msg2);
void OCXXMOP MopMoreWarningMessage(char* msg1, char* msg2 = nil);

#endif  /* _ptree_core_h */
