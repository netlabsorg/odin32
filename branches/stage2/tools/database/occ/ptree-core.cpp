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
  Copyright (C) 1997-2001 Shigeru Chiba, University of Tsukuba.

  Permission to use, copy, distribute and modify this software and   
  its documentation for any purpose is hereby granted without fee,        
  provided that the above copyright notice appear in all copies and that 
  both that copyright notice and this permission notice appear in 
  supporting documentation.

  Shigeru Chiba makes no representations about the suitability of this 
  software for any purpose.  It is provided "as is" without express or
  implied warranty.
*/

#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include "ptree.h"
#include "token.h"
#include "walker.h"
#include "typeinfo.h"
#include "buffer.h"

#if defined(_MSC_VER) || defined(IRIX_CC)
#include <stdlib.h>		// for exit()
#endif

bool Ptree::show_encoded = FALSE;

// error messages

void MopErrorMessage(char* where, char* msg)
{
    cerr << "MOP error: in " << where << ", " << msg << '\n';
    exit(1);
}

void MopErrorMessage2(char* msg1, char* msg2)
{
    cerr << "MOP error: " << msg1 << msg2 << '\n';
    exit(1);
}

void MopWarningMessage(char* where, char* msg)
{
    cerr << "MOP warning: in " << where << ", " << msg << '\n';
}

void MopWarningMessage2(char* msg1, char* msg2)
{
    cerr << "MOP warning: " << msg1 << msg2 << '\n';
}

void MopMoreWarningMessage(char* msg1, char* msg2)
{
    cerr << "             " << msg1;
    if(msg2 != nil)
	cerr << msg2;

    cerr << '\n';
}

// class Ptree

void Ptree::Display()
{
    Display2(cerr);
}

void Ptree::Display2(ostream& s)
{
    if(this == nil)
	s << "nil\n";
    else{
	Print(s, 0, 0);
	s << '\n';
    }
}

int Ptree::Write(ostream& s)
{
    if(this == nil)
	return 0;
    else
	return Write(s, 0);
}

void Ptree::PrintIndent(ostream& out, int indent)
{
    out << '\n';
    for(int i = 0; i < indent; ++i)
	out << "    ";
}

char* Ptree::ToString()
{
    if(this == nil)
	return nil;
    else{
	ProgramString ps;
	WritePS(ps);
	return (char*)ps.Read(0);
    }
}

bool Ptree::Eq(char c)
{
    if(this == nil)
	return FALSE;
    else
	return(IsLeaf() && GetLength() == 1 && *GetPosition() == c);
}

bool Ptree::Eq(char* str)
{
    if(this == nil)
	return FALSE;
    else if(IsLeaf()){
	char* p = GetPosition();
	int n = GetLength();
	int i;
	for(i = 0; i < n; ++i)
	    if(p[i] != str[i] || str[i] == '\0')
		return FALSE;

	return bool(str[i] == '\0');
    }
    else
	return FALSE;
}

bool Ptree::Eq(const char* str, int len)
{
    if(this != nil && IsLeaf()){
	char* p = GetPosition();
	int n = GetLength();
	if(n == len){
	    int i;
	    for(i = 0; i < n; ++i)
		if(p[i] != str[i])
		    return FALSE;

	    return TRUE;
	}
    }

    return FALSE;
}

Ptree* Ptree::Ca_ar()		// compute Caa..ar
{
    Ptree* p = this;
    while(p != nil && !p->IsLeaf())
	p = p->Car();

    return p;
}

char* Ptree::LeftMost()
{
    if(this == nil)
	return nil;
    else if(IsLeaf())
	return GetPosition();
    else{
	Ptree* p = this;
	while(p != nil){
	    char* i = p->Car()->LeftMost();
	    if(i != nil)
		return i;
	    else
		p = p->Cdr();
	}
	return nil;
    }
}

char* Ptree::RightMost()
{
    if(this == nil)
	return nil;
    else if(IsLeaf())
	return GetPosition() + GetLength();
    else{
	int n = Length();
	while(n > 0){
	    char* i = Nth(--n)->RightMost();
	    if(i != nil)
		return i;
	}

	return nil;
    }
}

int Ptree::What()
{
    return BadToken;
}

bool Ptree::IsA(int kind)
{
    if(this == nil)
	return FALSE;
    else
	return bool(What() == kind);
}

bool Ptree::IsA(int kind1, int kind2)
{
    if(this == nil)
	return FALSE;
    else{
	int k = What();
	return bool(k == kind1 || k == kind2);
    }
}

bool Ptree::IsA(int kind1, int kind2, int kind3)
{
    if(this == nil)
	return FALSE;
    else{
	int k = What();
	return bool(k == kind1 || k == kind2 || k == kind3);
    }
}

Ptree* Ptree::Translate(Walker* w)
{
    return w->TranslatePtree(this);
}

void Ptree::Typeof(Walker* w, TypeInfo& t)
{
    w->TypeofPtree(this, t);
}

char* Ptree::GetEncodedType()
{
    return nil;
}

char* Ptree::GetEncodedName()
{
    return nil;
}


// static members

bool Ptree::Eq(Ptree* p, char c)
{
    return p->Eq(c);
}

bool Ptree::Eq(Ptree* p, char* str)
{
    return p->Eq(str);
}

bool Ptree::Eq(Ptree* p, char* str, int len)
{
    return p->Eq(str, len);
}

bool Ptree::Eq(Ptree* p, Ptree* q)
{
    if(p == q)
	return TRUE;
    else if(p == nil || q == nil)
	return FALSE;
    else if(p->IsLeaf() && q->IsLeaf()){
	int plen = p->GetLength();
	int qlen = q->GetLength();
	if(plen == qlen){
	    char* pstr = p->GetPosition();
	    char* qstr = q->GetPosition();
	    while(--plen >= 0)
		if(pstr[plen] != qstr[plen])
		    return FALSE;

	    return TRUE;
	}
    }

    return FALSE;
}

/*
  Equiv() returns true even if p and q are lists and all the elements
  are equal respectively.
*/
bool Ptree::Equiv(Ptree* p, Ptree* q)
{
    if(p == q)
	return TRUE;
    else if(p == nil || q == nil)
	return FALSE;
    else if(p->IsLeaf() || q->IsLeaf())
	return Eq(p, q);
    else{
	while(p != nil && q != nil)
	    if(p->Car() != q->Car())
		return FALSE;
	    else{
		p = p->Cdr();
		q = q->Cdr();
	    }

	return p == nil && q == nil;
    }
}

bool Ptree::Equal(Ptree* p, Ptree* q)
{
    if(p == q)
	return TRUE;
    else if(p == nil || q == nil)
	return FALSE;
    else if(p->IsLeaf() || q->IsLeaf())
	return Eq(p, q);
    else
	return Equal(p->Car(), q->Car()) && Equal(p->Cdr(), q->Cdr());
}

Ptree* Ptree::Last(Ptree* p)	// return the last cons cell.
{
    Ptree* next;
    if(p == nil)
	return nil;

    while((next = p->Cdr()) != nil)
	p = next;

    return p;
}

Ptree* Ptree::First(Ptree* p)
{
    if(p != nil)
	return p->Car();
    else
	return p;
}

Ptree* Ptree::Rest(Ptree* p)
{
    if(p != nil)
	return p->Cdr();
    else
	return p;
}

Ptree* Ptree::Second(Ptree* p)
{
    if(p != nil){
	p = p->Cdr();
	if(p != nil)
	    return p->Car();
    }

    return p;
}

Ptree* Ptree::Third(Ptree* p)
{
    if(p != nil){
	p = p->Cdr();
	if(p != nil){
	    p = p->Cdr();
	    if(p != nil)
		return p->Car();
	}
    }

    return p;
}

/*
  Nth(lst, 0) is equivalent to First(lst).
*/
Ptree* Ptree::Nth(Ptree* p, int n)
{
    while(p != nil && n-- > 0)
	p = p->Cdr();

    if(p != nil)
	return p->Car();
    else
	return p;
}

/*
  Length() returns a negative number if p is not a list.
*/
int Ptree::Length(Ptree* p)
{
    int i = 0;

    if(p != nil && p->IsLeaf())
	return -2;	/* p is not a pair */

    while(p != nil){
	++i;
	if(p->IsLeaf())
	    return -1;	/* p is a pair, but not a list. */
	else
	    p = p->Cdr();
    }

    return i;
}

Ptree* Ptree::ListTail(Ptree* p, int k)
{
    while(p != nil && k-- > 0)
	p = p->Cdr();

    return p;
}

Ptree* Ptree::Cons(Ptree* p, Ptree* q)
{
    return new NonLeaf(p, q);
}

Ptree* Ptree::List(Ptree* p)
{
    return new NonLeaf(p, nil);
}

Ptree* Ptree::List()
{
    return nil;
}

Ptree* Ptree::List(Ptree* p, Ptree* q)
{
    return new NonLeaf(p, new NonLeaf(q, nil));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3)
{
    return new NonLeaf(p1, new NonLeaf(p2, new NonLeaf(p3, nil)));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3, Ptree* p4)
{
    return new NonLeaf(p1, List(p2, p3, p4));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3, Ptree* p4, Ptree* p5)
{
    return Nconc(List(p1, p2), List(p3, p4, p5));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3, Ptree* p4, Ptree* p5,
		   Ptree* p6)
{
    return Nconc(List(p1, p2, p3), List(p4, p5, p6));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3, Ptree* p4, Ptree* p5,
		   Ptree* p6, Ptree* p7)
{
    return Nconc(List(p1, p2, p3), List(p4, p5, p6, p7));
}

Ptree* Ptree::List(Ptree* p1, Ptree* p2, Ptree* p3, Ptree* p4, Ptree* p5,
		   Ptree* p6, Ptree* p7, Ptree* p8)
{
    return Nconc(List(p1, p2, p3, p4), List(p5, p6, p7, p8));
}

Ptree* Ptree::CopyList(Ptree* p)
{
    return Append(p, nil);
}

//   q may be a leaf
//
Ptree* Ptree::Append(Ptree* p, Ptree* q)
{
    Ptree *result, *tail;

    if(p == nil)
	if(q->IsLeaf())
	    return Cons(q, nil);
	else
	    return q;

    result = tail = Cons(p->Car(), nil);
    p = p->Cdr();
    while(p != nil){
	Ptree* cell = Cons(p->Car(), nil);
	tail->SetCdr(cell);
	tail = cell;
	p = p->Cdr();
    }

    if(q != nil && q->IsLeaf())
	tail->SetCdr(Cons(q, nil));
    else
	tail->SetCdr(q);

    return result;
}

/*
  ReplaceAll() substitutes SUBST for all occurences of ORIG in LIST.
  It recursively searches LIST for ORIG.
*/
Ptree* Ptree::ReplaceAll(Ptree* list, Ptree* orig, Ptree* subst)
{
    if(Eq(list, orig))
	return subst;
    else if(list == nil || list->IsLeaf())
	return list;
    else{
	PtreeArray newlist;
	bool changed = FALSE;
	Ptree* rest = list;
	while(rest != nil){
	    Ptree* p = rest->Car();
	    Ptree* q = ReplaceAll(p, orig, subst);
	    newlist.Append(q);
	    if(p != q)
		changed = TRUE;

	    rest = rest->Cdr();
	}

	if(changed)
	    return newlist.All();
	else
	    return list;
    }
}

Ptree* Ptree::Subst(Ptree* newone, Ptree* old, Ptree* tree)
{
    if(old == tree)
	return newone;
    else if(tree== nil || tree->IsLeaf())
	return tree;
    else{
	Ptree* head = tree->Car();
	Ptree* head2 = Subst(newone, old, head);
	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail : Subst(newone, old, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::Subst(Ptree* newone1, Ptree* old1, Ptree* newone2, Ptree* old2,
		    Ptree* tree)
{
    if(old1 == tree)
	return newone1;
    else if(old2 == tree)
	return newone2;
    else if(tree == nil || tree->IsLeaf())
	return tree;
    else{
	Ptree* head = tree->Car();
	Ptree* head2 = Subst(newone1, old1, newone2, old2, head);
	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail
			: Subst(newone1, old1, newone2, old2, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::Subst(Ptree* newone1, Ptree* old1, Ptree* newone2, Ptree* old2,
		    Ptree* newone3, Ptree* old3, Ptree* tree)
{
    if(old1 == tree)
	return newone1;
    else if(old2 == tree)
	return newone2;
    else if(old3 == tree)
	return newone3;
    else if(tree == nil || tree->IsLeaf())
	return tree;
    else{
	Ptree* head = tree->Car();
	Ptree* head2 = Subst(newone1, old1, newone2, old2,
			     newone3, old3, head);
	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail :
			Subst(newone1, old1, newone2, old2,
			      newone3, old3, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

// ShallowSubst() doesn't recursively apply substitution to a subtree.

Ptree* Ptree::ShallowSubst(Ptree* newone, Ptree* old, Ptree* tree)
{
    if(old == tree)
	return newone;
    else if(tree== nil || tree->IsLeaf())
	return tree;
    else{
	Ptree *head, *head2;
	head = tree->Car();
	if(old == head)
	    head2 = newone;
	else
	    head2 = head;

	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail : ShallowSubst(newone, old, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::ShallowSubst(Ptree* newone1, Ptree* old1,
			   Ptree* newone2, Ptree* old2, Ptree* tree)
{
    if(old1 == tree)
	return newone1;
    else if(old2 == tree)
	return newone2;
    else if(tree == nil || tree->IsLeaf())
	return tree;
    else{
	Ptree *head, *head2;
	head = tree->Car();
	if(old1 == head)
	    head2 = newone1;
	else if(old2 == head)
	    head2 = newone2;
	else
	    head2 = head;

	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail :
			ShallowSubst(newone1, old1, newone2, old2, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::ShallowSubst(Ptree* newone1, Ptree* old1,
			   Ptree* newone2, Ptree* old2,
			   Ptree* newone3, Ptree* old3, Ptree* tree)
{
    if(old1 == tree)
	return newone1;
    else if(old2 == tree)
	return newone2;
    else if(old3 == tree)
	return newone3;
    else if(tree == nil || tree->IsLeaf())
	return tree;
    else{
	Ptree *head, *head2;
	head = tree->Car();
	if(old1 == head)
	    head2 = newone1;
	else if(old2 == head)
	    head2 = newone2;
	else if(old3 == head)
	    head2 = newone3;
	else
	    head2 = head;

	Ptree* tail = tree->Cdr();
	Ptree* tail2 = (tail == nil) ? tail :
			ShallowSubst(newone1, old1, newone2, old2,
				     newone3, old3, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::ShallowSubst(Ptree* newone1, Ptree* old1,
			   Ptree* newone2, Ptree* old2,
			   Ptree* newone3, Ptree* old3,
			   Ptree* newone4, Ptree* old4, Ptree* tree)
{
    if(old1 == tree)
	return newone1;
    else if(old2 == tree)
	return newone2;
    else if(old3 == tree)
	return newone3;
    else if(old4 == tree)
	return newone4;
    else if(tree == nil || tree->IsLeaf())
	return tree;
    else{
	Ptree *head, *head2;
	head = tree->Car();
	if(old1 == head)
	    head2 = newone1;
	else if(old2 == head)
	    head2 = newone2;
	else if(old3 == head)
	    head2 = newone3;
	else if(old4 == head)
	    head2 = newone4;
	else
	    head2 = head;

	Ptree* tail = tree->Cdr();
	Ptree* tail2 =  (tail == nil) ? tail :
			ShallowSubst(newone1, old1, newone2, old2,
				     newone3, old3, newone4, old4, tail);
	if(head == head2 && tail == tail2)
	    return tree;
	else
	    return Cons(head2, tail2);
    }
}

Ptree* Ptree::SubstSublist(Ptree* newsub, Ptree* oldsub, Ptree* lst)
{
    if(lst == oldsub)
	return newsub;
    else
	return Cons(lst->Car(), SubstSublist(newsub, oldsub, lst->Cdr()));
}

Ptree* Ptree::Snoc(Ptree* p, Ptree* q)
{
    return Nconc(p, Cons(q, nil));
}

/* Nconc is desctructive append */

Ptree* Ptree::Nconc(Ptree* p, Ptree* q)
{
    if(p == nil)
	return q;
    else{
	Last(p)->data.nonleaf.next = q;	// Last(p)->SetCdr(q);
	return p;
    }
}

Ptree* Ptree::Nconc(Ptree* p, Ptree* q, Ptree* r)
{
    return Nconc(p, Nconc(q, r));
}


// class PtreeIter

Ptree* PtreeIter::Pop()
{
    if(ptree == nil)
	return nil;
    else{
	Ptree* p = ptree->Car();
	ptree = ptree->Cdr();
	return p;
    }
}

bool PtreeIter::Next(Ptree*& car)
{
    if(ptree == nil)
	return FALSE;
    else{
	car = ptree->Car();
	ptree = ptree->Cdr();
	return TRUE;
    }
}

Ptree* PtreeIter::This()
{
    if(ptree == nil)
	return nil;
    else
	return ptree->Car();
}

// class PtreeArray

PtreeArray::PtreeArray(int s)
{
    num = 0;
    if(s > 8){
	size = s;
	array = new (GC) Ptree*[s];
    }
    else{
	size = 8;
	array = default_buf;
    }
}

void PtreeArray::Append(Ptree* p)
{
    if(num >= size){
	size = size * 2;
	Ptree** a = new (GC) Ptree*[size];
	memmove(a, array, size_t(num * sizeof(Ptree*)));
	array = a;
    }

    array[num++] = p;
}

Ptree*& PtreeArray::Ref(uint i)
{
    if(i < num)
	return array[i];
    else{
	MopErrorMessage("PtreeArray", "out of range");
	return array[0];
    }
}

Ptree* PtreeArray::All()
{
    Ptree* lst = nil;

    for(sint i = Number() - 1; i >= 0; --i)
	lst = Ptree::Cons(Ref(i), lst);

    return lst;
}
