/* $Id: gen_object.h,v 1.1 1999-07-14 08:35:34 sandervl Exp $ */
/*
 * Generic Object Class for OS/2
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __GEN_OBJECT_H__
#define __GEN_OBJECT_H__

#define OBJTYPE_WINDOW 		0
#define OBJTYPE_DIALOG		1
#define OBJTYPE_CLASS		2
#define OBJTYPE_BITMAP		3
#define OBJTYPE_ICON		4
#define OBJTYPE_CURSOR		5
#define OBJTYPE_MENU		6
//......

class GenericObject
{
public:
	 GenericObject(GenericObject **head, DWORD objType);
virtual ~GenericObject();

	 DWORD getObjectType()			{ return objType; };
	 void  setObjectType(DWORD objType)	{ this->objType = objType; };

GenericObject *GetHead()	{ return *head; };
GenericObject *GetNext()	{ return next;  };

	 
private:

         DWORD objType;

protected:

         GenericObject **head;
 	 GenericObject  *next;	 
};

#endif