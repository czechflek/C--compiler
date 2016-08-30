#ifndef _SYMTAB_H
#define _SYMTAB_H

typedef enum {
    T_INT,
    T_CHAR,
    T_INTARR,
    T_CHARARR
} VARTYPE;

typedef struct {
    const char * name;
    VARTYPE type;
    int size;
    int stack_height; //distance from the stack bottom
    int is_array;
    int count;
} Symentry;

typedef struct {
    const char * name;
    VARTYPE type;
    VARTYPE param_type[4];
    int is_array[4];
    const char * param_name[4];
    int param_count;

} FunDef;

typedef struct {
    int size;
    int offset;
    int count;
} VarAddress;

void add_variable(VARTYPE type, const char * name);
void add_array(VARTYPE type, const char * name, int arr_size, int zero_size);
int get_padding();
int get_scope_size();
VarAddress lookup_variable(const char * name);
VarAddress lookup_array(const char * name);
int destroy_scope(int verbose);
void add_scope();
FunDef define_function(VARTYPE type, const char * name);
void add_parameter(VARTYPE type, int is_array, const char * name);
void set_current_function(FunDef * fun);
FunDef lookup_function(const char* name);
void copy_parameters();
void adjust_stack_height(int offset);
int get_current_stack_height();

#endif
