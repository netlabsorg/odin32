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

#ifndef __WIDL_PARSER_H
#define __WIDL_PARSER_H

int yyparse(void);

extern FILE *yyin;
extern char *yytext;
extern int yydebug;
extern int yy_flex_debug;

int yylex(void);

extern int import_stack_ptr;
int do_import(char *fname);
void abort_import(void);

#define parse_only import_stack_ptr

int is_type(const char *name);

#endif
