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

/*
  Copyright (c) 1995, 1996 Xerox Corporation.
  All Rights Reserved.

  Use and copying of this software and preparation of derivative works
  based upon this software are permitted. Any copy of this software or
  of any derivative work must include the above copyright notice of
  Xerox Corporation, this paragraph and the one after it.  Any
  distribution of this software or derivative works must comply with all
  applicable United States export control laws.

  This software is made available AS IS, and XEROX CORPORATION DISCLAIMS
  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF XEROX CORPORATION IS ADVISED
  OF THE POSSIBILITY OF SUCH DAMAGES.
*/

#ifndef _token_h
#define _token_h

#include "types.h"

class Program;
class HashTable;
class Ptree;

// class Token

class Token {
public:
    bool Eq(char c) { return(*ptr == c && len == 1); }

public:
    char* ptr;
    int len;
    int kind;
};

// class Lex

class Lex : public Object {
public:
    Lex(Program*);
    int GetToken(Token&);
    int LookAhead(int);
    int LookAhead(int, Token&);

    char* Save();
    void Restore(char*);
    void GetOnlyClosingBracket(Token&);

    Ptree* GetComments();
    Ptree* GetComments2();
    #ifdef SDS
    Ptree* GetAllComments();
    #endif

    uint LineNumber(char*, char*&, int&);
    #ifdef SDS
    uint LineNumber(char*, char*&, int&, int&, int&);
    #endif

    static bool RecordKeyword(char*, int);
    static bool Reify(Ptree*, unsigned int&);
    static bool Reify(Ptree* t, char*&);

private:
    class TokenFifo {
    public:
	TokenFifo(Lex*);
	~TokenFifo();
	void Clear();
	void Push(int, char*, int);
	int Pop(char*&, int&);
	int Peek(int);
	int Peek(int, char*&, int&);
    private:
	int Peek2(int);
	Lex* lex;
	int head;
	int tail;
	int size;
	struct Slot {
	    int token;
	    char* pos;
	    int len;
	}* ring;
    };

    friend class TokenFifo;

    uint Tokenp() { return tokenp; }
    int TokenLen() { return token_len; }
    char* TokenPosition();
    char Ref(uint i);
    void Rewind(char*);

    int ReadToken(char*&, int&);
    void SkipAttributeToken();
    int SkipExtensionToken(char*&, int&);

#if defined(_MSC_VER) || defined(_PARSE_VCC) || defined(OS2)
    void SkipAsmToken();
    void SkipDeclspecToken();
#endif

    char GetNextNonWhiteChar();
    int ReadLine();
    bool ReadCharConst(uint top);
    bool ReadStrConst(uint top);
    int ReadNumber(char c, uint top);
    int ReadFloat(uint top);
    bool ReadLineDirective();
    int ReadIdentifier(uint top);
    int Screening(char *identifier, int len);
    int ReadSeparator(char c, uint top);
    int SingleCharOp(unsigned char c);
    int ReadComment(char c, uint top);

private:
    Program* file;
    TokenFifo fifo;
    uint tokenp;
    int token_len;
    int last_token;

    static HashTable* user_keywords;
    static Ptree* comments;
    #ifdef SDS
    static Ptree* all_comments;
    #endif
};

// convenient functions

inline bool is_blank(char c){
    return(c == ' ' || c == '\t' || c == '\f' || c == '\r');
}

inline bool is_letter(char c){
    return('A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || c == '_'
	   || c == '$');
}

inline bool is_digit(char c){ return('0' <= c && c <= '9'); }

inline bool is_xletter(char c){ return(c == 'X' || c == 'x'); }

inline bool is_eletter(char c){ return(c == 'E' || c == 'e'); }

inline bool is_hexdigit(char c){
    return(is_digit(c) || 'A' <= c && c <= 'F' || 'a' <= c && c <= 'f');
}

inline bool is_int_suffix(char c){
    return(c == 'U' || c == 'u' || c == 'L' || c == 'l');
}

inline bool is_float_suffix(char c){
    return(c == 'F' || c == 'f' || c == 'L' || c == 'l');
}

// tokens

#define	Identifier	258
#define	Constant	262
#define	CharConst	263
#define	StringL		264
#define	AssignOp	267
#define	EqualOp		268
#define	RelOp		269
#define	ShiftOp		270
#define	LogOrOp		271
#define	LogAndOp	272
#define	IncOp		273
#define	Scope		274
#define	Ellipsis	275
#define	PmOp		276
#define	ArrowOp		277
#define	BadToken	278
#define	AUTO		281
#define	CHAR		282
#define	CLASS		283
#define	CONST		284
#define	DELETE		285
#define	DOUBLE		286
#define	ENUM		287
#define	EXTERN		288
#define	FLOAT		289
#define	FRIEND		290
#define	INLINE		291
#define	INT		292
#define	LONG		293
#define	NEW		294
#define	OPERATOR	295
#define	PRIVATE		296	/* must be consistent with Class::Private */
#define	PROTECTED	297	/* must be consistent with Class::Protected */
#define	PUBLIC		298	/* must be consistent with Class::Public */
#define	REGISTER	299
#define	SHORT		300
#define	SIGNED		301
#define	STATIC		302
#define	STRUCT		303
#define	TYPEDEF		304
#define	UNION		305
#define	UNSIGNED	306
#define	VIRTUAL		307
#define	VOID		308
#define	VOLATILE	309
#define	TEMPLATE	310
#define	MUTABLE		311
#define	BREAK		312
#define	CASE		313
#define	CONTINUE	314
#define	DEFAULT		315
#define	DO		316
#define	ELSE		317
#define	FOR		318
#define	GOTO		319
#define	IF		320
#define	RETURN		321
#define	SIZEOF		322
#define	SWITCH		323
#define	THIS		324
#define	WHILE		325
#define	ATTRIBUTE	326		// g++
#define	METACLASS	327
#define	UserKeyword	328
#define	UserKeyword2	329
#define	UserKeyword3	330
#define	UserKeyword4	331
#define BOOLEAN		332
#define EXTENSION	333		// g++
#define TRY		334
#define CATCH		335
#define THROW		336
#define UserKeyword5	337
#define NAMESPACE	338
#define USING		339

// non terminals

#define ntDeclarator	400
#define ntName		401
#define ntFstyleCast	402
#define ntClassSpec	403
#define ntEnumSpec	404
#define ntDeclaration	405
#define ntTypedef	406
#define ntTemplateDecl	407
#define ntMetaclassDecl	408
#define ntLinkageSpec	409
#define ntAccessSpec	410
#define ntUserAccessSpec 411
#define ntUserdefKeyword 412
#define ntExternTemplate 413
#define ntAccessDecl	414
#define ntNamespaceSpec	415
#define ntUsing		416
#define ntTemplateInstantiation	417

#define ntIfStatement		420
#define ntSwitchStatement	421
#define ntWhileStatement	422
#define ntDoStatement		423
#define ntForStatement		424
#define ntBreakStatement	425
#define ntContinueStatement	426
#define ntReturnStatement	427
#define ntGotoStatement		428
#define ntCaseStatement		429
#define ntDefaultStatement	430
#define ntLabelStatement	431
#define ntExprStatement		432
#define ntTryStatement		433

#define ntCommaExpr		450
#define ntAssignExpr		451
#define ntCondExpr		452
#define ntInfixExpr		453
#define ntPmExpr		454
#define ntCastExpr		455
#define ntUnaryExpr		456
#define ntSizeofExpr		457
#define ntNewExpr		458
#define ntDeleteExpr		459
#define ntArrayExpr		460
#define ntFuncallExpr		461
#define ntPostfixExpr		462
#define ntUserStatementExpr	463
#define ntDotMemberExpr		464
#define ntArrowMemberExpr	465
#define ntParenExpr		466
#define ntStaticUserStatementExpr 467
#define ntThrowExpr		468

#define Ignore		500
#define ASM		501
#define DECLSPEC        502
#define INT64		503

#endif /* _token_h */
