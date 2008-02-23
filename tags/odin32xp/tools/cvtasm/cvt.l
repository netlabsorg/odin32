%{
#include "cvt.tab.h"

extern int fStartProc;
%}
%option noyywrap

ifdef   "#ifdef"
ifndef  "#ifndef"
endif   "#endif"
include "#include"
define  "#define"
undef   "#undef"
inline  "#inline"
alpha	[A-Za-z]
dig	[0-9]
const   "CONST"|"const"
name	({alpha}|\_)({alpha}|{dig}|\_|\-)*

bracketopen     [\(]
bracketclose    [\)]
endofproc	[\;]
startproc       [\{]
endproc         [\}]
comma           [\,]
pointer         [\*]
array		"*[]"
ws		[ \t\n]+

%%

{ws}		/* skip blanks and tabs */

{const}		return CONST;
{name}		{	
   	     	        yylval = (int)strcpy(malloc(strlen(yytext)+1), yytext);
			return NAME;
		}

{bracketopen}   return BRACKETOPEN;
{bracketclose}  return BRACKETCLOSE;
{endofproc}	return ENDOFPROC;
{comma}		return COMMA;
{pointer}	return POINTER;
{array}		return PPOINTER;
{inline}        return STAT_INLINE;


{ifdef}         return STAT_IFDEF;
{ifndef}        return STAT_IFNDEF;
{endif}         return STAT_ENDIF;
{include}       return STAT_INCLUDE;
{define}        return STAT_DEFINE;
{undef}         return STAT_UNDEF;


%%
