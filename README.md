Complete compiler for C-- created as a project for a course Compiler Design at Union College, NY.  

Contains lexer, parser, and code generation. Some level of error recovery is implemented. The program outputs MIPS code.

C-- grammar: 
```
Program ------> VarDeclList FunDeclList 

VarDeclList --> epsilon 
		VarDecl  VarDeclList

VarDecl ------> Type id ;
                Type id [ num] ; 

FunDeclList --> FunDecl 
 		FunDecl  FunDeclList

FunDecl ------> Type id ( ParamDecList ) Block

ParamDeclList --> epsilon 
 		  ParamDeclListTail 

ParamDeclListTail -->  ParamDecl 
 		       ParamDecl,  ParamDeclListTail 

ParamDecl ----> Type id
		Type id[]

Block --------> { VarDeclList StmtList } 

Type ---------> int
                char

StmtList -----> Stmt 
 		Stmt  StmtList 

Stmt ---------> ;
                Expr ; 
                return Expr ;
                read id ;
                write Expr ;
                writeln ;
                break ;
                if ( Expr ) Stmt else Stmt
                while ( Expr ) Stmt 
                Block

Expr ---------> Primary 
                UnaryOp Expr
                Expr BinOp Expr
                id = Expr 
                id [ Expr ] = Expr 

Primary ------> id
                num 
                ( Expr )
                id ( ExprList )
                id [ Expr ] 

ExprList -----> epsilon 
                ExprListTail 

ExprListTail --> Expr 
                 Expr , ExprListTail
 
UnaryOp ------> - | !

BinOp -------->  + | - | * | / | == | != | < | <= | > | >=  | && | ||
```
