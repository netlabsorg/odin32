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

#include <string.h>
#include "env.h"
#include "ptree.h"
#include "walker.h"
#include "typeinfo.h"
#include "mop.h"
#include "metaclass.h"
#include "parse.h"
#include "encoding.h"
#include "member.h"

Parser* Walker::default_parser = nil;
char* Walker::argument_name = "_arg_%d_";
char* Walker::default_metaclass = nil;

Walker::Walker(Parser* p)
{
    env = new Environment(this);
    parser = p;
    if(default_parser == nil)
	default_parser = p;
}

Walker::Walker(Parser* p, Environment* e)
{
    env = new Environment(e, this);
    parser = p;
    if(default_parser == nil)
	default_parser = p;
}

Walker::Walker(Environment* e)
{
    env = new Environment(e, this);
    if(default_parser == nil)
	MopErrorMessage("Walker::Walker()", "no default parser");

    parser = default_parser;
}

Walker::Walker(Walker* w)
{
    env = w->env;
    parser = w->parser;
}

void Walker::NewScope()
{
    env = new Environment(env);
}

void Walker::NewScope(Environment* e)
{
    if (e != nil)
	env = e;
    else
	env = new Environment(env);
}

void Walker::NewScope(Class* metaobject)
{
    env = new Environment(env);
    if(metaobject != nil)
	metaobject->SetEnvironment(env);
}

Environment* Walker::ExitScope()
{
    Environment* old_env = env;
    env = old_env->GetOuterEnvironment();
    return old_env;
}

void Walker::RecordBaseclassEnv(Ptree* bases)
{
    while(bases != nil){
	bases = bases->Cdr();		// skip : or ,
	Ptree* base_class = bases->Car()->Last()->Car();
	Class* metaobject = env->LookupClassMetaobject(base_class);
	if(metaobject != nil){
	    Environment* e = metaobject->GetEnvironment();
	    if(e != nil)
		env->AddBaseclassEnv(e);
	}

	bases = bases->Cdr();
    }
}

Walker::NameScope Walker::ChangeScope(Environment* e)
{
    NameScope scope;
    scope.walker = e->GetWalker();
    e->SetWalker(this);
    scope.env = env;
    env = e;
    return scope;
}

void Walker::RestoreScope(Walker::NameScope& scope)
{
    env->SetWalker(scope.walker);
    env = scope.env;
}

bool Walker::IsClassWalker()
{
    return FALSE;
}

Ptree* Walker::Translate(Ptree* p)
{
    if(p == nil)
	return p;
    else
	return p->Translate(this);
}

void Walker::Typeof(Ptree* p, TypeInfo& t)
{
    if(p != nil)
	p->Typeof(this, t);
}

// default translation

Ptree* Walker::TranslatePtree(Ptree* p)
{
    return p;
}

void Walker::TypeofPtree(Ptree*, TypeInfo& t)
{
    t.Unknown();
}

// translation for each class of node

Ptree* Walker::TranslateTypedef(Ptree* def)
{
    Ptree *tspec, *tspec2;

    tspec = Ptree::Second(def);
    tspec2 = TranslateTypespecifier(tspec);
    env->RecordTypedefName(Ptree::Third(def));
    if(tspec == tspec2)
	return def;
    else
	return new PtreeTypedef(Ptree::First(def),
				Ptree::List(tspec2,
					    Ptree::ListTail(def, 2)));
}

Ptree* Walker::TranslateTemplateDecl(Ptree* def)
{
    Ptree* body = Ptree::Nth(def, 4);
    Ptree* class_spec = GetClassTemplateSpec(body);
    if(class_spec->IsA(ntClassSpec))
	return TranslateTemplateClass(def, class_spec);
    else
	return TranslateTemplateFunction(def, body);
}

Ptree* Walker::TranslateExternTemplate(Ptree* def)
{
    return def;
}

Ptree* Walker::TranslateTemplateClass(Ptree* temp_def, Ptree* class_spec)
{
    Ptree* userkey;
    Ptree* class_def;

    if(class_spec->Car()->IsLeaf()){
	userkey = nil;
	class_def = class_spec;
    }
    else{
	userkey = class_spec->Car();
	class_def = class_spec->Cdr();
    }

    Class* metaobject = nil;
    if(Ptree::Length(class_def) == 4)
	metaobject = MakeTemplateClassMetaobject(temp_def, userkey, class_def);

    env->RecordTemplateClass(class_spec, metaobject);
    Ptree* class_spec2 = TranslateClassSpec(class_spec, userkey, class_def,
					    metaobject);
    if(class_spec == class_spec2)
	return temp_def;
    else
	return new PtreeTemplateDecl(temp_def->Car(),
				     Ptree::Subst(class_spec2, class_spec,
						  temp_def->Cdr()));
}

Class* Walker::MakeTemplateClassMetaobject(Ptree* def, Ptree* userkey,
					   Ptree* class_def)
{
    Class* metaobject = LookupMetaclass(def, userkey, class_def, TRUE);
    if(metaobject == nil)
	metaobject = new TemplateClass;
    else
	if(metaobject->AcceptTemplate())
	    return metaobject;
	else{

	    ErrorMessage("the specified metaclass is not for templates.",
			 nil, def);
	    metaobject = new TemplateClass;
	}

    metaobject->InitializeInstance(def, nil);
    return metaobject;
}

Ptree* Walker::TranslateTemplateFunction(Ptree* temp_def, Ptree* fun)
{
    env->RecordTemplateFunction(temp_def, fun);
    return temp_def;
}

Ptree* Walker::TranslateTemplateInstantiation(Ptree *inst_spec)
{
    Ptree* userkey;
    Ptree* class_spec;
    Ptree* full_class_spec = Ptree::First(inst_spec);

    if(full_class_spec->Car()->IsLeaf()){
	userkey = nil;
	class_spec = full_class_spec;
    }
    else{
	userkey = full_class_spec->Car();
	class_spec = full_class_spec->Cdr();
    }

    Class* metaobject = nil;
    metaobject = MakeTemplateInstantiationMetaobject(full_class_spec,
						     userkey, class_spec);
    return TranslateTemplateInstantiation(inst_spec, userkey, 
					  class_spec, metaobject);
}

Class* Walker::MakeTemplateInstantiationMetaobject(
    Ptree* full_class_spec, Ptree* userkey, Ptree* class_spec)
{
    // [class [foo [< ... >]]] -> [class foo]
    Ptree* class_name = Ptree::First(Ptree::Second(class_spec));
    Bind* binding = nil;
    if (!env->Lookup(class_name,binding))
	return nil;

    Class* metaobject = nil;
    if (binding->What() != Bind::isTemplateClass) {
	ErrorMessage("not declarated as a template class?!?",
		     nil, full_class_spec);
	metaobject = nil;
    }
    else
	metaobject = binding->ClassMetaobject();

    if (metaobject == nil)
	metaobject = new TemplateClass;
    else
	if(metaobject->AcceptTemplate())
	    return metaobject;
	else{
	    ErrorMessage("the specified metaclass is not for templates.",
			 nil, full_class_spec);
	    metaobject = new TemplateClass;
	}

    return metaobject;
}

Ptree* Walker::TranslateTemplateInstantiation(Ptree* inst_spec,
		Ptree* userkey, Ptree* class_spec, Class* metaobject)
{
    if (metaobject == nil)
	return inst_spec;
    else {
	Ptree *class_spec2 = TranslateClassSpec(class_spec);
	if (class_spec == class_spec2)
	    return inst_spec;
	else
	    return class_spec2;
    }
}

Ptree* Walker::TranslateMetaclassDecl(Ptree* decl)
{
    env->RecordMetaclassName(decl);
    return decl;
}

Ptree* Walker::TranslateLinkageSpec(Ptree* def)
{
    Ptree* body = Ptree::Third(def);
    Ptree* body2 = Translate(body);
    if(body == body2)
	return def;
    else
	return new PtreeLinkageSpec(Ptree::First(def),
				    Ptree::List(Ptree::Second(def),
						body2));
}

Ptree* Walker::TranslateNamespaceSpec(Ptree* def)
{
    Ptree* name = Ptree::Second(def);
    Ptree* body = Ptree::Third(def);

    Environment* name_space = env->LookupNamespace0(name);
    NewScope(name_space);
    Ptree* body2 = Translate(body);
    Environment* name_space2 = ExitScope();

    if (name_space == nil)	// if this is the first time
	env->RecordNamespace(name_space2, name);

    if(body == body2)
	return def;
    else
	return new PtreeNamespaceSpec(Ptree::First(def),
				      Ptree::List(name, body2));
}

Ptree* Walker::TranslateUsing(Ptree* def)
{
    if (((PtreeUsing*)def)->isNamespace())
	env->RecordUsingNamespace(def);
    else
	env->RecordUsing(def);

    return def;
}

Ptree* Walker::TranslateDeclaration(Ptree* def)
{
    Ptree* decls = Ptree::Third(def);
    if(decls->IsA(ntDeclarator))	// if it is a function
	return TranslateFunctionImplementation(def);
    else{
	// if it is a function prototype or a variable declaration.
	Ptree* decls2;
	Ptree* sspec = Ptree::First(def);
	Ptree* sspec2 = TranslateStorageSpecifiers(sspec);
	Ptree* tspec = Ptree::Second(def);
	Ptree* tspec2 = TranslateTypespecifier(tspec);
	if(decls->IsLeaf())	// if it is ";"
	    decls2 = decls;
	else
	    decls2 = TranslateDeclarators(decls);

	if(sspec == sspec2 && tspec == tspec2 && decls == decls2)
	    return def;
	else if(decls2 == nil)
	    return new PtreeDeclaration(nil, Ptree::List(nil,
							 Class::semicolon_t));
	else
	    return new PtreeDeclaration(sspec2,
					Ptree::ShallowSubst(tspec2, tspec,
							    decls2, decls,
							    def->Cdr()));
    }
}

// TranslateStorageSpecifiers() also deals with inline, virtual, etc.

Ptree* Walker::TranslateStorageSpecifiers(Ptree* spec)
{
    return spec;
}

Ptree* Walker::TranslateDeclarators(Ptree* decls)
{
    return TranslateDeclarators(decls, TRUE);
}

Ptree* Walker::TranslateDeclarators(Ptree* decls, bool record)
{
    PtreeArray array;
    bool changed = FALSE;
    Ptree* rest = decls;
    while(rest != nil){
	Ptree *p, *q;
	int len;
	p = q = rest->Car();
	if(p->IsA(ntDeclarator)){
	    Ptree *exp, *exp2;

	    if(record)
		env->RecordDeclarator(p);

	    len = p->Length();
	    exp = exp2 = nil;
	    if(len >= 2 && p->Nth(len - 2)->Eq('=')){
		exp = p->ListTail(len - 2);
		exp2 = TranslateAssignInitializer((PtreeDeclarator*)p, exp);
	    }
	    else{
		Ptree* last = p->Last()->Car();
		if(last != nil && !last->IsLeaf() && last->Car()->Eq('(')){
		    exp = last;
		    exp2 = TranslateInitializeArgs((PtreeDeclarator*)p, last);
		}
	    }

	    q = TranslateDeclarator(FALSE, (PtreeDeclarator*)p);
	    if(exp != exp2){
		// exp2 should be a list, but...
		if(exp2 != nil && exp2->IsLeaf())
		    exp2 = Ptree::List(exp2);

		if(p == q){
		    q = Ptree::SubstSublist(exp2, exp, p->Cdr());
		    q = new PtreeDeclarator((PtreeDeclarator*)p, p->Car(), q);
		}
		else if(q != nil && !q->IsLeaf())
		    q = new PtreeDeclarator((PtreeDeclarator*)p, q->Car(),
					    Ptree::Subst(exp2, exp, q->Cdr()));
	    }
	}

	if(q == nil){
	    changed = TRUE;
	    rest = rest->Cdr();
	    if(rest != nil)
		rest = rest->Cdr();
	}
	else{
	    array.Append(q);
	    if(p != q)
		changed = TRUE;

	    rest = rest->Cdr();
	    if(rest != nil){
		array.Append(rest->Car());
		rest = rest->Cdr();
	    }
	}
    }

    if(changed)
	return array.All();
    else
	return decls;
}

Ptree* Walker::TranslateDeclarator(bool record, PtreeDeclarator* decl)
{
    // if record is true, the formal arguments are recorded in the
    // current environment.

    Ptree* args;
    if(GetArgDeclList(decl, args)){
	Ptree* args2 = TranslateArgDeclList(record, decl, args);
	if(args == args2)
	    return decl;
	else
	    return new PtreeDeclarator(decl, decl->Car(),
				       Ptree::Subst(args2, args,
						    decl->Cdr()));
    }
    else
	return decl;
}

bool Walker::GetArgDeclList(PtreeDeclarator* decl, Ptree*& args)
{
    Ptree* p = decl;
    while(p != nil){
	Ptree* q = p->Car();
	if(q != nil)
	    if(q->IsLeaf()){
		if(q->Eq('(')){
		    args = p->Cadr();
		    return TRUE;
		}
	    }
	    else if(q->Car()->Eq('('))	// e.g. int (*p)[];
		p = q->Cadr();

	p = p->Cdr();
    }

    args = nil;
    return FALSE;
}

Ptree* Walker::TranslateArgDeclList(bool record, Ptree*, Ptree* args)
{
    return TranslateArgDeclList2(record, env, FALSE, FALSE, 0, args);
}

// If translate is TRUE, this function eliminates a user-defined keyword.

Ptree* Walker::TranslateArgDeclList2(bool record, Environment* e,
				     bool translate,
				     bool fill_args, int arg_name,
				     Ptree* args)
{
    Ptree* rest;
    Ptree* rest2;

    if(args == nil)
	return args;
    else{
	Ptree *a, *a2;
	a = a2 = args->Car();
	if(args->Cdr() == nil)
	    rest = rest2 = nil;
	else{
	    rest = args->Cddr();	// skip ","
	    rest2 = TranslateArgDeclList2(record, e, translate, fill_args,
					  arg_name + 1, rest);
	    if(rest == rest2)
		rest = rest2 = args->Cdr();
	    else
		rest2 = Ptree::Cons(args->Cadr(), rest2);
	}

	bool is_ellipsis = a->IsLeaf();		// a may be "..."
	if(is_ellipsis)
	    /* do nothing */;
	else if(a->Car()->IsA(ntUserdefKeyword)){
	    if(record)
		e->RecordDeclarator(a->Third());

	    if(translate){
		a2 = a->Cdr();
		if(fill_args)
		    a2 = FillArgumentName(a2, a2->Second(), arg_name);
	    }
	}
	else if(a->Car()->IsA(REGISTER)){
	    if(record)
		e->RecordDeclarator(a->Third());

	    if(translate && fill_args){
		a2 = FillArgumentName(a, a->Third(), arg_name);
		if(a != a2)
		    a2 = Ptree::Cons(a->First(), a2);
	    }
	}
	else{
	    if(record)
		e->RecordDeclarator(a->Second());

	    if(translate && fill_args)
		a2 = FillArgumentName(a, a->Second(), arg_name);
	}

	if(a != a2 || rest != rest2)
	    return Ptree::Cons(a2, rest2);
	else
	    return args;
    }
}

Ptree* Walker::FillArgumentName(Ptree* arg, Ptree* d, int arg_name)
{
    PtreeDeclarator* decl = (PtreeDeclarator*)d;
    if(decl->Name() != nil)
	return arg;
    else{
	unsigned char* type = (unsigned char*)decl->GetEncodedType();
	return Encoding::MakePtree(type,
				   Ptree::Make(argument_name, arg_name));
    }
}

Ptree* Walker::TranslateAssignInitializer(PtreeDeclarator*, Ptree* init)
{
    Ptree* exp = init->Second();
    Ptree* exp2 = Translate(exp);
    if(exp == exp2)
	return init;
    else
	return Ptree::List(init->Car(), exp2);
}

Ptree* Walker::TranslateInitializeArgs(PtreeDeclarator*, Ptree* init)
{
    return TranslateArguments(init);
}

Ptree* Walker::TranslateFunctionImplementation(Ptree* impl)
{
    Ptree* sspec = Ptree::First(impl);
    Ptree* sspec2 = TranslateStorageSpecifiers(sspec);
    Ptree* tspec = Ptree::Second(impl);
    Ptree* decl = Ptree::Third(impl);
    Ptree* body = Ptree::Nth(impl, 3);
    Ptree* decl2;
    Ptree* body2;

    Ptree* tspec2 = TranslateTypespecifier(tspec);
    Environment* fenv = env->RecordDeclarator(decl);
    if(fenv == nil){
	// reach here if resolving the qualified name fails. error?
	NewScope();
	decl2 = TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	body2 = Translate(body);
	ExitScope();
    }
    else{
	NameScope old_env = ChangeScope(fenv);
	NewScope();
	decl2 = TranslateDeclarator(TRUE, (PtreeDeclarator*)decl);
	body2 = Translate(body);
	ExitScope();
	RestoreScope(old_env);
    }

    if(sspec == sspec2 && tspec == tspec2 && decl == decl2 && body == body2)
	return impl;
    else
	return new PtreeDeclaration(sspec2,
				    Ptree::List(tspec2, decl2, body2));
}

Ptree* Walker::RecordArgsAndTranslateFbody(Class*, Ptree* args, Ptree* body)
{
    NewScope();
    TranslateArgDeclList2(TRUE, env, FALSE, FALSE, 0, args);
    Ptree* body2 = TranslateFunctionBody(body);
    ExitScope();
    return body2;
}

Ptree* Walker::TranslateFunctionBody(Ptree* body)
{
    return Translate(body);
}

Ptree* Walker::TranslateBrace(Ptree* block)
{
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

    if(changed)
	return new PtreeBrace(Ptree::First(block), array.All(),
			      Ptree::Third(block));
    else
	return block;
}

Ptree* Walker::TranslateBlock(Ptree* block)
{
    Ptree* block2;

    NewScope();

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

    if(changed)
	block2 = new PtreeBlock(Ptree::First(block), array.All(),
				Ptree::Third(block));
    else
	block2 = block;

    ExitScope();
    return block2;
}

Ptree* Walker::TranslateClassBody(Ptree* block, Ptree* bases,
				  Class* metaobject)
{
    Ptree* block2;

    NewScope(metaobject);
    RecordBaseclassEnv(bases);

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

    if(changed)
	block2 = new PtreeClassBody(Ptree::First(block), array.All(),
				    Ptree::Third(block));
    else
	block2 = block;

    ExitScope();
    return block2;
}

Ptree* Walker::TranslateClassSpec(Ptree* spec)
{
    Ptree* userkey;
    Ptree* class_def;

    if(spec->Car()->IsLeaf()){
	userkey = nil;
	class_def = spec;
    }
    else{
	userkey = spec->Car();
	class_def = spec->Cdr();
    }

    Class* metaobject = nil;
    if(Ptree::Length(class_def) == 4)
	metaobject = MakeClassMetaobject(spec, userkey, class_def);

    env->RecordClassName(spec->GetEncodedName(), metaobject);
    return TranslateClassSpec(spec, userkey, class_def, metaobject);
}

Class* Walker::MakeClassMetaobject(Ptree* def, Ptree* userkey,
				   Ptree* class_def)
{
    Class* metaobject = LookupMetaclass(def, userkey, class_def, FALSE);
    if(metaobject == nil && default_metaclass != nil){
	metaobject = opcxx_ListOfMetaclass::New(default_metaclass, class_def,
						nil);
	if(metaobject == nil)
	    MopErrorMessage2("the default metaclass cannot be loaded: ",
			     default_metaclass);
    }

    if(metaobject == nil)
	metaobject = new Class;
    else{
	if(!metaobject->AcceptTemplate())
	    return metaobject;
	else{
	    ErrorMessage("the specified metaclass is for templates.",
			 nil, def);
	    metaobject = new Class;
	}
    }

    metaobject->InitializeInstance(class_def, nil);
    return metaobject;
}

void Walker::ChangeDefaultMetaclass(char* name)
{
    default_metaclass = name;
}

// LookupMetaclass() returns nil if no metaclass is found.

Class* Walker::LookupMetaclass(Ptree* def, Ptree* userkey, Ptree* class_def,
			       bool is_template)
{
    Ptree *mclass, *margs;
    Class* metaobject;

    Ptree* class_name = class_def->Second();

    // for bootstrapping
    if(Metaclass::IsBuiltinMetaclass(class_name)){
	metaobject = new Metaclass;
	metaobject->InitializeInstance(def, nil);
	return metaobject;
    }

    Ptree* mdecl = env->LookupMetaclass(class_name);
    if(mdecl != nil){
	mclass = mdecl->Second();
	margs = mdecl->Nth(4);
	metaobject = opcxx_ListOfMetaclass::New(mclass, def, margs);
	if(metaobject == nil)
	    ErrorMessage("the metaclass is not loaded: ", mclass, class_def);
	else if(userkey != nil)
	    ErrorMessage("the metaclass declaration conflicts"
			 " with the keyword: ", mclass, class_def);

	return metaobject;
    }

    if(userkey != nil){
	mclass = env->LookupClasskeyword(userkey->Car());
	if(mclass == nil)
	    ErrorMessage("invalid keyword: ", userkey, class_def);
	else{
	    metaobject = opcxx_ListOfMetaclass::New(mclass, class_def,
						    userkey->Third());
	    if(metaobject == nil)
		ErrorMessage("the metaclass associated with the"
			     " keyword is not loaded: ", userkey, class_def);

	    return metaobject;
	}
    }

    return LookupBaseMetaclass(def, class_def, is_template);
}

Class* Walker::LookupBaseMetaclass(Ptree* def, Ptree* class_def,
				   bool is_template)
{
    Class* metaobject = nil;
    Ptree* bases = class_def->Third();
    while(bases != nil){
	bases = bases->Cdr();
	Ptree* base = bases->Car()->Last()->Car();
	bases = bases->Cdr();
	Class* m = env->LookupClassMetaobject(base);
	if(m != nil){
	    if(metaobject == nil)
		metaobject = m;
	    else if(m == nil || strcmp(metaobject->MetaclassName(),
				       m->MetaclassName()) != 0){
		ErrorMessage("inherited metaclasses conflict: ",
			     class_def->Second(), class_def);
		return nil;
	    }
	}
    }

    if(metaobject == nil)
	return nil;

    bool accept_template = metaobject->AcceptTemplate();
    if((is_template && accept_template) || (!is_template && !accept_template))
	return opcxx_ListOfMetaclass::New(metaobject->MetaclassName(),
					  def, nil);
    else
	return nil;
}

Ptree* Walker::TranslateClassSpec(Ptree* spec, Ptree*,
				  Ptree* class_def, Class* metaobject)
{
    if(metaobject == nil)
	return spec;
    else{
	// a class body is specified.
	Ptree* body = class_def->Nth(3);
	Ptree* body2 = TranslateClassBody(body, class_def->Third(),
					  metaobject);
	if(body == body2)
	    return spec;
	else
	    return new PtreeClassSpec(spec->Car(),
				      Ptree::ShallowSubst(body2, body,
							  spec->Cdr()),
				      nil, spec->GetEncodedName());
    }
}


Ptree* Walker::TranslateEnumSpec(Ptree* spec)
{
    env->RecordEnumName(spec);
    return spec;
}

Ptree* Walker::TranslateAccessSpec(Ptree* p)
{
    return p;
}

Ptree* Walker::TranslateAccessDecl(Ptree* p)
{
    return p;
}

Ptree* Walker::TranslateUserAccessSpec(Ptree* p)
{
    return p;
}

Ptree* Walker::TranslateIf(Ptree* s)
{
    Ptree* cond = s->Third();
    Ptree* cond2 = Translate(cond);
    Ptree* then_part = s->Nth(4);
    Ptree* then_part2 = Translate(then_part);
    Ptree* else_part = s->Nth(6);
    Ptree* else_part2 = Translate(else_part);

    if(cond == cond2 && then_part == then_part2 && else_part == else_part2)
	return s;
    else{
	Ptree* rest = Ptree::ShallowSubst(cond2, cond, then_part2, then_part,
					  else_part2, else_part, s->Cdr());
	return new PtreeIfStatement(s->Car(), rest);
    }
}

Ptree* Walker::TranslateSwitch(Ptree* s)
{
    Ptree* cond = s->Third();
    Ptree* cond2 = Translate(cond);
    Ptree* body = s->Nth(4);
    Ptree* body2 = Translate(body);
    if(cond == cond2 && body == body2)
	return s;
    else{
	Ptree* rest = Ptree::ShallowSubst(cond2, cond, body2, body, s->Cdr());
	return new PtreeSwitchStatement(s->Car(), rest);
    }
}

Ptree* Walker::TranslateWhile(Ptree* s)
{
    Ptree* cond = s->Third();
    Ptree* cond2 = Translate(cond);
    Ptree* body = s->Nth(4);
    Ptree* body2 = Translate(body);
    if(cond == cond2 && body == body2)
	return s;
    else{
	Ptree* rest = Ptree::ShallowSubst(cond2, cond, body2, body, s->Cdr());
	return new PtreeWhileStatement(s->Car(), rest);
    }
}

Ptree* Walker::TranslateDo(Ptree* s)
{
    Ptree* body = s->Second();
    Ptree* body2 = Translate(body);
    Ptree* cond = s->Nth(4);
    Ptree* cond2 = Translate(cond);
    if(cond == cond2 && body == body2)
	return s;
    else{
	Ptree* rest = Ptree::ShallowSubst(body2, body, cond2, cond, s->Cdr());
	return new PtreeDoStatement(s->Car(), rest);
    }
}

Ptree* Walker::TranslateFor(Ptree* s)
{
    NewScope();
    Ptree* exp1 = s->Third();
    Ptree* exp1t = Translate(exp1);
    Ptree* exp2 = s->Nth(3);
    Ptree* exp2t = Translate(exp2);
    Ptree* exp3 = s->Nth(5);
    Ptree* exp3t = Translate(exp3);
    Ptree* body = s->Nth(7);
    Ptree* body2 = Translate(body);
    ExitScope();

    if(exp1 == exp1t && exp2 == exp2t && exp3 == exp3t && body == body2)
	return s;
    else{
	Ptree* rest = Ptree::ShallowSubst(exp1t, exp1, exp2t, exp2,
					  exp3t, exp3, body2, body, s->Cdr());
	return new PtreeForStatement(s->Car(), rest);
    }
}

Ptree* Walker::TranslateTry(Ptree* s)
{
    Ptree* try_block = s->Second();
    Ptree* try_block2 = Translate(try_block);

    PtreeArray array;
    Ptree* handlers = s->Cddr();
    bool changed = FALSE;

    while(handlers != nil){
	Ptree* handle = handlers->Car();
	Ptree* body = handle->Nth(4);
	Ptree* body2 = Translate(body);
	if(body == body2)
	    array.Append(handle);
	else{
	    array.Append(Ptree::ShallowSubst(body2, body, handle));
	    changed = TRUE;
	}

	handlers = handlers->Cdr();
    }

    if(try_block == try_block2 && !changed)
	return s;
    else
	return new PtreeTryStatement(s->Car(),
				     Ptree::Cons(try_block2, array.All()));
}

Ptree* Walker::TranslateBreak(Ptree* s)
{
    return s;
}

Ptree* Walker::TranslateContinue(Ptree* s)
{
    return s;
}

Ptree* Walker::TranslateReturn(Ptree* s)
{
    if(s->Length() == 2)
	return s;
    else{
	Ptree* exp = s->Second();
	Ptree* exp2 = Translate(exp);
	if(exp == exp2)
	    return s;
	else
	    return new PtreeReturnStatement(s->Car(),
					    Ptree::ShallowSubst(exp2, exp,
								s->Cdr()));
    }
}

Ptree* Walker::TranslateGoto(Ptree* s)
{
    return s;
}

Ptree* Walker::TranslateCase(Ptree* s)
{
    Ptree* st = s->Nth(3);
    Ptree* st2 = Translate(st);
    if(st == st2)
	return s;
    else
	return new PtreeCaseStatement(s->Car(),
				      Ptree::ShallowSubst(st2, st, s->Cdr()));
}

Ptree* Walker::TranslateDefault(Ptree* s)
{
    Ptree* st = s->Third();
    Ptree* st2 = Translate(st);
    if(st == st2)
	return s;
    else
	return new PtreeDefaultStatement(s->Car(),
					 Ptree::List(s->Cadr(), st2));
}

Ptree* Walker::TranslateLabel(Ptree* s)
{
    Ptree* st = s->Third();
    Ptree* st2 = Translate(st);
    if(st == st2)
	return s;
    else
	return new PtreeLabelStatement(s->Car(),
				       Ptree::List(s->Cadr(), st2));
}

Ptree* Walker::TranslateExprStatement(Ptree* s)
{
    Ptree* exp = s->First();
    Ptree* exp2 = Translate(exp);
    if(exp == exp2)
	return s;
    else
	return new PtreeExprStatement(exp2, s->Cdr());
}

Ptree* Walker::TranslateTypespecifier(Ptree* tspec)
{
    Ptree *class_spec, *class_spec2;

    class_spec = GetClassOrEnumSpec(tspec);
    if(class_spec == nil)
	class_spec2 = nil;
    else
	class_spec2 = Translate(class_spec);

    if(class_spec == class_spec2)
	return tspec;
    else
	return Ptree::ShallowSubst(class_spec2, class_spec, tspec);
}

Ptree* Walker::GetClassOrEnumSpec(Ptree* typespec)
{
    Ptree* spec = StripCvFromIntegralType(typespec);
    if(spec->IsA(ntClassSpec, ntEnumSpec))
	return spec;

    return nil;
}

Ptree* Walker::GetClassTemplateSpec(Ptree* body)
{
    if(Ptree::Eq(Ptree::Third(body), ';')){
	Ptree* spec = StripCvFromIntegralType(Ptree::Second(body));
	if(spec->IsA(ntClassSpec))
	    return spec;
    }

    return nil;
}

Ptree* Walker::StripCvFromIntegralType(Ptree* integral)
{
    if(integral == nil)
	return nil;

    if(!integral->IsLeaf())
	if(integral->Car()->IsA(CONST, VOLATILE))
	    return Ptree::Second(integral);
	else if(Ptree::Second(integral)->IsA(CONST, VOLATILE))
	    return integral->Car();

    return integral;
}

Ptree* Walker::TranslateComma(Ptree* exp)
{
    Ptree* left = exp->First();
    Ptree* left2 = Translate(left);
    Ptree* right = exp->Third();
    Ptree* right2 = Translate(right);
    if(left == left2 && right == right2)
	return exp;
    else
	return new PtreeCommaExpr(left2, Ptree::List(exp->Second(), right2));
}

void Walker::TypeofComma(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Third(), t);
}

Ptree* Walker::TranslateAssign(Ptree* exp)
{
    Ptree* left = exp->First();
    Ptree* left2 = Translate(left);
    Ptree* right = exp->Third();
    Ptree* right2 = Translate(right);
    if(left == left2 && right == right2)
	return exp;
    else
	return new PtreeAssignExpr(left2, Ptree::List(exp->Second(), right2));
}

void Walker::TypeofAssign(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->First(), t);
}

Ptree* Walker::TranslateCond(Ptree* exp)
{
    Ptree* c = exp->First();
    Ptree* c2 = Translate(c);
    Ptree* t = exp->Third();
    Ptree* t2 = Translate(t);
    Ptree* e = exp->Nth(4);
    Ptree* e2 = Translate(e);
    if(c == c2 && t == t2 && e == e2)
	return exp;
    else
	return new PtreeCondExpr(c2, Ptree::List(exp->Second(), t2,
						 exp->Nth(3), e2));
}

void Walker::TypeofCond(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Third(), t);
}

Ptree* Walker::TranslateInfix(Ptree* exp)
{
    Ptree* left = exp->First();
    Ptree* left2 = Translate(left);
    Ptree* right = exp->Third();
    Ptree* right2 = Translate(right);
    if(left == left2 && right == right2)
	return exp;
    else
	return new PtreeInfixExpr(left2, Ptree::List(exp->Second(), right2));
}

void Walker::TypeofInfix(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->First(), t);
}

Ptree* Walker::TranslatePm(Ptree* exp)
{
    Ptree* left = exp->First();
    Ptree* left2 = Translate(left);
    Ptree* right = exp->Third();
    Ptree* right2 = Translate(right);
    if(left == left2 && right == right2)
	return exp;
    else
	return new PtreePmExpr(left2, Ptree::List(exp->Second(), right2));
}

void Walker::TypeofPm(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Third(), t);
    t.Dereference();
}

Ptree* Walker::TranslateCast(Ptree* exp)
{
    Ptree* e = exp->Nth(3);
    Ptree* e2 = Translate(e);
    if(e == e2)
	return exp;
    else
	return new PtreeCastExpr(exp->First(),
				 Ptree::ShallowSubst(e2, e, exp->Cdr()));
}

void Walker::TypeofCast(Ptree* exp, TypeInfo& t)
{
    t.Set(exp->Second()->Second()->GetEncodedType(), env);
}

Ptree* Walker::TranslateUnary(Ptree* exp)
{
    Ptree* oprnd = exp->Second();
    Ptree* oprnd2 = Translate(oprnd);
    if(oprnd == oprnd2)
	return exp;
    else
	return new PtreeUnaryExpr(exp->First(), Ptree::List(oprnd2));
}

void Walker::TypeofUnary(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Second(), t);

    Ptree* op = exp->First();
    if(op->Eq('*'))
	t.Dereference();
    else if(op->Eq('&'))
	t.Reference();
}

Ptree* Walker::TranslateThrow(Ptree* exp)
{
    Ptree* oprnd = exp->Second();
    Ptree* oprnd2 = Translate(oprnd);
    if(oprnd == oprnd2)
	return exp;
    else
	return new PtreeThrowExpr(exp->First(), Ptree::List(oprnd2));
}

void Walker::TypeofThrow(Ptree*, TypeInfo& t)
{
    t.SetVoid();
}

Ptree* Walker::TranslateSizeof(Ptree* exp)
{
    Ptree* e = exp->Second();
    if(e->Eq('('))
	e = exp->Third();

    Ptree* e2 = Translate(e);
    if(e == e2)
	return exp;
    else
	return new PtreeSizeofExpr(exp->First(),
				   Ptree::ShallowSubst(e2, e, exp->Cdr()));
}

void Walker::TypeofSizeof(Ptree*, TypeInfo& t)
{
    t.SetInt();
}

Ptree* Walker::TranslateNew(Ptree* exp)
{
    Ptree *p;
    Ptree *userkey, *scope, *op, *placement, *type, *init;

    p = exp;
    userkey = p->Car();
    if(userkey == nil || !userkey->IsLeaf())
	p = exp->Cdr();		// user keyword
    else
	userkey = nil;

    if(p->Car()->Eq("::")){
	scope = p->Car();
	p = p->Cdr();
    }
    else
	scope = nil;

    op = p->Car();
    placement = p->Cadr();
    type = p->Third();
    init = p->Nth(3);
    return TranslateNew2(exp, userkey, scope, op, placement, type, init);
}

Ptree* Walker::TranslateNew2(Ptree* exp, Ptree*, Ptree*,
			     Ptree*, Ptree* placement,
			     Ptree* type, Ptree* init)
{
    Ptree* placement2 = TranslateArguments(placement);
    Ptree* type2 = TranslateNew3(type);
    Ptree* init2 = TranslateArguments(init);
    if(placement == placement2 && init == init2)
	return exp;
    else
	return new PtreeNewExpr(exp->Car(),
				Ptree::ShallowSubst(placement2, placement,
						    type2, type,
						    init2, init,
						    exp->Cdr()));
}

Ptree* Walker::TranslateNew3(Ptree* type)
{
    Ptree* p = type;
    if(p->Car()->Eq('('))
	p = p->Second();

    Ptree* decl = p->Second();
    Ptree* decl2 = TranslateNewDeclarator(decl);
    if(decl == decl2)
	return type;
    else
	return Ptree::Subst(decl2, decl, type);
}

void Walker::TypeofNew(Ptree* exp, TypeInfo& t)
{
    Ptree *p, *userkey, *type;

    p = exp;
    userkey = p->Car();
    if(userkey == nil || !userkey->IsLeaf())
	p = exp->Cdr();		// user keyword

    if(p->Car()->Eq("::"))
	p = p->Cdr();

    type = p->Third();

    if(type->Car()->Eq('('))
	t.Set(type->Second()->Second()->GetEncodedType(), env);
    else
	t.Set(type->Second()->GetEncodedType(), env);

    t.Reference();
}

Ptree* Walker::TranslateDelete(Ptree* exp)
{
    Ptree* obj = Ptree::Last(exp)->Car();
    Ptree* obj2 = Translate(obj);
    if(obj == obj2)
	return exp;
    else
	return new PtreeDeleteExpr(exp->Car(),
				   Ptree::ShallowSubst(obj2, obj,
						       exp->Cdr()));
}

void Walker::TypeofDelete(Ptree*, TypeInfo& t)
{
    t.SetVoid();
}

Ptree* Walker::TranslateThis(Ptree* exp)
{
    return exp;
}

void Walker::TypeofThis(Ptree*, TypeInfo& t)
{
    t.Set(env->LookupThis());
}

Ptree* Walker::TranslateVariable(Ptree* exp)
{
    return exp;
}

/*
  This may be a class name if the expression is a function-style cast.
*/
void Walker::TypeofVariable(Ptree* exp, TypeInfo& t)
{
    bool is_type_name;

    if(env->Lookup(exp, is_type_name, t))
	if(is_type_name)	// if exp is a class name
	    t.Reference();	// see TypeofFuncall
}

/*
  TranslateFstyleCast() deals with function-style casts
  to an integral type
*/
Ptree* Walker::TranslateFstyleCast(Ptree* exp)
{
    Ptree* args = exp->Cdr();
    Ptree* args2 = TranslateArguments(args);
    if(args == args2)
	return exp;
    else
	return new PtreeFstyleCastExpr(exp->GetEncodedType(), exp->Car(),
				       args2);
}

void Walker::TypeofFstyleCast(Ptree* exp, TypeInfo& t)
{
    t.Set(exp->GetEncodedType(), env);
}

Ptree* Walker::TranslateArray(Ptree* exp)
{
    Ptree* array = exp->Car();
    Ptree* array2 = Translate(array);
    Ptree* index = exp->Third();
    Ptree* index2 = Translate(index);
    if(array == array2 && index == index2)
	return exp;
    else
	return new PtreeArrayExpr(array2, Ptree::Subst(index2, index,
						       exp->Cdr()));
}

void Walker::TypeofArray(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Car(), t);
    t.Dereference();
}

/*
  TranslateFuncall() also deals with function-style casts to a class.
*/
Ptree* Walker::TranslateFuncall(Ptree* exp)
{
    Ptree* fun = exp->Car();
    Ptree* fun2 = Translate(fun);
    Ptree* args = exp->Cdr();
    Ptree* args2 = TranslateArguments(args);
    if(fun == fun2 && args == args2)
	return exp;
    else
	return new PtreeFuncallExpr(fun2, args2);
}

void Walker::TypeofFuncall(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Car(), t);
    if(!t.IsFunction())
	t.Dereference();	// maybe a pointer to a function

    t.Dereference();
}

Ptree* Walker::TranslatePostfix(Ptree* exp)
{
    Ptree* left = exp->Car();
    Ptree* left2 = Translate(left);
    if(left == left2)
	return exp;
    else
	return new PtreePostfixExpr(left2, exp->Cdr());
}

void Walker::TypeofPostfix(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Car(), t);
}

Ptree* Walker::TranslateUserStatement(Ptree* exp)
{
    return exp;
}

void Walker::TypeofUserStatement(Ptree*, TypeInfo& t)
{
    t.Unknown();
}

Ptree* Walker::TranslateDotMember(Ptree* exp)
{
    Ptree* left = exp->Car();
    Ptree* left2 = Translate(left);
    if(left == left2)
	return exp;
    else
	return new PtreeDotMemberExpr(left2, exp->Cdr());
}

void Walker::TypeofDotMember(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Car(), t);
    t.SetMember(exp->Third());
}

Ptree* Walker::TranslateArrowMember(Ptree* exp)
{
    Ptree* left = exp->Car();
    Ptree* left2 = Translate(left);
    if(left == left2)
	return exp;
    else
	return new PtreeArrowMemberExpr(left2, exp->Cdr());
}

void Walker::TypeofArrowMember(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Car(), t);
    t.Dereference();
    t.SetMember(exp->Third());
}

Ptree* Walker::TranslateParen(Ptree* exp)
{
    Ptree* e = exp->Second();
    Ptree* e2 = Translate(e);
    if(e == e2)
	return exp;
    else
	return new PtreeParenExpr(exp->Car(), Ptree::List(e2, exp->Third()));
}

void Walker::TypeofParen(Ptree* exp, TypeInfo& t)
{
    Typeof(exp->Second(), t);
}

Ptree* Walker::TranslateStaticUserStatement(Ptree* exp)
{
    return exp;
}

void Walker::TypeofStaticUserStatement(Ptree*, TypeInfo& t)
{
    t.Unknown();
}

Ptree* Walker::TranslateNewDeclarator(Ptree* decl)
{
    Ptree* decl2 = decl;
    Ptree* p = decl;
    while(p != nil){
	Ptree* head = p->Car();
	if(head == nil)
	    return decl;
	else if(head->Eq('[')){
	    Ptree* p2 = TranslateNewDeclarator2(p);
	    if(p == p2)
		return decl;
	    else{
		decl2 = Ptree::ShallowSubst(p2, p, decl);
		break;
	    }
	}
	else if(!head->IsLeaf() && head->Car()->Eq('(')){
	    Ptree* d = head->Cadr();
	    Ptree* d2 = TranslateNewDeclarator(d);
	    decl2 = Ptree::ShallowSubst(d2, d, decl);
	    break;
	}

	p = p->Cdr();
    }

    if(p == nil)
	return decl;
    else if(decl->IsA(ntDeclarator))
	return new PtreeDeclarator((PtreeDeclarator*)decl,
				   decl2->Car(), decl2->Cdr());
    else
	return decl2;
}

Ptree* Walker::TranslateNewDeclarator2(Ptree* decl)
{
    for(Ptree* p = decl; p != nil; p = p->Cdr()){
	Ptree* head = p->Car();
	if(head->Eq('[')){
	    Ptree* size = p->Cadr();
	    Ptree* size2 = Translate(size);
	    if(size != size2){
		Ptree* q = TranslateNewDeclarator2(Ptree::ListTail(p, 3));
		return Ptree::Nconc(Ptree::List(p->Car(), size2, p->Third()),
				    q);
	    }
	}
	else if(head->Eq('('))
	    break;
    }

    return decl;
}

Ptree* Walker::TranslateArguments(Ptree* arglist)
{
    if(arglist == nil)
	return arglist;

    PtreeArray array;
    bool changed = FALSE;
    Ptree* body = Ptree::Second(arglist);
    Ptree* args = body;
    while(args != nil){
	Ptree* p = args->Car();
	Ptree* q = Translate(p);
	array.Append(q);
	if(p != q)
	    changed = TRUE;

	args = args->Cdr();
	if(args != nil){
	    array.Append(args->Car());
	    args = args->Cdr();
	}
    }

    if(changed)
	return Ptree::ShallowSubst(array.All(), body, arglist);
    else
	return arglist;
}

void Walker::SetDeclaratorComments(Ptree* def, Ptree* comments)
{
    if (def == nil || !def->IsA(ntDeclaration))
	return;

    Ptree* decl;
    int n = 0;
    for (;;) {
	int i = n++;
	decl = NthDeclarator(def, i);
	if (decl == nil)
	    break;
	else if (decl->IsA(ntDeclarator))
	    ((PtreeDeclarator*)decl)->SetComments(comments);
    }
}

Ptree* Walker::NthDeclarator(Ptree* def, int& nth)
{
    Ptree* decls = def->Third();
    if(decls == nil || decls->IsLeaf())
	return nil;

    if(decls->IsA(ntDeclarator)){	// if it is a function
	if(nth-- == 0)
	    return decls;
    }
    else
	while(decls != nil && !decls->IsLeaf()){
	    if(nth-- == 0)
		return decls->Car();

	    if((decls = decls->Cdr()) != nil)
		decls = decls->Cdr();		// skip ,
	}

    return nil;
}

Ptree* Walker::FindDeclarator(Ptree* def, char* name, int len,
			      char* signature, int& nth, Environment* e)
{
    Ptree* decls = def->Third();
    if(decls == nil || decls->IsLeaf())
	return nil;

    if(decls->IsA(ntDeclarator)){	// if it is a function
	if(MatchedDeclarator(decls, name, len, signature, e))
	    return decls;

	++nth;
    }
    else
	while(decls != nil){
	    Ptree* d = decls->Car();
	    if(MatchedDeclarator(d, name, len, signature, e))
		return d;

	    ++nth;
	    if((decls = decls->Cdr()) != nil)
		decls = decls->Cdr();		// skip ,
	}

    return nil;
}

bool Walker::MatchedDeclarator(Ptree* decl, char* name, int len,
			       char* signature, Environment* e)
{
    char* str;
    int strlen;
    char* sig;

    str = decl->GetEncodedName();
    sig = decl->GetEncodedType();
    if(str == nil || sig == nil)
	return FALSE;

    str = Encoding::GetBaseName(str, strlen, e);
    return bool(len == strlen && memcmp(name, str, len) == 0
		&& strcmp(signature, sig) == 0);
}

bool Walker::WhichDeclarator(Ptree* def, Ptree* name, int& nth,
			     Environment* env)
{
    char* str;
    int len;
    Environment* e;
    Ptree* decls = def->Third();
    if(decls == nil || decls->IsLeaf())
	return FALSE;

    if(decls->IsA(ntDeclarator)){	// if it is a function
	str = decls->GetEncodedName();
	e = env;
	str = Encoding::GetBaseName(str, len, e);
	if(name->Eq(str, len))
	    return TRUE;

	++nth;
    }
    else
	while(decls != nil){
	    str = decls->Car()->GetEncodedName();
	    e = env;
	    str = Encoding::GetBaseName(str, len, e);
	    if(name->Eq(str, len))
		return TRUE;

	    ++nth;
	    if((decls = decls->Cdr()) != nil)
		decls = decls->Cdr();
	}

    return FALSE;
}

void Walker::ErrorMessage(char* msg, Ptree* name, Ptree* where)
{
    parser->ErrorMessage(msg, name, where);
}

void Walker::WarningMessage(char* msg, Ptree* name, Ptree* where)
{
    parser->WarningMessage(msg, name, where);
}

// InaccurateErrorMessage() may report a wrong line number.

void Walker::InaccurateErrorMessage(char* msg, Ptree* name, Ptree* where)
{
    if(default_parser == nil)
	MopErrorMessage("Walker::InaccurateErrorMessage()",
			"no default parser");
    else
	default_parser->ErrorMessage(msg, name, where);
}

void Walker::InaccurateWarningMessage(char* msg, Ptree* name, Ptree* where)
{
    if(default_parser == nil)
	MopErrorMessage("Walker::InaccurateWarningMessage()",
			"no default parser");
    else
	default_parser->WarningMessage(msg, name, where);
}
