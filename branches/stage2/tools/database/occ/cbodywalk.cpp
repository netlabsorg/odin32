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

#include "ptree.h"
#include "cbodywalk.h"
#include "mop.h"
#include "env.h"
#include "member.h"

Ptree* ClassBodyWalker::TranslateClassBody(Ptree* block, Ptree*,
					   Class* metaobject)
{
    Ptree* block2;

    Environment* fenv = metaobject->GetEnvironment();
    if(fenv == nil)
	fenv = env;	// should not reach here.

    NameScope old_env = ChangeScope(fenv);

    PtreeArray array;
    bool changed = FALSE;
    Ptree* body = Ptree::Second(block);
    Ptree* rest = body;
    while(rest != nil){
	Ptree* p = rest->Car();
	Ptree* q = Translate(p);
	array.Append(q);
	if(p != q)
	    changed = TRUE;

	rest = rest->Cdr();
    }

    AppendNewMembers(metaobject, array, changed);

    Ptree* appended = metaobject->GetAppendedCode();
    if(appended != nil){
	changed = TRUE;
	while(appended != nil){
	    array.Append(appended->Car());
	    appended = appended->Cdr();
	}
    }

    if(changed)
	block2 = new PtreeClassBody(Ptree::First(block), array.All(),
				    Ptree::Third(block));
    else
	block2 = block;

    RestoreScope(old_env);
    return block2;
}

void ClassBodyWalker::AppendNewMembers(Class* metaobject, PtreeArray& array,
				       bool& changed)
{
    ChangedMemberList::Cmem* m;
    ChangedMemberList* appended_list = metaobject->GetAppendedMembers();
    if(appended_list == nil)
	return;

    int i = 0;
    while((m = appended_list->Get(i++)) != nil)
	if(m->def != nil){
	    changed = TRUE;
	    ClassWalker w(this);
	    array.Append(w.ConstructAccessSpecifier(m->access));
	    array.Append(w.ConstructMember(m));
	}
}

Ptree* ClassBodyWalker::TranslateTypespecifier(Ptree* tspec)
{
    if(tspec_list == nil)
	return tspec;

    int n = tspec_list->Number();
    for(int i =  0; i < n; i += 2)
	if(tspec_list->Ref(i) == tspec)
	    return tspec_list->Ref(i + 1);

    return tspec;
}

Ptree* ClassBodyWalker::TranslateTypedef(Ptree* def)
{
    Ptree *tspec, *tspec2;

    tspec = Ptree::Second(def);
    tspec2 = TranslateTypespecifier(tspec);

    if(tspec == tspec2)
	return def;
    else
	return new PtreeTypedef(Ptree::First(def),
				Ptree::List(tspec2,
					    Ptree::ListTail(def, 2)));
}

Ptree* ClassBodyWalker::TranslateMetaclassDecl(Ptree*)
{
    return nil;
}

Ptree* ClassBodyWalker::TranslateDeclarators(Ptree* decls)
{
    return ClassWalker::TranslateDeclarators(decls, FALSE);
}

Ptree* ClassBodyWalker::TranslateAssignInitializer(PtreeDeclarator* decl,
						   Ptree* init)
{
    ClassWalker w(this);

    return w.TranslateAssignInitializer(decl, init);
}

Ptree* ClassBodyWalker::TranslateInitializeArgs(PtreeDeclarator* decl,
						Ptree* init)
{
    ClassWalker w(this);

    return w.TranslateInitializeArgs(decl, init);
}

Ptree* ClassBodyWalker::TranslateDeclarator(bool record,
					    PtreeDeclarator* decl)
{
    return TranslateDeclarator(record, decl, TRUE);
}

Ptree* ClassBodyWalker::TranslateDeclarator(bool record,
					    PtreeDeclarator* decl,
					    bool append_body)
{
    ClassWalker w(this);

    Class* metaobject = env->LookupThis();
    if(metaobject != nil){
	ChangedMemberList::Cmem* m = metaobject->GetChangedMember(decl);
	if(m != nil){
	    Ptree* decl2 = w.MakeMemberDeclarator(record, m, decl);
	    if(m->removed || m->body == nil || !append_body)
		return decl2;
	    else
		return Ptree::List(decl2, m->body);
	}
    }

    return w.TranslateDeclarator(record, decl);
}

Ptree* ClassBodyWalker::TranslateFunctionImplementation(Ptree* impl)
{
    Ptree* sspec = impl->First();
    Ptree* sspec2 = TranslateStorageSpecifiers(sspec);
    Ptree* tspec = impl->Second();
    Ptree* decl = impl->Third();
    Ptree* body = impl->Nth(3);
    Ptree* decl2;
    Ptree* body2;

    Ptree* tspec2 = TranslateTypespecifier(tspec);
    Environment* fenv = env->DontRecordDeclarator(decl);

    if(fenv == nil){
	// shouldn't reach here.
	NewScope();
	ClassWalker w(this);	// this should be after NewScope().
	decl2 = w.TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	body2 = w.TranslateFunctionBody(body);
	ExitScope();
    }
    else{
	bool is_nested_class = bool(env != fenv);
	NameScope old_env = ChangeScope(fenv);
	NewScope();
	ClassWalker w(this);
	if(is_nested_class){
	    // if it's a member function of a nested class
	    decl2 = w.TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	    body2 = w.TranslateFunctionBody(body);
	}
	else{
	    decl2 = TranslateDeclarator(TRUE, (PtreeDeclarator*)decl, FALSE);
	    Class* metaobject = fenv->IsClassEnvironment();
	    ChangedMemberList::Cmem* m = nil;
	    if(metaobject != nil)
		m = metaobject->GetChangedMember(decl);

	    if(m != nil && m->body != nil)
		body2 = m->body;
	    else
		body2 = w.TranslateFunctionBody(body);
	}

	ExitScope();
	RestoreScope(old_env);
    }

    if(sspec == sspec2 && tspec == tspec2 && decl == decl2 && body == body2)
	return impl;
    else if(decl2 == nil)
	return new PtreeDeclaration(nil, Ptree::List(nil,
						     Class::semicolon_t));
    else
	return new PtreeDeclaration(sspec2,
				    Ptree::List(tspec2, decl2, body2));
}


