/* The lexical analyzer for the C-- Programming Language
 */
// TODO: you are welcome to completely wipe out the contents of this
// file and start from scratch

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "lexer.h"

// these are likely values that will be needed by the parser, so 
// making them global variables is okay
int lexbuf_count = 0;
char lexbuf[MAXLEXSIZE];  // stores current lexeme
int  tokenval=0;          // stores current token's value
                          // (may not used for every token)
int  src_lineno=0;        // current line number in source code input

// char represantation of the lexemes
// the indexes have to correspond to position in enum tokenT
const char * lex_symbol_table[LEXEME_COUNT] =
{
			  "STARTTOKEN", 
			  "ID",
              "INT", "CHAR",
              "IF",
              "NUM",
              "RETURN",
              "READ",
              "WRITE",
              "WRITELN",
              "BREAK",
              "ELSE",
              "WHILE",
              "SEMICOLON",
              "COMMA",
              "LPAREN",
              "RPAREN",
              "LBRACKET",
              "RBRACKET",
              "LCURLY",
              "RCURLY",
              "NEG",
              "PLUS",
              "MINUS",
              "MULT",
              "DIV",
              "EQU",
              "NEQ",
              "LSS",
              "LEQ",
              "GTR",
              "GEQ",
              "AND",
              "OR",
              "ASSIGN",
              "DONE",
              "ENDTOKEN"
};

/**
 * Resets the lexbuf.
 */
static void reset_lexbuf()
{
	lexbuf[0] = '\0';
	lexbuf_count = 0;
}

/**
 * Resets the values.
 */
static void reset_values()
{
	reset_lexbuf();
	tokenval = 0;
}

/**
 * Returns true if the given character is a digit, letter or _.
 *
 * param c: character from the stream
 *
 * returns: true if c is digit, letter or _, false otherwise
 */
static int is_idchar(int c)
{
	return isalnum(c) || c == '_';
}

/**
 * Reads the lexeme value.
 *
 * param c: character from the stream
 *
 */
static void read_val(int c)
{
	if (is_idchar(c))
	{
		if (lexbuf_count >= MAXLEXSIZE - 1) // max size of lexeme
		{
			lexer_error("lexeme too long\n", src_lineno);
			return;
		}
		lexbuf[lexbuf_count++] = c;
		lexbuf[lexbuf_count] = '\0';
	}
	if (isdigit(c))
	{
		int d = c - '0';
		tokenval = tokenval * 10 + d;
	}
}

/**
 * Increases the line number.
 */
static void line_inc()
{
	src_lineno = src_lineno + 1;
}

/**
 * Finds the next character from input, skips whitespaces
 * and advances line number if necessary.
 *
 * param fd: file pointer for reading input file (source C-- program)
 *
 * returns: next character
 */
static int next_char(FILE * fd)
{
	int c;
	while (isspace(c = fgetc(fd)))
	{
		if (c == '\n')
			line_inc();
	}
	read_val(c);
	return c;
}

/**
 * Processes next character from input,
 * and advances line number if necessary.
 *
 * param fd: file pointer for reading input file (source C-- program)
 *
 * returns: next character
 */
static int char_space(FILE * fd)
{
	int c = fgetc(fd);
	if (c == '\n')
	{
		line_inc();
		return -1;
	}
	if (c == EOF)
	{
		ungetc(c, fd);
		return -1;
	}
	if (isspace(c))
	{
		return -1;
	}
	read_val(c);
	return c;
}

static int q67(FILE * fd);
// '\'
static int q69(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case ' ':
		tokenval = ' ';
		return q67(fd);
	case 't':
		tokenval = '\t';
		return q67(fd);
	case 'f':
		tokenval = '\f';
		return q67(fd);
	case 'n':
		tokenval = '\n';
		return q67(fd);
	case 'r':
		tokenval = '\r';
		return q67(fd);
	case 'v':
		tokenval = '\v';
		return q67(fd);
	}
	return LEXERROR;
}

// 'letter'
static int q68(FILE * fd)
{
	reset_lexbuf();
	return NUM;
}

// 'letter
static int q67(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '\'':
		return q68(fd);
	}
	return LEXERROR;
}

// '
static int q66(FILE * fd)
{
	int c = char_space(fd);
	if (isalpha(c))
	{
		tokenval = c;
		return q67(fd);
	}
	switch (c)
	{
	case '\\':
		return q69(fd);
	}
	return LEXERROR;
}

// ,
static int q65(FILE * fd)
{	
	reset_lexbuf();
	return COMMA;
}

// ;
static int q64(FILE * fd)
{
	reset_lexbuf();
	return SEMICOLON;
}

// ||
static int q63(FILE * fd)
{
	reset_lexbuf();
	return OR;
}

// |
static int q62(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '|':
		return q63(fd);
	default:
		lexer_recovery('|', src_lineno);
		return q63(fd);
	}
	return LEXERROR;
}

// digit
static int q61(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return NUM;	
	if (isdigit(c))
		return q61(fd);
	if (isalpha(c)) 
		return LEXERROR;
	ungetc(c, fd);
	reset_lexbuf();
	return NUM;
}

static int q0(FILE * fd);

// //
static int q60(FILE * fd)
{
	int c = fgetc(fd);
	switch (c)
	{
	case '\n':
		line_inc();
		return q0(fd);
	case EOF:
		return DONE;
	}
	return q60(fd);
}

static int q57(FILE * fd);
// /* ... *
static int q58(FILE * fd)
{
	int c = fgetc(fd);
	switch (c)
	{
	case '/':
		return q0(fd);
	case EOF:
		return LEXERROR;
	}
	return q57(fd);
}

// /*
static int q57(FILE * fd)
{
	int c = fgetc(fd);
	switch (c)
	{
	case '\n':
		line_inc();
		break;
	case '*':
		return q58(fd);
	case EOF:
		return LEXERROR;
	}
	return q57(fd);
}

// &&
static int q561(FILE * fd)
{
	reset_lexbuf();
	return AND;
}

// &
static int q56(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '&':
		return q561(fd);
	default:
		lexer_recovery('&', src_lineno);
		return q561(fd);
	}
	return LEXERROR;
}

// identifier
static int q55(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);	
	ungetc(c, fd);
	return ID;
}

// >=
static int q54(FILE * fd)
{
	reset_lexbuf();
	return GEQ;
}

// >
static int q53(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '=':
		return q54(fd);
	}
	ungetc(c, fd);
	reset_lexbuf();
	return GTR;
}

// <=
static int q52(FILE * fd)
{
	reset_lexbuf();
	return LEQ;
}

// <
static int q51(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '=':
		return q52(fd);
	}
	ungetc(c, fd);
	reset_lexbuf();
	return LSS;
}

// !=
static int q50()
{
	reset_lexbuf();
	return NEQ;
}

// !
static int q49(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '=':
		return q50(fd);
	}
	ungetc(c, fd);
	reset_lexbuf();
	return NEG;
}

// ==
static int q48()
{
	reset_lexbuf();
	return EQU;
}

// =
static int q47(FILE * fd)
{
	int c = char_space(fd);
	switch (c)
	{
	case '=':
		return q48(fd);
	}
	ungetc(c, fd);
	reset_lexbuf();
	return ASSIGN;
}

// /
static int q46(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
	{	
		reset_lexbuf();
		return DIV;
	}
	switch (c)
	{
	case '*':
		return q57(fd);
	case '/':
		return q60(fd);
	}
	ungetc(c, fd);
	reset_lexbuf();
	return DIV;
}

// *
static int q45()
{
	reset_lexbuf();
	return MULT;
}

// -
static int q44()
{
	reset_lexbuf();
	return MINUS;
}

// +
static int q43()
{
	reset_lexbuf();
	return PLUS;
}

// }
static int q421()
{
	reset_lexbuf();
	return RCURLY;
}

// {
static int q42()
{
	reset_lexbuf();
	return LCURLY;
}

// ]
static int q41()
{
	reset_lexbuf();
	return RBRACKET;
}

// [
static int q40()
{
	reset_lexbuf();
	return LBRACKET;
}

// )
static int q39()
{
	reset_lexbuf();
	return RPAREN;
}

// (
static int q38()
{
	reset_lexbuf();
	return LPAREN;
}

// while
static int q37(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return WHILE;
}

// whil
static int q36(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'e':
		return q37(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// whi
static int q35(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'l':
		return q36(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// wh
static int q34(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'i':
		return q35(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// else
static int q33(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return ELSE;
}

// els
static int q32(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'e':
		return q33(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// el
static int q31(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 's':
		return q32(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// e
static int q30(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'l':
		return q31(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// if
static int q29(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return IF;
}

// break
static int q27(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return BREAK;
}

// brea
static int q26(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'k':
		return q27(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// bre
static int q25(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'a':
		return q26(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// br
static int q24(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'e':
		return q25(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// b
static int q23(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'r':
		return q24(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// writeln
static int q22(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return WRITELN;
}

// writel
static int q21(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'n':
		return q22(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// write
static int q20(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
	{
		reset_lexbuf();
		return WRITE;
	}
	switch (c)
	{
	case 'l':
		return q21(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return WRITE;
}

// writ
static int q19(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'e':
		return q20(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// wri
static int q18(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 't':
		return q19(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// wr
static int q17(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'i':
		return q18(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// w
static int q16(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'r':
		return q17(fd);
	case 'h':
		return q34(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// read
static int q15(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return READ;
}

// rea
static int q14(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'd':
		return q15(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// return
static int q13(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return RETURN;
}

// retur
static int q12(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'n':
		return q13(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// retu
static int q11(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'r':
		return q12(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// ret
static int q10(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'u':
		return q11(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// re
static int q9(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 't':
		return q10(fd);
	case 'a':
		return q14(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// r
static int q8(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	if (c == 'e')
		return q9(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// char
static int q7(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	reset_lexbuf();
	return CHAR;
}

// cha
static int q6(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	if (c == 'r')
		return q7(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// ch
static int q5(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	if (c == 'a')
		return q6(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// c
static int q4(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	if (c == 'h')
		return q5(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// int
static int q3(FILE * fd)
{
	int c = char_space(fd);
	if (is_idchar(c))
		return q55(fd);
	reset_lexbuf();
	return INT;
}

// in
static int q2(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	if (c == 't')
		return q3(fd);
	if (is_idchar(c))
		return q55(fd);
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// i
static int q1(FILE * fd)
{
	int c = char_space(fd);
	if (c < 0)
		return ID;
	switch (c)
	{
	case 'f':
		return q29(fd);
	case 'n':
		return q2(fd);
	}
	if (is_idchar(c))
		return q55(fd);
	ungetc(c, fd);
	return ID;
}

// starting state
static int q0(FILE * fd) 
{
	int c = next_char(fd);
	if (isdigit(c))
	{
		return q61(fd);
	}
	switch (c)
	{
	case '(':
		return q38(fd);
	case ')':
		return q39(fd);
	case '[':
		return q40(fd);
	case ']':
		return q41(fd);
	case '{':
		return q42(fd);
	case '}':
		return q421(fd);
	case '+':
		return q43(fd);
	case '-':
		return q44(fd);
	case '*':
		return q45(fd);
	case '/':
		return q46(fd);
	case '=':
		return q47(fd);
	case '!':
		return q49(fd);
	case '<':
		return q51(fd);
	case '>':
		return q53(fd);
	case '&':
		return q56(fd);
	case '|':
		return q62(fd);
	case ';':
		return q64(fd);
	case ',':
		return q65(fd);
	case '\'':
		return q66(fd);
	case 'b':
		return q23(fd);
	case 'c':
		return q4(fd);
	case 'e':
		return q30(fd);
	case 'i':
		return q1(fd);
	case 'r':
		return q8(fd);
	case 'w':
		return q16(fd);
	case EOF:
		return DONE;
	}
	if (is_idchar(c))
		return q55(fd);
	return LEXERROR;
}

/***************************************************************************/
/* 
 *  Main lexer routine:  returns the next token in the input 
 *
 *  param fd: file pointer for reading input file (source C-- program)
 *            TODO: you may want to add more parameters
 *
 *  returns: the next token or 
 *           DONE if there are no more tokens 
 *           LEXERROR if there is a token parsing error 
 */
token lexan(FILE *fd) 
{
	reset_values();
	tokenT token_type = q0(fd);
	token t;
	t.type = token_type;
	t.line = src_lineno;
	if (lexbuf_count > 0)
	{
		strcpy(t.lexeme, lexbuf);
		t.t_value = LEXEME;
	}
	else if (t.type == NUM)
	{
		t.t_value = VALUE;
	}
	else
	{
		t.t_value = EMPTY;
	}
	t.value = tokenval;
	return t;
}

