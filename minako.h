#ifndef MINAKO_H
#define MINAKO_H

#ifndef YYSTYPE
typedef union {
	char* string;
	double floatValue;
	int intValue;
} yystype;
#define YYSTYPE yystype
#endif	/* YYSTYPE */

#define AND             257
#define OR              258
#define EQ              259
#define NEQ             260
#define LEQ             261
#define GEQ             262
#define LSS             263
#define GRT             264
#define KW_BEGIN		265
#define KW_BOOLEAN      266
#define KW_DO           267
#define KW_ELSE         268
#define KW_END			269
#define KW_FLOAT        270
#define KW_FOR          271
#define KW_IF           272
#define KW_INT          273
#define KW_PRINTF       274
#define KW_RETURN       275
#define KW_VOID         276
#define KW_WHILE        277
#define CONST_INT       278
#define CONST_FLOAT     279
#define CONST_BOOLEAN   280
#define ID              281

extern YYSTYPE yylval;
extern int yylex();
extern FILE* yyin;
extern int yylineno;

#endif
