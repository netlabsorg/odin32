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

#ifndef _classwalk_h
#define _classwalk_h

#include "ptree-core.h"
#include "walker.h"

class ClassWalker : public Walker {
public:
    ClassWalker(Parser* p) : Walker(p) { client_data = nil; }
    ClassWalker(Parser* p, Environment* e)
	: Walker(p, e) { client_data = nil; }
    ClassWalker(Environment* e) : Walker(e) { client_data = nil; }
    ClassWalker(Walker* w) : Walker(w) { client_data = nil; }

    bool IsClassWalker();
    void InsertBeforeStatement(Ptree*);
    void AppendAfterStatement(Ptree*);
    void InsertBeforeToplevel(Ptree*);
    void AppendAfterToplevel(Ptree*);
    bool InsertDeclaration(Ptree*, Class*, Ptree*, void*);
    void* LookupClientData(Class*, Ptree*);

    Ptree* GetInsertedPtree();
    Ptree* GetAppendedPtree();

    Ptree* TranslateMetaclassDecl(Ptree* decl);
    Ptree* TranslateClassSpec(Ptree* spec, Ptree* userkey,
			      Ptree* class_def, Class* metaobject);
    Ptree* TranslateTemplateInstantiation(Ptree* spec, Ptree* userkey,
					Ptree* class_def, Class* metaobject);
    Ptree* ConstructClass(Class* metaobject);

    Ptree* ConstructAccessSpecifier(int access);
    Ptree* ConstructMember(void* /* i.e. ChangedMemberList::Mem* */);

    Ptree* TranslateStorageSpecifiers(Ptree*);
    Ptree* TranslateTemplateFunction(Ptree* temp_def, Ptree* impl);
    Class* MakeMetaobjectForCfunctions();
    Ptree* TranslateFunctionImplementation(Ptree*);
    Ptree* MakeMemberDeclarator(bool record,
				void* /* aka ChangedMemberList::Mem* */,
				PtreeDeclarator*);
    Ptree* RecordArgsAndTranslateFbody(Class*, Ptree* args, Ptree* body);
    Ptree* TranslateFunctionBody(Ptree*);
    Ptree* TranslateBlock(Ptree*);
    Ptree* TranslateArgDeclList(bool, Ptree*, Ptree*);
    Ptree* TranslateInitializeArgs(PtreeDeclarator*, Ptree*);
    Ptree* TranslateAssignInitializer(PtreeDeclarator*, Ptree*);
    Ptree* TranslateUserAccessSpec(Ptree*);
    Ptree* TranslateAssign(Ptree*);
    Ptree* TranslateInfix(Ptree*);
    Ptree* TranslateUnary(Ptree*);
    Ptree* TranslateArray(Ptree*);
    Ptree* TranslatePostfix(Ptree*);
    Ptree* TranslateFuncall(Ptree*);
    Ptree* TranslateDotMember(Ptree*);
    Ptree* TranslateArrowMember(Ptree*);
    Ptree* TranslateThis(Ptree*);
    Ptree* TranslateVariable(Ptree*);
    Ptree* TranslateUserStatement(Ptree*);
    Ptree* TranslateStaticUserStatement(Ptree*);
    Ptree* TranslateNew2(Ptree*, Ptree*, Ptree*, Ptree*, Ptree*,
			 Ptree*, Ptree*);
    Ptree* TranslateDelete(Ptree*);

private:
    static Class* GetClassMetaobject(TypeInfo&);
    PtreeArray* RecordMembers(Ptree*, Ptree*, Class*);
    void RecordMemberDeclaration(Ptree* mem, PtreeArray* tspec_list);
    Ptree* TranslateStorageSpecifiers2(Ptree* rest);

    static Ptree* CheckMemberEquiv(Ptree*, Ptree*);
    static Ptree* CheckEquiv(Ptree* p, Ptree* q) {
	return Ptree::Equiv(p, q) ? p : q;
    }

private:
    struct ClientDataLink : public LightObject {
	ClientDataLink*	next;
	Class*		metaobject;
	Ptree*		key;
	void*		data;
    };

    PtreeArray before_statement, after_statement;
    PtreeArray before_toplevel, after_toplevel;
    PtreeArray inserted_declarations;
    ClientDataLink* client_data;
};

#endif /* _classwalk_h */
