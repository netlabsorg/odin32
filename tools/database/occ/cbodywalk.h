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

#ifndef _cbodywalk_h
#define _cbodywalk_h

#include "classwalk.h"

// ClassBodyWalker is only used by ClassWalker::TranslateClassSpec.

class ClassBodyWalker : public ClassWalker {
public:
    ClassBodyWalker(Walker* w, PtreeArray* tlist) : ClassWalker(w) {
	tspec_list = tlist;
    }
    Ptree* TranslateClassBody(Ptree* block, Ptree* bases, Class*);
    void AppendNewMembers(Class*, PtreeArray&, bool&);
    Ptree* TranslateTypespecifier(Ptree* tspec);
    Ptree* TranslateTypedef(Ptree* def);
    Ptree* TranslateMetaclassDecl(Ptree* decl);
    Ptree* TranslateDeclarators(Ptree* decls);
    Ptree* TranslateAssignInitializer(PtreeDeclarator* decl, Ptree* init);
    Ptree* TranslateInitializeArgs(PtreeDeclarator* decl, Ptree* init);
    Ptree* TranslateDeclarator(bool record, PtreeDeclarator* decl);
    Ptree* TranslateDeclarator(bool record, PtreeDeclarator* decl,
			       bool append_body);
    Ptree* TranslateFunctionImplementation(Ptree* impl);

private:
    PtreeArray* tspec_list;
};

#endif /* _cbodywalk_h */
