/*
 * IDL Compiler
 *
 * Copyright 2002 Ove Kaaven
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __WIDL_WIDL_H
#define __WIDL_WIDL_H

#include "widltypes.h"

#include <time.h>

#define WIDL_FULLVERSION "0.1"

extern int debuglevel;
#define DEBUGLEVEL_NONE		0x0000
#define DEBUGLEVEL_CHAT		0x0001
#define DEBUGLEVEL_DUMP		0x0002
#define DEBUGLEVEL_TRACE	0x0004
#define DEBUGLEVEL_PPMSG	0x0008
#define DEBUGLEVEL_PPLEX	0x0010
#define DEBUGLEVEL_PPTRACE	0x0020

extern int win32;
extern int pedantic;
extern int do_header;
extern int do_typelib;
extern int do_proxies;
extern int do_client;
extern int do_server;

extern char *input_name;
extern char *header_name;
extern char *typelib_name;
extern char *proxy_name;
extern char *proxy_token;
extern char *client_name;
extern char *client_token;
extern char *server_name;
extern char *server_token;
extern time_t now;

extern int line_number;
extern int char_number;

extern FILE* header;

extern void write_proxies(ifref_t *ifaces);
extern void write_client(ifref_t *ifaces);
extern void write_server(ifref_t *ifaces);

#endif
