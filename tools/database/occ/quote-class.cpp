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

// Part of the implementation of QuoteClass is in metaclass.cc.

#include <string.h>
#include "ptree.h"
#include "quote-class.h"
#include "buffer.h"

Ptree* QuoteClass::TranslateMemberCall(Environment* env,
				       Ptree* member, Ptree* args)
{
    Ptree* name = StripClassQualifier(member);
    char* str;

    if(Ptree::Eq(name, "qMake")){
	Ptree* arg1 = Ptree::First(Ptree::Second(args));
	if(arg1->Reify(str) && str != nil)
	    return ProcessBackQuote(env, str, arg1, name);
	else
	    ErrorMessage(env, "bad argument for qMake()", arg1, name);
    }
    else if(Ptree::Eq(name, "qMakeStatement")){
	WarnObsoleteness("Ptree::qMakeStatement()", "Ptree::qMake()");
	Ptree* arg1 = Ptree::First(Ptree::Second(args));
	if(arg1->Reify(str) && str != nil)
	    return ProcessBackQuote(env, str, arg1, name);
	else
	    ErrorMessage(env, "bad argument for qMakeStatement()", arg1, name);
    }

    return Class::TranslateMemberCall(env, member, args);
}

Ptree* QuoteClass::ProcessBackQuote(Environment* env,
				    char* str, Ptree* arg, Ptree* exp)
{
    ProgramString result;

    result << "(Ptree*)(PtreeHead()";
    while(*str != '\0')
	if(*str == '`'){
	    result << '+';
	    while(*++str != '`')
		if(*str != '\0')
		    result << *str;
		else{
		    ErrorMessage(env,
				 "unmatched backquote for Ptree::qMake(): ",
				 arg, exp);
		    break;
		}

	    ++str;
	}
	else{
	    result << "+\"";
	    while(*str != '`' && *str != '\0')
		result << *str++;

	    result << '"';
	}

    result << ')';
    char* rstr = (char*)result.Read(0);
    return new DupLeaf(rstr, strlen(rstr));
}
