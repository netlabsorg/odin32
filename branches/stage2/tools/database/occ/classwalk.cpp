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
#include "classwalk.h"
#include "cbodywalk.h"
#include "mop.h"
#include "env.h"
#include "typeinfo.h"
#include "member.h"
#include "encoding.h"

Class* ClassWalker::GetClassMetaobject(TypeInfo& tinfo)
{
    Class* c;
    if(tinfo.IsClass(c))
	return c;
    else if(tinfo.IsReferenceType()){
	tinfo.Dereference();
	if(tinfo.IsClass(c))
	    return c;
    }

    return nil;
}

bool ClassWalker::IsClassWalker()
{
    return TRUE;
}

void ClassWalker::InsertBeforeStatement(Ptree* p)
{
    before_statement.Append(p);
}

void ClassWalker::AppendAfterStatement(Ptree* p)
{
    after_statement.Append(p);
}

void ClassWalker::InsertBeforeToplevel(Ptree* p)
{
    before_toplevel.Append(p);
}

void ClassWalker::AppendAfterToplevel(Ptree* p)
{
    after_toplevel.Append(p);
}

bool ClassWalker::InsertDeclaration(Ptree* d, Class* metaobject, Ptree* key,
				    void* data)
{
    inserted_declarations.Append(d);
    if(metaobject == nil || key == nil)
	return TRUE;
    else if(LookupClientData(metaobject, key))
	return FALSE;
    else{
	ClientDataLink* entry = new ClientDataLink;
	entry->next = client_data;
	entry->metaobject = metaobject;
	entry->key = key;
	entry->data = data;
	client_data = entry;
	return TRUE;
    }
}

void* ClassWalker::LookupClientData(Class* metaobject, Ptree* key)
{
    for(ClientDataLink* c = client_data; c != nil; c = c->next)
	if(c->metaobject == metaobject && Ptree::Eq(key, c->key))
	    return c->data;

    return nil;
}

Ptree* ClassWalker::GetInsertedPtree()
{
    Ptree* result = nil;
    if(before_toplevel.Number() > 0)
	result = Ptree::Nconc(result, before_toplevel.All());

    if(inserted_declarations.Number() > 0)
	result = Ptree::Nconc(result, inserted_declarations.All());

    if(before_statement.Number() > 0)
	result = Ptree::Nconc(result, before_statement.All());

    before_statement.Clear();
    inserted_declarations.Clear();
    client_data = nil;
    before_toplevel.Clear();
    return result;
}

Ptree* ClassWalker::GetAppendedPtree()
{
    Ptree* result = nil;
    if(after_statement.Number() > 0)
	result = Ptree::Nconc(result, after_statement.All());

    if(after_toplevel.Number() > 0)
	result = Ptree::Nconc(result, after_toplevel.All());

    after_statement.Clear();
    after_toplevel.Clear();
    return result;
}

Ptree* ClassWalker::TranslateMetaclassDecl(Ptree* decl)
{
    env->RecordMetaclassName(decl);
    return nil;
}

Ptree* ClassWalker::TranslateClassSpec(Ptree* spec, Ptree* userkey,
				       Ptree* class_def, Class* metaobject)
{
    if(metaobject != nil){
	// the class body is given.
	Ptree* bases = class_def->Third();
	PtreeArray* tspec_list = RecordMembers(class_def, bases, metaobject);
	metaobject->TranslateClass(env);
	metaobject->TranslateClassHasFinished();
	if(metaobject->removed)
	    return nil;

	ClassBodyWalker w(this, tspec_list);
	Ptree* body = class_def->Nth(3);
	Ptree* body2 = w.TranslateClassBody(body, class_def->Third(),
					    metaobject);
	Ptree* bases2 = metaobject->GetBaseClasses();
	Ptree* cspec = metaobject->GetClassSpecifier();
	Ptree* name2 = metaobject->GetNewName();
	if(bases != bases2 || body != body2 || cspec != nil || name2 != nil){
	    if(name2 == nil)
		name2 = class_def->Second();

	    Ptree* rest = Ptree::List(name2, bases2, body2);
	    if(cspec != nil)
		rest = Ptree::Cons(cspec, rest);
	    return new PtreeClassSpec(class_def->Car(), rest, nil,
				      spec->GetEncodedName());
	}
    }

    if(userkey == nil)
	return spec;
    else
	return new PtreeClassSpec(class_def->Car(), class_def->Cdr(),
				  nil, spec->GetEncodedName());
}

Ptree* ClassWalker::TranslateTemplateInstantiation(Ptree* inst_spec,
			Ptree* userkey, Ptree* class_spec, Class* metaobject)
{
    Ptree* class_spec2;
    if (metaobject != nil && metaobject->AcceptTemplate()) {
	TemplateClass* tmetaobj = (TemplateClass*)metaobject;
	class_spec2 = tmetaobj->TranslateInstantiation(env, class_spec);
	if (class_spec != class_spec2)
	    return class_spec2;
    }
    else
	class_spec2 = class_spec;

    if(userkey == nil)
	return inst_spec;
    else if (class_spec == class_spec2)
	return inst_spec;
    else
	return new PtreeTemplateInstantiation(class_spec);
}

Ptree* ClassWalker::ConstructClass(Class* metaobject)
{
    Ptree* def = metaobject->Definition();
    Ptree* def2;

    metaobject->TranslateClassHasFinished();
    ClassBodyWalker w(this, nil);
    Ptree* body = def->Nth(3);
    Ptree* body2 = w.TranslateClassBody(body, nil, metaobject);
    Ptree* bases2 = metaobject->GetBaseClasses();
    Ptree* cspec2 = metaobject->GetClassSpecifier();
    Ptree* name2 = metaobject->GetNewName();
    if(body == body2 && bases2 == nil && cspec2 == nil && name2 == nil)
	def2 = def;
    else{
	if(name2 == nil)
	    name2 = def->Second();

	Ptree* rest = Ptree::List(name2, bases2, body2);
	if(cspec2 != nil)
	    rest = Ptree::Cons(cspec2, rest);

	def2 = new PtreeClassSpec(def->Car(), rest,
				  nil, def->GetEncodedName());
    }

    return new PtreeDeclaration(nil, Ptree::List(def2, Class::semicolon_t));
}

PtreeArray* ClassWalker::RecordMembers(Ptree* class_def, Ptree* bases,
				       Class* metaobject)
{
    Ptree *tspec, *tspec2;

    NewScope(metaobject);
    RecordBaseclassEnv(bases);

    PtreeArray* tspec_list = new PtreeArray();

    Ptree* rest = class_def->Nth(3)->Second();
    while(rest != nil){
	Ptree* mem = rest->Car();
	switch(mem->What()){
	case ntTypedef :
	    tspec = mem->Second();
	    tspec2 = TranslateTypespecifier(tspec);
	    env->RecordTypedefName(mem->Third());
	    if(tspec != tspec2){
		tspec_list->Append(tspec);
		tspec_list->Append(tspec2);
	    }

	    break;
	case ntMetaclassDecl :
	    env->RecordMetaclassName(mem);
	    break;
	case ntDeclaration :
	    RecordMemberDeclaration(mem, tspec_list);
	    break;
	case ntTemplateDecl :
	case ntTemplateInstantiation :
	case ntUsing :
	default :
	    break;
	}

	rest = rest->Cdr();
    }

    if(tspec_list->Number() == 0){
	delete tspec_list;
	tspec_list = nil;
    }
	
    ExitScope();
    return tspec_list;
}

//  RecordMemberDeclaration() is derived from TranslateDeclaration().

void ClassWalker::RecordMemberDeclaration(Ptree* mem,
					  PtreeArray* tspec_list)
{
    Ptree *tspec, *tspec2, *decls;

    tspec = mem->Second();
    tspec2 = TranslateTypespecifier(tspec);
    decls = mem->Third();
    if(decls->IsA(ntDeclarator))	// if it is a function
	env->RecordDeclarator(decls);
    else if(!decls->IsLeaf())		// not a null declaration.
	while(decls != nil){
	    Ptree* d = decls->Car();
	    if(d->IsA(ntDeclarator))
		env->RecordDeclarator(d);

	    decls = decls->Cdr();
	    if(decls != nil)
		decls = decls->Cdr();
	}

    if(tspec != tspec2){
	tspec_list->Append(tspec);
	tspec_list->Append(tspec2);
    }
}

Ptree* ClassWalker::ConstructAccessSpecifier(int access)
{
    Ptree* lf;
    switch(access){
    case Class::Protected :
        lf = Class::protected_t;
	break;
    case Class::Private :
	lf = Class::private_t;
	break;
    case Class::Public :
    default :
	lf = Class::public_t;
	break;
     }

     return new PtreeAccessSpec(lf, Ptree::List(Class::colon_t));
}

Ptree* ClassWalker::ConstructMember(void* ptr)
{
    ChangedMemberList::Cmem* m = (ChangedMemberList::Cmem*)ptr;
    Ptree* def = m->def;
    Ptree* def2;

    if(def->Third()->IsA(ntDeclarator)){
	// function implementation
	if(m->body == nil){
	    NameScope old_env;
	    Environment* fenv = env->DontRecordDeclarator(m->declarator);
	    if(fenv != nil)
		old_env = ChangeScope(fenv);

	    NewScope();
	    def2 = MakeMemberDeclarator(TRUE, m,
					(PtreeDeclarator*)m->declarator);
	    def2 = Ptree::List(def2,
			       TranslateFunctionBody(def->Nth(3)));
	    ExitScope();
	    if(fenv != nil)
		RestoreScope(old_env);
	}
	else{
	    def2 = MakeMemberDeclarator(FALSE, m,
					(PtreeDeclarator*)m->declarator);
	    def2 = Ptree::List(def2, m->body);
	}
    }
    else{
	// declaration
	def2 = MakeMemberDeclarator(FALSE, m,
				    (PtreeDeclarator*)m->declarator);
	if(m->body == nil)
	    def2 = Ptree::List(Ptree::List(def2), Class::semicolon_t);
	else
	    def2 = Ptree::List(def2, m->body);
    }

    def2 = new PtreeDeclaration(
			TranslateStorageSpecifiers(def->First()),
			Ptree::Cons(TranslateTypespecifier(def->Second()),
				    def2));
    return def2;
}

Ptree* ClassWalker::TranslateStorageSpecifiers(Ptree* spec)
{
    return TranslateStorageSpecifiers2(spec);
}

Ptree* ClassWalker::TranslateStorageSpecifiers2(Ptree* rest)
{
    if(rest == nil)
	return nil;
    else{
	Ptree* h = rest->Car();
	Ptree* t = rest->Cdr();
	Ptree* t2 = TranslateStorageSpecifiers2(t);
	if(h->IsA(ntUserdefKeyword))
	    return t2;
	else if(t == t2)
	    return rest;
	else
	    return Ptree::Cons(h, t2);
    }
}

Ptree* ClassWalker::TranslateTemplateFunction(Ptree* temp_def, Ptree* impl)
{
    Environment* fenv = env->RecordTemplateFunction(temp_def, impl);
    if (fenv != nil) {
	Class* metaobject = fenv->IsClassEnvironment();
	if(metaobject != nil){
	    NameScope old_env = ChangeScope(fenv);
	    NewScope();

	    ChangedMemberList::Cmem m;
	    Ptree* decl = impl->Third();
	    MemberFunction mem(metaobject, impl, decl);
	    metaobject->TranslateMemberFunction(env, mem);
	    ChangedMemberList::Copy(&mem, &m, Class::Undefined);
	    Ptree* decl2
		= MakeMemberDeclarator(TRUE, &m, (PtreeDeclarator*)decl);

	    ExitScope();
	    RestoreScope(old_env);
	    if(decl != decl2) {
		Ptree* pt = Ptree::List(impl->Second(), decl2, impl->Nth(3));
		pt = new PtreeDeclaration(impl->First(), pt);
		pt = Ptree::List(temp_def->Second(), temp_def->Third(),
				 temp_def->Nth(3), pt);
		return new PtreeTemplateDecl(temp_def->First(), pt);
	    }
	}
    }

    return temp_def;
}

Ptree* ClassWalker::TranslateFunctionImplementation(Ptree* impl)
{
    Ptree* sspec = impl->First();
    Ptree* sspec2 = TranslateStorageSpecifiers(sspec);
    Ptree* tspec = impl->Second();
    Ptree* decl = impl->Third();
    Ptree* body = impl->Nth(3);
    Ptree* decl2;
    Ptree *body2;

    Ptree* tspec2 = TranslateTypespecifier(tspec);
    Environment* fenv = env->RecordDeclarator(decl);

    if(fenv == nil){
	// reach here if resolving the qualified name fails. error?
	NewScope();
	decl2 = TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	body2 = TranslateFunctionBody(body);
	ExitScope();
    }
    else{
	Class* metaobject = fenv->IsClassEnvironment();
	NameScope old_env = ChangeScope(fenv);
	NewScope();

	if (metaobject == nil && Class::metaclass_for_c_functions != nil)
	    metaobject = MakeMetaobjectForCfunctions();

	if(metaobject == nil){
	    decl2 = TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	    body2 = TranslateFunctionBody(body);
	}
	else{
	    ChangedMemberList::Cmem m;
	    MemberFunction mem(metaobject, impl, decl);
	    metaobject->TranslateMemberFunction(env, mem);
	    ChangedMemberList::Copy(&mem, &m, Class::Undefined);
	    decl2 = MakeMemberDeclarator(TRUE, &m, (PtreeDeclarator*)decl);
	    if(m.body != nil)
		body2 = m.body;
	    else
		body2 = TranslateFunctionBody(body);
	}

	ExitScope();
	RestoreScope(old_env);
    }

    if(sspec == sspec2 && tspec == tspec2 && decl == decl2 && body == body2)
	return impl;
    else
	return new PtreeDeclaration(sspec2,
				    Ptree::List(tspec2, decl2, body2));
}

Class* ClassWalker::MakeMetaobjectForCfunctions() {
    if (Class::for_c_functions == nil) {
	Encoding encode;
	Ptree* name = new Leaf("<C>", 3);
	encode.SimpleName(name);
	Ptree* class_def
	    = new PtreeClassSpec(Class::class_t,
				 Ptree::List(name, nil,
					     Class::empty_block_t),
				 nil, encode.Get());
	cerr << "encode: " << class_def->GetEncodedName();
	Class* metaobject = opcxx_ListOfMetaclass::New(
			Class::metaclass_for_c_functions,
			class_def, nil);
	if(metaobject == nil)
	    MopErrorMessage2(
		"the metaclass for C functions cannot be loaded: ",
		Class::metaclass_for_c_functions);

	metaobject->SetEnvironment(env);
	Class::for_c_functions = metaobject;
    }

    return Class::for_c_functions;
}

Ptree* ClassWalker::MakeMemberDeclarator(bool record, void* ptr,
					 PtreeDeclarator* decl)
{
    Ptree *args, *args2, *name, *name2, *init, *init2;

    //  Since g++ cannot parse the nested-class declaration:
    //     class ChangedMemberList::Cmem;
    //  MakeMemberDeclarator() takes a void* pointer and convert the type
    //  into ChangedMemberList::Cmem*.
    ChangedMemberList::Cmem* m = (ChangedMemberList::Cmem*)ptr;

    if(m->removed)
	return nil;

    if(GetArgDeclList(decl, args))
	if(m->args == nil)
	    args2 = TranslateArgDeclList2(record, env, TRUE,
					  m->arg_name_filled, 0, args);
	else{
	    args2 = m->args;
	    // we may need to record the arguments.
	    TranslateArgDeclList2(record, env, FALSE, FALSE, 0, args);
	}
    else
	args = args2 = nil;

    name = decl->Name();
    if(m->name != nil)
	name2 = m->name;
    else
	name2 = name;

    if(m->init == nil)
	init = init2 = nil;
    else{
	init2 = m->init;
	init = decl->Last()->Car();
	if(init->IsLeaf() || !init->Car()->Eq(':'))
	    init = nil;
    }

    if(args == args2 && name == name2 && init == init2)
	return decl;
    else{
	Ptree* rest;
	if(init == nil && init2 != nil){
	    rest = Ptree::Subst(args2, args, name2, name, decl->Cdr());
	    rest = Ptree::Append(rest, init2);
	}
	else
	    rest = Ptree::Subst(args2, args, name2, name,
				init2, init, decl->Cdr());

	if(decl->Car() == name)
	    return new PtreeDeclarator(decl, name2, rest);
	else
	    return new PtreeDeclarator(decl, decl->Car(), rest);
    }
}

Ptree* ClassWalker::RecordArgsAndTranslateFbody(Class* c, Ptree* args,
						Ptree* body)
{
    NameScope old_env;
    Environment* fenv = c->GetEnvironment();

    if(fenv != nil)
	old_env = ChangeScope(fenv);

    NewScope();
    TranslateArgDeclList2(TRUE, env, FALSE, FALSE, 0, args);
    Ptree* body2 = TranslateFunctionBody(body);
    ExitScope();

    if(fenv != nil)
	RestoreScope(old_env);

    return body2;
}

Ptree* ClassWalker::TranslateFunctionBody(Ptree* body)
{
    Ptree* body2;

    inserted_declarations.Clear();
    client_data = nil;
    body = Translate(body);
    if(body == nil || body->IsLeaf() || inserted_declarations.Number() <= 0)
	body2 = body;
    else{
	Ptree* decls = inserted_declarations.All();
	body2 = new PtreeBlock(Ptree::First(body),
			      Ptree::Nconc(decls, Ptree::Second(body)),
			      Ptree::Third(body));
    }

    inserted_declarations.Clear();
    client_data = nil;
    return body2;
}

Ptree* ClassWalker::TranslateBlock(Ptree* block)
{
    Ptree* block2;

    NewScope();

    PtreeArray array;
    bool changed = FALSE;
    Ptree* body = Ptree::Second(block);
    Ptree* rest = body;
    while(rest != nil){
	uint i, n;
	Ptree* p = rest->Car();
	Ptree* q = Translate(p);

	n = before_statement.Number();
	if(n > 0){
	    changed = TRUE;
	    for(i = 0; i < n; ++i)
		array.Append(before_statement[i]);
	}

	array.Append(q);
	if(p != q)
	    changed = TRUE;

	n = after_statement.Number();
	if(n > 0){
	    changed = TRUE;
	    for(i = 0; i < n; ++i)
		array.Append(after_statement[i]);
	}

	before_statement.Clear();
	after_statement.Clear();
	rest = rest->Cdr();
    }

    if(changed)
	block2 = new PtreeBlock(Ptree::First(block), array.All(),
				Ptree::Third(block));
    else
	block2 = block;

    ExitScope();
    return block2;
}

Ptree* ClassWalker::TranslateArgDeclList(bool record, Ptree*, Ptree* args)
{
    return TranslateArgDeclList2(record, env, TRUE, FALSE, 0, args);
}

Ptree* ClassWalker::TranslateInitializeArgs(PtreeDeclarator* decl, Ptree* init)
{
    TypeInfo tinfo;
    env->Lookup(decl, tinfo);
    Class* metaobject = tinfo.ClassMetaobject();
    if(metaobject != nil)
	return metaobject->TranslateInitializer(env, decl->Name(), init);
    else
	return TranslateArguments(init);
}

Ptree* ClassWalker::TranslateAssignInitializer(PtreeDeclarator* decl,
					       Ptree* init)
{
    TypeInfo tinfo;
    env->Lookup(decl, tinfo);
    Class* metaobject = tinfo.ClassMetaobject();
    if(metaobject != nil)
	return metaobject->TranslateInitializer(env, decl->Name(), init);
    else{
	Ptree* exp = init->Second();
	Ptree* exp2 = Translate(exp);
	if(exp == exp2)
	    return init;
	else
	    return Ptree::List(init->Car(), exp2);
    }
}

Ptree* ClassWalker::TranslateUserAccessSpec(Ptree*)
{
    return nil;
}

Ptree* ClassWalker::TranslateAssign(Ptree* exp)
{
    Ptree *left, *left2, *right, *right2, *exp2;
    Environment* scope;
    Class* metaobject;
    TypeInfo type;

    left = exp->First();
    right = exp->Third();
    if(left->IsA(ntDotMemberExpr, ntArrowMemberExpr)){
	Ptree* object = left->First();
	Ptree* op = left->Second();
	Ptree* member = left->Third();
	Ptree* assign_op = exp->Second();
	Typeof(object, type);
	if(!op->Eq('.'))
	    type.Dereference();

	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslateMemberWrite(env, object, op,
						    member, assign_op, right);
	    return CheckMemberEquiv(exp, exp2);
	}
    }
    else if((scope = env->IsMember(left)) != nil){
	metaobject = scope->IsClassEnvironment();
	if(metaobject != nil){
	    exp2 = metaobject->TranslateMemberWrite(env, left, exp->Second(),
						    right);
	    return CheckEquiv(exp, exp2);
	}
    }
    else{
	Typeof(left, type);
	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslateAssign(env, left, exp->Second(),
					       right);
	    return CheckEquiv(exp, exp2);	
	}
    }

    left2 = Translate(left);
    right2 = Translate(right);
    if(left == left2 && right == right2)
	return exp;
    else
	return new PtreeAssignExpr(left2, Ptree::List(exp->Second(), right2));
}

Ptree* ClassWalker::CheckMemberEquiv(Ptree* exp, Ptree* exp2)
{
    if(!exp2->IsLeaf()
       && Ptree::Equiv(exp->Car(), exp2->Car())
       && Ptree::Equiv(exp->Cdr(), exp2->Cdr()))
	return exp;
    else
	return exp2;
}

Ptree* ClassWalker::TranslateInfix(Ptree* exp)
{
    TypeInfo type;

    Ptree* left = exp->First();
    Ptree* right = exp->Third();
    Typeof(right, type);
    Class* metaobject = GetClassMetaobject(type);
    if(metaobject == nil){
	Typeof(left, type);
	metaobject = GetClassMetaobject(type);
    }

    if(metaobject != nil){
	Ptree* exp2 = metaobject->TranslateBinary(env, left, exp->Second(),
						  right);
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* left2 = Translate(left);
	Ptree* right2 = Translate(right);
	if(left == left2 && right == right2)
	    return exp;
	else
	    return new PtreeInfixExpr(left2, Ptree::List(exp->Second(),
							 right2));
    }
}

Ptree* ClassWalker::TranslateUnary(Ptree* exp)
{
    TypeInfo type;
    Environment* scope;
    Class* metaobject;
    Ptree* exp2;

    Ptree* unaryop = exp->Car();
    Ptree* right = exp->Second();
    if(right->IsA(ntDotMemberExpr, ntArrowMemberExpr)){
	Ptree* object = right->First();
	Ptree* op = right->Second();
	Typeof(object, type);
	if(!op->Eq('.'))
	    type.Dereference();

	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslateUnaryOnMember(env, unaryop, object,
						      op, right->Third());
	    if(exp2->Length() == 2 && exp2->Car() == unaryop
	       && Ptree::Equiv(exp2->Second(), right))
		return exp;
	    else
		return exp2;
	}
    }    
    else if((scope = env->IsMember(right)) != nil){
	metaobject = scope->IsClassEnvironment();
	if(metaobject != nil){
	    exp2 = metaobject->TranslateUnaryOnMember(env, unaryop, right);
	    return CheckEquiv(exp, exp2);
	}
    }

    Typeof(right, type);
    metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	Ptree* exp2 = metaobject->TranslateUnary(env, unaryop, right);
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* right2 = Translate(right);
	if(right == right2)
	    return exp;
	else
	    return new PtreeUnaryExpr(unaryop, Ptree::List(right2));
    }
}

Ptree* ClassWalker::TranslateArray(Ptree* exp)
{
    TypeInfo type;

    Ptree* array = exp->Car();
    Typeof(array, type);
    Class* metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	Ptree* exp2 = metaobject->TranslateSubscript(env, array, exp->Cdr());
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* index = exp->Third();
	Ptree* array2 = Translate(array);
	Ptree* index2 = Translate(index);
	if(array == array2 && index == index2)
	    return exp;
	else
	    return new PtreeArrayExpr(array2,
			Ptree::ShallowSubst(index2, index, exp->Cdr()));
    }
}

Ptree* ClassWalker::TranslatePostfix(Ptree* exp)
{
    TypeInfo type;
    Environment* scope;
    Class* metaobject;
    Ptree* exp2;

    Ptree* left = exp->Car();
    Ptree* postop = exp->Second();
    if(left->IsA(ntDotMemberExpr, ntArrowMemberExpr)){
	Ptree* object = left->First();
	Ptree* op = left->Second();
	Typeof(object, type);
	if(!op->Eq('.'))
	    type.Dereference();

	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslatePostfixOnMember(env, object, op,
							left->Third(), postop);
	    return CheckMemberEquiv(exp, exp2);
	}
    }    
    else if((scope = env->IsMember(left)) != nil){
	metaobject = scope->IsClassEnvironment();
	if(metaobject != nil){
	    exp2 = metaobject->TranslatePostfixOnMember(env, left, postop);
	    return CheckEquiv(exp, exp2);
	}
    }

    Typeof(left, type);
    metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	exp2 = metaobject->TranslatePostfix(env, left, postop);
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* left2 = Translate(left);
	if(left == left2)
	    return exp;
	else
	    return new PtreePostfixExpr(left2, exp->Cdr());
    }
}

Ptree* ClassWalker::TranslateFuncall(Ptree* exp)
{
    TypeInfo type;
    Environment* scope;
    Class* metaobject;
    Ptree *fun, *arglist, *exp2;

    fun = exp->Car();
    arglist = exp->Cdr();
    if(fun->IsA(ntDotMemberExpr, ntArrowMemberExpr)){
	Ptree* object = fun->First();
	Ptree* op = fun->Second();
	Ptree* member = fun->Third();
	Typeof(object, type);
	if(!op->Eq('.'))
	    type.Dereference();

	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslateMemberCall(env, object, op,
						   member, arglist);
	    return CheckMemberEquiv(exp, exp2);
	}
    }
    else if((scope = env->IsMember(fun)) != nil){
	metaobject = scope->IsClassEnvironment();
	if(metaobject != nil){
	    exp2 = metaobject->TranslateMemberCall(env, fun, arglist);
	    return CheckEquiv(exp, exp2);
	}
    }
    else{
	Typeof(fun, type);
	metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    exp2 = metaobject->TranslateFunctionCall(env, fun, arglist);
	    return CheckEquiv(exp, exp2);
	}
    }

    Ptree* fun2 = Translate(fun);
    Ptree* arglist2 = TranslateArguments(arglist);
    if(fun == fun2 && arglist == arglist2)
	return exp;
    else
	return new PtreeFuncallExpr(fun2, arglist2);
}

Ptree* ClassWalker::TranslateDotMember(Ptree* exp)
{
    TypeInfo type;

    Ptree* left = exp->Car();
    Typeof(left, type);
    Class* metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	Ptree* exp2 = metaobject->TranslateMemberRead(env, left, exp->Second(),
						      exp->Third());
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* left2 = Translate(left);
	if(left == left2)
	    return exp;
	else
	    return new PtreeDotMemberExpr(left2, exp->Cdr());
    }
}

Ptree* ClassWalker::TranslateArrowMember(Ptree* exp)
{
    TypeInfo type;

    Ptree* left = exp->Car();
    Typeof(left, type);
    type.Dereference();
    Class* metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	Ptree* exp2 = metaobject->TranslateMemberRead(env, left, exp->Second(),
						      exp->Third());
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* left2 = Translate(left);
	if(left == left2)
	    return exp;
	else
	    return new PtreeArrowMemberExpr(left2, exp->Cdr());
    }
}

Ptree* ClassWalker::TranslateThis(Ptree* exp)
{
    TypeInfo type;
    Typeof(exp, type);
    type.Dereference();
    Class* metaobject = GetClassMetaobject(type);
    if(metaobject != nil)
	return metaobject->TranslatePointer(env, exp);
    else
	return exp;
}

Ptree* ClassWalker::TranslateVariable(Ptree* exp)
{
    Environment* scope = env->IsMember(exp);
    if(scope != nil){
	Class* metaobject = scope->IsClassEnvironment();
	if(metaobject != nil)
	    return metaobject->TranslateMemberRead(env, exp);
    }

    TypeInfo type;
    Typeof(exp, type);
    if(type.IsPointerType()){
	type.Dereference();
	Class* metaobject = GetClassMetaobject(type);
	if(metaobject != nil)
	    return metaobject->TranslatePointer(env, exp);
    }

    return exp;
}

Ptree* ClassWalker::TranslateUserStatement(Ptree* exp)
{
    TypeInfo type;
    Ptree* object = exp->First();
    Ptree* op = exp->Second();
    Ptree* keyword = exp->Third();
    Ptree* rest = exp->ListTail(3);

    Typeof(object, type);
    if(!op->Eq('.'))
	type.Dereference();

    Class* metaobject = GetClassMetaobject(type);
    if(metaobject != nil){
	NewScope();
	if(keyword->IsA(UserKeyword2))		// closure style
	    TranslateArgDeclList2(TRUE, env, FALSE, FALSE, 0, rest->Second());

	Ptree* exp2 = metaobject->TranslateUserStatement(env, object, op,
							 keyword, rest);
	ExitScope();
	return exp2;
    }

    ErrorMessage("no complete class specification for: ", object, exp);
    return nil;
}

Ptree* ClassWalker::TranslateStaticUserStatement(Ptree* exp)
{
    bool is_type_name;
    TypeInfo type;
    Ptree* qualifier = exp->First();
    Ptree* keyword = exp->Third();
    Ptree* rest = exp->ListTail(3);

    if(env->Lookup(qualifier, is_type_name, type))
	if(is_type_name){
	    Class* metaobject = GetClassMetaobject(type);
	    if(metaobject != nil){
		NewScope();
		if(keyword->IsA(UserKeyword2))		// closure style
		    TranslateArgDeclList2(TRUE, env, FALSE, FALSE, 0,
					  rest->Second());
		Ptree* exp2 = metaobject->TranslateStaticUserStatement(env,
							keyword, rest);
		ExitScope();
		return exp2;
	    }
	}

    ErrorMessage("no complete class specification for: ", qualifier, exp);
    return nil;
}

Ptree* ClassWalker::TranslateNew2(Ptree* exp, Ptree* userkey, Ptree* scope,
				  Ptree* op, Ptree* placement,
				  Ptree* type, Ptree* init)
{
    TypeInfo t;

    if(type->Car()->Eq('('))
	t.Set(type->Second()->Second()->GetEncodedType(), env);
    else
	t.Set(type->Second()->GetEncodedType(), env);

    Class* metaobject = GetClassMetaobject(t);
    if(metaobject != nil){
	if(userkey == nil)
	    userkey = scope;

	Ptree* exp2 = metaobject->TranslateNew(env, userkey, op,
					       placement, type, init);
	return CheckEquiv(exp, exp2);
    }
    else{
	Ptree* placement2 = TranslateArguments(placement);
	Ptree* type2 = TranslateNew3(type);
	Ptree* init2 = TranslateArguments(init);
	if(userkey == nil && placement == placement2
	   && type == type2 && init == init2)
	    return exp;
	else{
	    if(userkey == nil)
		return new PtreeNewExpr(exp->Car(),
				Ptree::ShallowSubst(placement2, placement,
						    type2, type,
						    init2, init, exp->Cdr()));
	    else{
		ErrorMessage("no complete class specification for: ",
			     type, exp);
		exp = exp->Cdr();
		if(placement == placement2 && type == type2 && init == init2)
		    return exp;
		else
		    return new PtreeNewExpr(exp->Car(),
				Ptree::ShallowSubst(placement2, placement,
						    type2, type,
						    init2, init, exp->Cdr()));
	    }

	}
    }
}

Ptree* ClassWalker::TranslateDelete(Ptree* exp)
{
    TypeInfo type;

    Ptree* obj = Ptree::Last(exp)->Car();
    if(exp->Length() == 2){	// not ::delete or delete []
	Typeof(obj, type);
	type.Dereference();
	Class* metaobject = GetClassMetaobject(type);
	if(metaobject != nil){
	    Ptree* exp2 = metaobject->TranslateDelete(env, exp->Car(), obj);
	    return CheckEquiv(exp, exp2);
	}
    }

    Ptree* obj2 = Translate(obj);
    if(obj == obj2)
	return exp;
    else
	return new PtreeDeleteExpr(exp->Car(),
				   Ptree::ShallowSubst(obj2, obj,
						       exp->Cdr()));
}
