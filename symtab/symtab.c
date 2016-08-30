#include "symtab.h"
#include "codegen.h"
#include "codetable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define FUNCTIONS_SIZE 50

typedef struct Scope {
    Symentry * variables;
    int variables_count;
    int variables_max;
    struct Scope * parent;
} Scope;


int current_stack_height = 0;
Scope * current_scope = NULL;
FunDef * current_function = NULL;
FunDef functions[FUNCTIONS_SIZE];
int functions_count = 0;

void add_scope() {
    printf("Entering a new scope\n");
    Scope * scope = (Scope *) malloc(sizeof (Scope));

    scope->variables = NULL;
    scope->variables_count = 0;
    scope->variables_max = 0;
    scope->parent = current_scope;
    current_scope = scope;
}

int destroy_scope(int verbose) {
    if (verbose) printf("Exiting the scope\n");
    Scope * scope = current_scope;
    if (scope == NULL) return -1;
    current_scope = scope->parent;
    free(scope->variables);
    free(scope);
    return 0;
}

static void extend_scope() {
    int new_size;
    if (current_scope->variables_max > 0) {
        new_size = current_scope->variables_max * 2;
        current_scope->variables = (Symentry *) realloc(current_scope->variables, new_size * sizeof (Symentry));
    } else {
        new_size = 2;
        current_scope->variables = (Symentry *) malloc(new_size * sizeof (Symentry));
    }
    current_scope->variables_max = new_size;
}

static int get_var_size(VARTYPE type)
{
	return (type == T_INT) ? 4 : 1;
}

static void add_variable_to_scope(Symentry entry) {
    int x = current_scope->variables_count++;
    current_scope->variables[x] = entry;
}

static Symentry create_symentry(const char * name, int size, int stack_height, int is_array, int count)
{
	Symentry entry;
	entry.name = name;
    entry.size = size;
    entry.stack_height = stack_height;
    entry.is_array = is_array;
    entry.count = count;
    return entry;
}

static void extend_scope_if_needed()
{
	if (current_scope->variables_count >= current_scope->variables_max)
        extend_scope();
}

void add_variable(VARTYPE type, const char * name) {
    int size = get_var_size(type);
    extend_scope_if_needed();
    add_variable_to_scope(create_symentry(name, size, current_stack_height, 0, 1));
    current_stack_height += size;
}

void add_array(VARTYPE type, const char * name, int arr_size, int reference)
{
	int size = get_var_size(type);
    extend_scope_if_needed();
    add_variable_to_scope(create_symentry(name, size, current_stack_height, 1, arr_size));
    current_stack_height += size * arr_size;
}

int get_scope_size() {
    int size, padding;
    if (current_scope->variables_count == 0) return 0;
    size = current_stack_height - current_scope->variables[0].stack_height;
    padding = (4 - size % 4) % 4;
    current_stack_height += padding;
    return size + padding;
}

void adjust_stack_height(int offset) {
    current_stack_height += offset;
}

int get_current_stack_height() {
	return current_stack_height;
}

/**
 * @return: offset of the variable from the top of the stack
 */
VarAddress lookup_variable(const char * name) {
    Scope * current = current_scope;
    int i;
    VarAddress var;

    while (current != NULL) {
        for (i = 0; i < current->variables_count; i++) {
            if (!strcmp(current->variables[i].name, name) && !current->variables[i].is_array) {
                var.offset = current_stack_height - current->variables[i].stack_height;
                var.size = current->variables[i].size;
                var.count = current->variables[i].count;
                
                return var;
            }
        }
        current = current->parent;
    }
    var.offset = -1;
    var.size = -1;
    
    return var;
}

/**
 * @return: offset of the array from the top of the stack
 */
VarAddress lookup_array(const char * name) {
    Scope * current = current_scope;
    int i;
    VarAddress arr;

    while (current != NULL) {
        for (i = 0; i < current->variables_count; i++) {
            if (!strcmp(current->variables[i].name, name) && current->variables[i].is_array) {
                arr.offset = current_stack_height - current->variables[i].stack_height;
                arr.size = current->variables[i].size;
                arr.count = current->variables[i].count;
                
                return arr;
            }
        }
        current = current->parent;
    }
    arr.offset = -1;
    arr.size = -1;
    
    return arr;
}

VarAddress lookup_variable_in_current_scope(const char * name) {
    int i;
    VarAddress var;

    for (i = 0; i < current_scope->variables_count; i++) {
        if (!strcmp(current_scope->variables[i].name, name) && !current_scope->variables[i].is_array) {
            var.offset = current_stack_height - current_scope->variables[i].stack_height;
            var.size = current_scope->variables[i].size;
            var.count = current_scope->variables[i].count;
            
            return var;
        }
    }
    var.offset = -1;
    var.size = -1;
    
    return var;
}

VarAddress lookup_array_in_current_scope(const char * name) {
    int i;
    VarAddress arr;

    for (i = 0; i < current_scope->variables_count; i++) {
        if (!strcmp(current_scope->variables[i].name, name) && current_scope->variables[i].is_array) {
            arr.offset = current_stack_height - current_scope->variables[i].stack_height;
            arr.size = current_scope->variables[i].size;
            arr.count = current_scope->variables[i].count;
            
            return arr;
        }
    }
    arr.offset = -1;
    arr.size = -1;
    
    return arr;
}

void set_array_stack_height_in_current_scope(const char * name, int stack_height) {
	int i;
	for (i = 0; i < current_scope->variables_count; i++) {
        if (!strcmp(current_scope->variables[i].name, name) && current_scope->variables[i].is_array) {
        	current_scope->variables[i].stack_height = stack_height;
        	return;
        }
    }
}

/***FUNCTIONS***/
FunDef define_function(VARTYPE type, const char * name) {
    FunDef function;

    function.name = name;
    function.type = type;
    function.param_count = 0;
    functions[functions_count] = function;
    set_current_function(&(functions[functions_count++]));

    return function;
}

FunDef lookup_function(const char* name) {
    int i;
    FunDef dummy;
    for (i = 0; i < functions_count; i++) {
        if (!strcmp(functions[i].name, name)) {
            return functions[i];
        }
    }
    dummy.name = NULL;
    return dummy;
}

void set_current_function(FunDef * fun) {
    current_function = fun;
}

void add_parameter(VARTYPE type, int is_array, const char * name) {
    current_function->param_type[current_function->param_count] = type;
    current_function->is_array[current_function->param_count] = is_array;
    current_function->param_name[current_function->param_count++] = name;
    if (is_array)
    	add_array(type, name, 0, 1);
    else
    	add_variable(type, name);

}

void copy_parameters() {
    int i;
    VarAddress var;

    for (i = 0; i < current_function->param_count; i++) {
    	if (current_function->is_array[i]) {
        	var = lookup_array_in_current_scope(current_function->param_name[i]);
        } else {
        	var = lookup_variable_in_current_scope(current_function->param_name[i]);
		    if (var.size == 1)
		        add_instruction(create_instruction_offset(SB, 4 + i, sp, 0, var.offset));
		    else
		        add_instruction(create_instruction_offset(SW, 4 + i, sp, 0, var.offset));
        }
    }
}
