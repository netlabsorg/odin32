%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE	1
#define FALSE	0

char  arg = 'a';
char  procname[256] = {0};
char  defprocname[256] = {0};
char  tempname[256];
char  returntype[256] = {0};
char  typename[256] = {0};
char  callconv[256] = {0};
int   i;
int   firstarg = TRUE;
int   fStartProc = FALSE;
int   fVoidReturn = FALSE;
int   nrargs = 0;
FILE  *file = NULL;
%}

%token NAME
%token ENDOFPROC
%token BRACKETOPEN
%token BRACKETCLOSE
%token COMMA
%token POINTER
%token PPOINTER
%token CONST
%token VOID
%token BEGINPROC
%token ENDPROC
%token STAT_IFDEF
%token STAT_IFNDEF
%token STAT_ENDIF
%token STAT_INCLUDE
%token STAT_DEFINE
%token STAT_UNDEF
%token STAT_INLINE
%token STRUCT
%%

lines		: line
		| lines line
		;

line		: IFDEFSTATEMENT
		| ENDIFSTATEMENT
                | INCLUDESTATEMENT
                | DEFINESTATEMENT
		| PROCEDUREDEF
		;

//PROCEDUREDEF	: RET PROCNAME BRACKETOPEN ARGUMENTS BRACKETCLOSE ENDOFPROC
PROCEDUREDEF	: RET CALLCONV PROCNAME BRACKETOPEN ARGUMENTS BRACKETCLOSE ENDOFPROC
		{
                        fprintf(file, "    push   fs\n");
                        fprintf(file, "    mov    eax, 0150bh\n");
                        fprintf(file, "    mov    fs, eax\n");

                        for(i=0;i<nrargs;i++) {
                            fprintf(file, "    push   dword ptr [esp+%d]\n", 8+(nrargs-i-1)*4 + i*4);
                        }
                        fprintf(file, "    call   %s\n", &defprocname[1]);
                        if(nrargs) {
                            fprintf(file, "    add    esp, %d\n", nrargs*4);
                        }
                        fprintf(file, "    pop    fs\n");
                        fprintf(file, "    ret\n");
                        fprintf(file, "%s ENDP\n", defprocname);

                        fprintf(file, "\nCODE32          ENDS\n\n");
                        fprintf(file, "                END\n");
                        fclose(file);
		}
		;


RET		: NAME
		{
			strcpy(typename, (char *)$1);
			strupr(typename);
//			printf("inline %s %s ", $1, callconv);
//			printf("inline %s ", $1);
			if(strcmp(typename, "VOID") != 0) {
				fVoidReturn = FALSE;
			}
			else    fVoidReturn = TRUE;
		}
		| NAME POINTER
		{
			strcpy(typename, (char *)$1);
			strupr(typename);
                        strcat(typename, " *");
//			printf("inline %s %s *", $1, callconv);
//			printf("inline %s *", $1);
			if(strcmp(typename, "VOID") != 0) {
				fVoidReturn = FALSE;
			}
			else    fVoidReturn = TRUE;
		}
		;
CALLCONV	: NAME
		{
			strcpy(callconv, (char *)$1);
		}
		;

PROCNAME	: NAME
		{
			strcpy(procname, (char *)$1);
                        strcat(procname, ".asm");
                        file = fopen(procname, "wb+");

			strcpy(procname, (char *)$1);
			strcpy(defprocname, procname);

			fprintf(file, "        .386p\n");
			fprintf(file, "CODE32          SEGMENT DWORD PUBLIC USE32 'CODE'\n");
			fprintf(file, "\n        EXTRN   %s:PROC\n", &defprocname[1]);
			fprintf(file, "        PUBLIC  %s\n", $1);
			fprintf(file, "%s PROC NEAR\n", $1);

                        nrargs = 0;

			if(!strncmp(procname, "Mou", 3))
			{
				strupr(defprocname);
				sprintf(tempname, "MOU16%s", &defprocname[3]);
				strcpy(defprocname, tempname);
			}
			else
			if(!strncmp(procname, "Kdb", 3))
			{
				strupr(defprocname);
				sprintf(tempname, "KDB16%s", &defprocname[3]);
				strcpy(defprocname, tempname);
			}
			else
			if(!strncmp(procname, "Vio", 3)) 
			{
				strupr(defprocname);
				sprintf(tempname, "VIO16%s", &defprocname[3]);
				strcpy(defprocname, tempname);
			}
			$$ = $1;
		}
		;
ARGUMENTS	: ARGUMENTS COMMA ARGUMENT
		| ARGUMENT
		| /* nothing */
		;

ARGUMENT	: TYPE NAME
		| TYPE 
		;

TYPE		: CONST NAME PPOINTER
		{
                        nrargs++;
		}
                | CONST STRUCT NAME POINTER
		{
                        nrargs++;
		}
		| CONST NAME POINTER
		{
                        nrargs++;
		}
		| CONST NAME 
		{
                        nrargs++;
		}
		| NAME PPOINTER
		{
                        nrargs++;
		}
		| NAME POINTER POINTER
		{
                        nrargs++;
		}
		| STRUCT NAME POINTER
		{
                        nrargs++;
		}
		| NAME POINTER
		{
                        nrargs++;
		}
                | STRUCT NAME
		{
                        nrargs++;
		}
		| NAME
		{
			strcpy(typename, (char *)$1);
			strupr(typename);
			if(strcmp(typename, "VOID") != 0) {
                            nrargs++;
                        }
		}
		;

IFDEFSTATEMENT  : STAT_IFDEF NAME
		{
		}
                | STAT_IFNDEF NAME
		{
		}
		;

ENDIFSTATEMENT  : STAT_ENDIF
		{
		}
		;

INCLUDESTATEMENT : STAT_INCLUDE NAME
		{
		}
		;

DEFINESTATEMENT : STAT_DEFINE NAME NAME
		{
		}
		;
                | STAT_UNDEF NAME
		{
		}
		;

%%


main()
{
	yyparse();
}

yyerror(char *msg)
{
	printf("yyerror: %s (%s)\n", msg, procname);
	exit(1);
}

