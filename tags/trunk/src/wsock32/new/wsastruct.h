/* $Id: wsastruct.h,v 1.1 2000-03-22 20:01:07 sandervl Exp $ */

/*
 *
 * Win32 SOCK32 for OS/2
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WSASTRUCT_H__
#define __WSASTRUCT_H__

#include "wsock32.h"

#define WS_DUP_LINEAR           0x0001
#define WS_DUP_NATIVE           0x0000          /* not used anymore */
#define WS_DUP_OFFSET           0x0002          /* internal pointers are offsets */
#define WS_DUP_SEGPTR           0x0004          /* internal pointers are SEGPTRs */

int WS_copy_se(struct ws_servent *p_to, char *p_base, int t_size, struct servent* p_se);
int WS_copy_pe(struct ws_protoent *p_to,char *p_base,int t_size,struct protoent* p_pe);
int WS_copy_he(struct ws_hostent *p_to,char *p_base,int t_size,struct hostent* p_he);
int WS_dup_se(LPWSINFO pwsi, struct servent* p_se, int flag = WS_DUP_LINEAR);
int WS_dup_pe(LPWSINFO pwsi, struct protoent* p_pe, int flag = WS_DUP_LINEAR);
int WS_dup_he(LPWSINFO pwsi, struct hostent* p_he, int flag = WS_DUP_LINEAR);


#endif //__WSASTRUCT_H__