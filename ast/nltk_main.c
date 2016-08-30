//
// An example program that uses the ast library.
// This builds the ast top-down, however, when you build an ast
// as part of the parsing step, you will be building it from the
// leaf nodes up.
//
// (Thanks to Tia Newhall)
//
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "ast.h"

#define TOKEN0  0 
#define TOKEN1  1 
#define TOKEN2  2 
#define TOKEN3  3 
#define TOKEN4  4 
#define TOKEN5  5 
#define TOKEN6  6 
#define TOKEN7  7 
#define TOKEN8  8 
#define TOKEN9  9 

#define NONTERM 100 
#define PROGRAM 101
#define FUNCTION 102
#define NONE 0
#define FUNCSTR "Function"  
#define PROGSTR "Program"  

static char *token_strings[] = { 
                        "Hello", "There", "What", "is",
                        "Happening?"
                };

void print_token(FILE *outfile, ast_info *t);

int main(int argc, char *argv[]) {

  ast atree;
  int i, j, k, p;
  ast_info *s;
  ast_node *n;
  FILE *outfile;
  char lexeme[MAX_LEXEME_SIZE];

  // create and init new ast node
  s = create_new_ast_node_info(NONTERM, 0, PROGRAM, 0, 0);
  n = create_ast_node(s);
  init_ast(&atree, n);

  for(i=0; i < 3; i++) {
        sprintf(lexeme, "t_%d", i);  
        printf("creating node %s\n", lexeme);
        s = create_new_ast_node_info(i, i, NONE,lexeme,0);
        n = create_ast_node(s);
        if(s == NULL || n==NULL) { printf("ERROR token create\n"); exit(1); }
        add_child_node(atree.root, n);
  }

  for(i=0; i < 3; i++) {
    for(j=0; j < 2; j++) {
        sprintf(lexeme, "t_%d_%d", i,j);  
        printf("creating node %s\n", lexeme);
        s = create_new_ast_node_info(j, i*10+j, NONE, lexeme, 0);
        n = create_ast_node(s);
        if(s == NULL || n==NULL) { printf("ERROR token create\n"); exit(1); }
        add_child_node((atree.root->childlist[i]), n);
    }
  }

  // let's just add another one that could be how you would 
  // add one for a non-terminal grammar symbol
  s = create_new_ast_node_info(NONTERM, 0, FUNCTION, 0, 0);
  n = create_ast_node(s);
  add_child_node(atree.root, n);
         
  if(argc == 2) {
    outfile = fopen(argv[1], "w");
    if(outfile) {
        create_nltk(outfile, atree, print_token); 
    }
  }

  // clean up all malloced ast state
  destroy_ast(&atree); 
  exit(0);
}


//**********************************************************************
// This is a print function that is passed to the create_nltk function.
// The user of the ast library provides this function since the
// particular tokens are defined by the user and will likely differ
// from one source to another...the ast library is more generic this
// way.  Your print function must have the same prototye as this one.
//
// note: that you need a FILE * argument
//
// warning: do not print new line characters in the format string to fprintf
//          otherwise the resulting nltk format may get messed up
void print_token(FILE *outfile, ast_info *t) {

  if(t != NULL) {

    if((t->token <= TOKEN9) && (t->token >= TOKEN0)) {
        fprintf(outfile, "%s:%d", token_strings[t->token], t->value); 
    }
    else if ((t->token == NONTERM)) {
      if(t->grammar_symbol == FUNCTION){
        fprintf(outfile, "%s:%d", FUNCSTR, t->value);
      } else if(t->grammar_symbol == PROGRAM){
        fprintf(outfile, "%s:%d", PROGSTR, t->value);
      } else {
        fprintf(outfile, "unknown token");
      }
    }
    else if(strlen(t->lexeme)) {
        fprintf(outfile, ":%s", t->lexeme);
    }
    else {
      fprintf(outfile, "unknown token");
    }
  }
  else {
    fprintf(outfile, "NULL token");
  }
}

