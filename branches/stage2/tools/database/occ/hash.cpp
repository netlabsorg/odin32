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

#include <iostream.h>
#include <string.h>
#include "types.h"
#include "hash.h"
#include "ptree-core.h"
#ifndef DONT_GC
#include "gc/gc.h"
#endif

struct HashTableEntry {
    char*	key;		// nil: unused, -1: deleted
    HashValue	value;
};

// class HashTable

HashTable::HashTable()
{
    Size = 251;
    Prime2 = 127;
    MakeTable();
}

void HashTable::MakeTable()
{
    entries = new (GC) HashTableEntry[Size];
    for(int i = 0; i < Size; ++i)
	entries[i].key = nil;
}

BigHashTable::BigHashTable() : HashTable(0)
{
    #if defined(OS2)                    //kso: I wanna have a larger table
    Size = 6007;
    Prime2 = 3001;
    #else
    Size = 2053;	// 1021
    Prime2 = 1021;	// 509
    #endif
    MakeTable();
}

bool HashTable::IsEmpty()
{
    for(int i = 0; i < Size; ++i)
	if(entries[i].key != nil && entries[i].key != (char*)-1)
	    return FALSE;

    return TRUE;
}

void HashTable::Dump(ostream& out)
{
    out << '{';
    for(int i = 0; i < Size; ++i)
	if(entries[i].key != nil && entries[i].key != (char*)-1)
//	    out << entries[i].key << ", ";
	    out << entries[i].key << '(' << i << "), ";

    out << '}';
}

char* HashTable::KeyString(char* key) {
    char* str = new (GC) char[strlen(key) + 1];
    strcpy(str, key);
    return str;
}

char* HashTable::KeyString(char* key, int len) {
    char* str = new (GC) char[len + 1];
    memmove(str, key, len);
    str[len] = '\0';
    return str;
}

bool HashTable::Lookup(char* key, HashValue* value)
{
    int i;
    return Lookup2(key, value, &i);
}

bool HashTable::Lookup(char* key, int len, HashValue* value)
{
    int i;
    return Lookup2(key, len, value, &i);
}

bool HashTable::Lookup2(char* key, HashValue* value, int* index)
{
    unsigned int p = StringToInt(key);
    for(int i = 0; i < Size; ++i){
	int j = HashFunc(p, i);
	if(entries[j].key == nil){
	    return FALSE;		// not found
	}
	else if(entries[j].key != (char*)-1
				&& strcmp(entries[j].key, key) == 0){
	    *value = entries[j].value;
	    *index = j;
	    return TRUE;
	}
    }

    return FALSE;
}

bool HashTable::Lookup2(char* key, int len, HashValue* value, int* index)
{
    unsigned int p = StringToInt(key, len);
    for(int i = 0; i < Size; ++i){
	int j = HashFunc(p, i);
	if(entries[j].key == nil){
	    return FALSE;		// not found
	}
	else if(entries[j].key != (char*)-1
		&& strncmp(entries[j].key, key, len) == 0
		&& entries[j].key[len] == '\0'){
	    *value = entries[j].value;
	    *index = j;
	    return TRUE;
	}
    }

    return FALSE;
}

/*
  LookupEntries() is used to find multiple entries recorded with the
  same key.  It returns the entry found with the nth (>= 0) hash key.
  After this function completes, nth is increamented for the next try.
  The next entry can be found if nth is passed to LookupEntries() as is.
*/
bool HashTable::LookupEntries(char* key, int len, HashValue* value,
			      int& nth)
{
    unsigned int p = StringToInt(key, len);
    for(int i = nth; i < Size; ++i){
	int j = HashFunc(p, i);
	if(entries[j].key == nil){
	    return FALSE;		// not found
	}
	else if(entries[j].key != (char*)-1
		&& strncmp(entries[j].key, key, len) == 0
		&& entries[j].key[len] == '\0'){
	    *value = entries[j].value;
	    nth = i + 1;
	    return TRUE;
	}
    }

    return FALSE;
}

/*
  A naive implementation to calculate a prime number.
  This function returns the first prime number being greater
  than or equal to 'number'.
*/
uint HashTable::NextPrimeNumber(uint number)
{
    if(number < 2)
	return 2;

    for(;;){
        uint half = number / 2;
        bool prime = TRUE;
        for(uint i = 2; i <= half && prime; ++i)
            if(number % i == 0)
		prime = FALSE;

        if(prime)
	    return number;

        ++number;
    }
}

/*
  WARNING! When an hashtable is expanded, the elements change of position!
  This means that the index returned by some HashTable methods is safely valid
  until the next insertion of a new element. So don't store such an index for
  a long period!

  Post condition : new Size >= old Size + 2 * increment
*/
bool HashTable::GrowTable(int increment)
{
    HashTable bigger(0);

    MopWarningMessage2("The hash table is full.  ", "Expanded...");

    bigger.Prime2 = (int)NextPrimeNumber(Prime2 + increment);
    bigger.Size = (int)NextPrimeNumber(2 * bigger.Prime2);
    bigger.MakeTable();

    bool done = TRUE;
    for(int i = 0; done && i < Size; ++i) {
        char *key = this->entries[i].key;
        if (key != nil && key != (char*)-1)
	    done = bool(bigger.AddDupEntry(key, strlen(key), entries[i].value)
			>= 0);
    }

    if(done){
        entries = bigger.entries;
        Size = bigger.Size;
        Prime2 = bigger.Prime2;
    }

    return done;
}

// AddEntry adds a new entry to the hash table.
// If succeeding, this returns an index of the added entry, otherwise -1.
// Because `key' is duplicated, you can delete `key' later on.

int HashTable::AddEntry(char* key, HashValue value, int* index)
{
    unsigned int p = StringToInt(key);
    for(int i = 0; i < Size; ++i){
	int j = HashFunc(p, i);
	if(entries[j].key == nil || entries[j].key == (char*)-1){
	    entries[j].key = KeyString(key);
	    entries[j].value = value;
	    if(index != nil)
		*index = j;

	    return j;
	}
	else if(strcmp(entries[j].key, key) == 0){
	    if(index != nil)
		*index = j;

	    return -1;		// it is already registered.
	}
    }

    if(GrowTable(1000))
	return AddEntry(key, value, index);

    cerr << "HashTable overflow (key: " << key << ")\nPanic...\n";
    if(index != nil)
	*index = 0;		// no meaning

    return -1;
}

int HashTable::AddEntry(bool check_duplication,
			char* key, int len, HashValue value, int* index)
{
    int i;
    unsigned int p = StringToInt(key, len);
    for(i = 0; i < Size; ++i){
	int j = HashFunc(p, i);
	if(entries[j].key == nil || entries[j].key == (char*)-1){
	    entries[j].key = KeyString(key, len);
	    entries[j].value = value;
	    if(index != nil)
		*index = j;

	    return j;
	}
	else if(check_duplication
		&& strncmp(entries[j].key, key, len) == 0
		&& entries[j].key[len] == '\0'){
	    if(index != nil)
		*index = j;

	    return -1;		// it is already registered.
	}
    }

    if(GrowTable(1000))
	return AddEntry(check_duplication, key, len, value, index);

    cerr << "HashTable overflow (key: ";
    for(i = 0; i < len; ++i)
	cerr << key[i];

    cerr << ")\nPanic...\n";
    if(index != nil)
	*index = 0;		// no meaning

    return -1;
}

HashValue HashTable::Peek(int index)
{
    return entries[index].value;
}

void HashTable::ReplaceValue(int index, HashValue val)
{
    if(0 <= index && index < Size)
	entries[index].value = val;
    else
	cerr << "HashTable: invalid index (" << index << ")\n";
}

bool HashTable::RemoveEntry(char* key)
{
    HashValue	u;
    int		index;

    if(!Lookup2(key, &u, &index))
	return FALSE;		// not found
    else{
	entries[index].key = (char*)-1;
	return TRUE;
    }
}

bool HashTable::RemoveEntry(char* key, int len)
{
    HashValue	u;
    int		index;

    if(!Lookup2(key, len, &u, &index))
	return FALSE;		// not found
    else{
	entries[index].key = (char*)-1;
	return TRUE;
    }
}

unsigned int HashTable::StringToInt(char* key)
{
    if(key == nil)
	return 0;

    unsigned int p = 0;
    unsigned int i, j;
    for(i = j = 0; key[i] != '\0'; ++i, ++j){
	if(j >= sizeof(unsigned int) * 8 - 7)
	    j = 0;

	p += key[i] << j;
    }

    return p;
}

unsigned int HashTable::StringToInt(char* key, int len)
{
    if(key == nil)
	return 0;

    unsigned int p = 0;
    int i;
    unsigned int j;
    for(i = j = 0; i < len; ++i, ++j){
	if(j >= sizeof(unsigned int) * 8 - 7)
	    j = 0;

	p += key[i] << j;
    }

    return p;
}

int HashTable::HashFunc(unsigned int p, int n)
{
    return((p + (unsigned int)n * (p % Prime2 + 1)) % Size);
}


#ifdef TEST

#include <stdio.h>

int main()
{
    char	buf[128];
    int		v;
    HashTable	t;

    while(gets(buf) != NULL){
	unsigned int k = t.StringToInt(buf);
	printf("key %s (%d): %d\n", buf, k, t.HashFunc(k, 0));
	printf("key %s (%d): %d\n", buf, k, t.HashFunc(k, 1));

	printf("add %d, ", t.AddEntry(buf, 1));
	printf("lookup %d", t.Lookup(buf, v));
	printf("(%d)\n", v);
    }
}

#endif /* TEST */
