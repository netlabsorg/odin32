/* $Id: wsastruct.cpp,v 1.1 2000-03-22 20:01:07 sandervl Exp $ */

/*
 *
 * Win32 SOCK32 for OS/2 (WSA apis)
 *
 * Based on Wine code: (dlls\winsock\async.c,socket.c)
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 * (C) 1999 Marcus Meissner
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/* ----------------------------------- helper functions -
 *
 * TODO: Merge WS_dup_..() stuff into one function that
 * would operate with a generic structure containing internal
 * pointers (via a template of some kind).
 */

#define INCL_BASE
#include <os2wrap.h>	//Odin32 OS/2 api wrappers

#include <string.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <stdlib.h>
#include <win32api.h>
#include <misc.h>

#include "wsock32.h"
#include "wsastruct.h"

struct ws_hostent* _check_buffer_he(LPWSINFO pwsi, int size)
{
    if( pwsi->he && pwsi->helen >= size ) return pwsi->he;
    else free(pwsi->he);

    pwsi->he = (struct ws_hostent*)malloc((pwsi->helen = size)); 
    return pwsi->he;
}

struct ws_servent* _check_buffer_se(LPWSINFO pwsi, int size)
{
    if( pwsi->se && pwsi->selen >= size ) return pwsi->se;
    else free(pwsi->se);

    pwsi->se = (struct ws_servent*)malloc((pwsi->selen = size)); 
    return pwsi->se;
}

struct ws_protoent* _check_buffer_pe(LPWSINFO pwsi, int size)
{
    if( pwsi->pe && pwsi->pelen >= size ) return pwsi->pe;
    else free(pwsi->pe);

    pwsi->pe = (struct ws_protoent*)malloc((pwsi->pelen = size)); 
    return pwsi->pe;
}

static int list_size(char** l, int item_size)
{
  int i,j = 0;
  if(l)
  { for(i=0;l[i];i++) 
	j += (item_size) ? item_size : strlen(l[i]) + 1;
    j += (i + 1) * sizeof(char*); }
  return j;
}

static int list_dup(char** l_src, char* ref, char* base, int item_size)
{ 
   /* base is either either equal to ref or 0 or SEGPTR */

   char*		p = ref;
   char**		l_to = (char**)ref;
   int			i,j,k;

   for(j=0;l_src[j];j++) ;
   p += (j + 1) * sizeof(char*);
   for(i=0;i<j;i++)
   { l_to[i] = base + (p - ref);
     k = ( item_size ) ? item_size : strlen(l_src[i]) + 1;
     memcpy(p, l_src[i], k); p += k; }
   l_to[i] = NULL;
   return (p - ref);
}

/* ----- hostent */

static int hostent_size(struct hostent* p_he)
{
  int size = 0;
  if( p_he )
  { size  = sizeof(struct hostent); 
    size += strlen(p_he->h_name) + 1;
    size += list_size(p_he->h_aliases, 0);  
    size += list_size(p_he->h_addr_list, p_he->h_length ); }
  return size;
}

int WS_dup_he(LPWSINFO pwsi, struct hostent* p_he, int flag)
{
   /* Convert hostent structure into ws_hostent so that the data fits 
    * into pwsi->buffer. Internal pointers can be linear, SEGPTR, or 
    * relative to pwsi->buffer depending on "flag" value. Returns size
    * of the data copied (also in the pwsi->buflen).
    */

   int size = hostent_size(p_he);

   if( size )
   {
     struct ws_hostent* p_to;
     char* p_name,*p_aliases,*p_addr,*p_base,*p;

     _check_buffer_he(pwsi, size);
     p_to = (struct ws_hostent*)pwsi->he;
     p = (char*)pwsi->he;
     p_base = (flag & WS_DUP_OFFSET) ? NULL : p;
////				     : ((flag & WS_DUP_SEGPTR) ? (char*)SEGPTR_GET(p) : p);
     p += sizeof(struct ws_hostent);
     p_name = p;
     strcpy(p, p_he->h_name); p += strlen(p) + 1;
     p_aliases = p;
     p += list_dup(p_he->h_aliases, p, p_base + (p - (char*)pwsi->he), 0);
     p_addr = p;
     list_dup(p_he->h_addr_list, p, p_base + (p - (char*)pwsi->he), p_he->h_length);

     p_to->h_addrtype = (INT16)p_he->h_addrtype; 
     p_to->h_length = (INT16)p_he->h_length;
     p_to->h_name = (char *)(p_base + (p_name - (char*)pwsi->he));
     p_to->h_aliases = (char **)(p_base + (p_aliases - (char*)pwsi->he));
     p_to->h_addr_list = (char **)(p_base + (p_addr - (char*)pwsi->he));

     size += (sizeof(struct ws_hostent) - sizeof(struct hostent));
   }
   return size;
}

/* ----- protoent */

static int protoent_size(struct protoent* p_pe)
{
  int size = 0;
  if( p_pe )
  { size  = sizeof(struct protoent);
    size += strlen(p_pe->p_name) + 1;
    size += list_size(p_pe->p_aliases, 0); }
  return size;
}

int WS_dup_pe(LPWSINFO pwsi, struct protoent* p_pe, int flag)
{
   int size = protoent_size(p_pe);
   if( size )
   {
     struct ws_protoent* p_to;
     char* p_name,*p_aliases,*p_base,*p;

     _check_buffer_pe(pwsi, size);
     p_to = (struct ws_protoent*)pwsi->pe;
     p = (char*)pwsi->pe; 
     p_base = (flag & WS_DUP_OFFSET) ? NULL : p;
////				     : ((flag & WS_DUP_SEGPTR) ? (char*)SEGPTR_GET(p) : p);
     p += sizeof(struct ws_protoent);
     p_name = p;
     strcpy(p, p_pe->p_name); p += strlen(p) + 1;
     p_aliases = p;
     list_dup(p_pe->p_aliases, p, p_base + (p - (char*)pwsi->pe), 0);

     p_to->p_proto = (INT16)p_pe->p_proto;
     p_to->p_name = (char *)(p_base) + (p_name - (char*)pwsi->pe);
     p_to->p_aliases = (char **)((p_base) + (p_aliases - (char*)pwsi->pe)); 

     size += (sizeof(struct ws_protoent) - sizeof(struct protoent));
   }
   return size;
}

/* ----- servent */

static int servent_size(struct servent* p_se)
{
  int size = 0;
  if( p_se )
  { size += sizeof(struct servent);
    size += strlen(p_se->s_proto) + strlen(p_se->s_name) + 2;
    size += list_size(p_se->s_aliases, 0); }
  return size;
}

int WS_dup_se(LPWSINFO pwsi, struct servent* p_se, int flag)
{
   int size = servent_size(p_se);
   if( size )
   {
     struct ws_servent* p_to;
     char* p_name,*p_aliases,*p_proto,*p_base,*p;

     _check_buffer_se(pwsi, size);
     p_to = (struct ws_servent*)pwsi->se;
     p = (char*)pwsi->se;
     p_base = (flag & WS_DUP_OFFSET) ? NULL : p;
////				     : ((flag & WS_DUP_SEGPTR) ? (char*)SEGPTR_GET(p) : p);
     p += sizeof(struct ws_servent);
     p_name = p;
     strcpy(p, p_se->s_name); p += strlen(p) + 1;
     p_proto = p;
     strcpy(p, p_se->s_proto); p += strlen(p) + 1;
     p_aliases = p;
     list_dup(p_se->s_aliases, p, p_base + (p - (char*)pwsi->se), 0);

     p_to->s_port = (INT16)p_se->s_port;
     p_to->s_name = (char *)(p_base + (p_name - (char*)pwsi->se));
     p_to->s_proto = (char *)(p_base + (p_proto - (char*)pwsi->se));
     p_to->s_aliases = (char **)(p_base + (p_aliases - (char*)pwsi->se)); 

     size += (sizeof(struct ws_servent) - sizeof(struct servent));
   }
   return size;
}
/* ----------------------------------- helper functions - */


/* ----- hostent */

/* Copy hostent to p_to, fix up inside pointers using p_base (different for
 * Win16 (linear vs. segmented). Return -neededsize on overrun.
 */
int WS_copy_he(struct ws_hostent *p_to,char *p_base,int t_size,struct hostent* p_he)
{
	char* p_name,*p_aliases,*p_addr,*p;
	int	size=hostent_size(p_he)+(sizeof(struct ws_hostent)-sizeof(struct hostent));

	if (t_size < size)
		return -size;
	p = (char*)p_to;
	p += sizeof(struct ws_hostent);
	p_name = p;
	strcpy(p, p_he->h_name); p += strlen(p) + 1;
	p_aliases = p;
	p += list_dup(p_he->h_aliases, p, p_base + (p - (char*)p_to), 0);
	p_addr = p;
	list_dup(p_he->h_addr_list, p, p_base + (p - (char*)p_to), p_he->h_length);

	p_to->h_addrtype = (INT16)p_he->h_addrtype; 
	p_to->h_length = (INT16)p_he->h_length;
	p_to->h_name = (p_base + (p_name - (char*)p_to));
	p_to->h_aliases = (char **)(p_base + (p_aliases - (char*)p_to));
	p_to->h_addr_list = (char **)(p_base + (p_addr - (char*)p_to));

	return size;
}

/* ----- protoent */

/* Copy protoent to p_to, fix up inside pointers using p_base (different for
 * Win16 (linear vs. segmented). Return -neededsize on overrun.
 */
int WS_copy_pe(struct ws_protoent *p_to,char *p_base,int t_size,struct protoent* p_pe)
{
	char* p_name,*p_aliases,*p;
	int	size=protoent_size(p_pe)+(sizeof(struct ws_protoent)-sizeof(struct protoent));

	if (t_size < size)
		return -size;
	p = (char*)p_to;
	p += sizeof(struct ws_protoent);
	p_name = p;
	strcpy(p, p_pe->p_name); p += strlen(p) + 1;
	p_aliases = p;
	list_dup(p_pe->p_aliases, p, p_base + (p - (char*)p_to), 0);

	p_to->p_proto = (INT16)p_pe->p_proto;
	p_to->p_name = (p_base) + (p_name - (char*)p_to);
	p_to->p_aliases = (char **)((p_base) + (p_aliases - (char*)p_to)); 


	return size;
}

/* ----- servent */

/* Copy servent to p_to, fix up inside pointers using p_base (different for
 * Win16 (linear vs. segmented). Return -neededsize on overrun.
 */
int WS_copy_se(struct ws_servent *p_to, char *p_base, int t_size, struct servent* p_se)
{
	char* p_name,*p_aliases,*p_proto,*p;
	int	size = servent_size(p_se)+(sizeof(struct ws_servent)-sizeof(struct servent));

	if (t_size < size )
		return -size;
	p = (char*)p_to;
	p += sizeof(struct ws_servent);
	p_name = p;
	strcpy(p, p_se->s_name); p += strlen(p) + 1;
	p_proto = p;
	strcpy(p, p_se->s_proto); p += strlen(p) + 1;
	p_aliases = p;
	list_dup(p_se->s_aliases, p, p_base + (p - (char*)p_to), 0);

	p_to->s_port = (INT16)p_se->s_port;
	p_to->s_name = (p_base + (p_name - (char*)p_to));
	p_to->s_proto = (p_base + (p_proto - (char*)p_to));
	p_to->s_aliases = (char **)(p_base + (p_aliases - (char*)p_to)); 

	return size;
}
