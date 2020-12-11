#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int max_function_name = 4;
const int variables_count = 26;

enum Node_Type{
    FUNCTION,
    OPERATOR,
    VALUE,
    VARIABLE
};



#define FUNCTION(name, num, code)\
    _ ## name = num,

enum Function_Types{
    #include "functions.h"
    _error
};
#undef FUNCTION




enum Operator_Types{
    PLUS = '+',
    MINUS = '-',
    MUL = '*',
    DIV = '/',
    POW = '^'
};

struct Node{
    Node* left;
    Node* right;
    Node_Type type;
    double value;
    void constructor(Node_Type type, double value);

};



struct Tree{
   

    void constructor();

    void init(const char* line);

    bool check();

    double evaluate();

    void simplificate_expression(FILE* fp = NULL);

    void fill_variables(const char* variables, ...);

    void destructor();

    void print_tex_expression(FILE* fp);

    void derivative(Tree* expression, const char var);

    void copy(Tree* expression);

    void taylor(Tree* expression, const char var, int count, double point);

    void dump(const char* file);

    void print_tex_plot_x(FILE* fp, double left, double right, double step);

    void print_tex_plot_x(Tree* expression, FILE* fp, double left, double right, double step);

    void print_ful_derivative(FILE* fp);

    void generate_article();

private:
    Node* root;
    double* variables;
    char wrong_symbol;
    const char* curr_pointer;
    Node* getG(const char* line);
    Node* getExpr();
    Node* getNum();
    Node* getTerm();
    Node* getPow();
    Node* getPrim();
    Node* getVar();
    Node* getFunc();

    void errors_processing();

    Node* simplify_plus(Node* node, FILE* fp = NULL);
    Node* simplify_minus(Node* node, FILE* fp = NULL);
    Node* simplify_mul(Node* node, FILE* fp = NULL);
    Node* simplify_div(Node* node, FILE* fp = NULL);
    Node* simplify_pow(Node* node, FILE* fp = NULL);
 


// G::= Expr\0 
// Expr::= Term {[+-]Term}*
// Term::= Pow  {[*/]Pow }*
// Pow ::= Prim {^   Prim}*
// Prim::= (Expr) | Num | Var | Func(Expr)
// Num ::= [-+]? [0-9]+ [.]? [0-9]* | p | e
// Var ::= [a-d] | [f-o] | [q-z]
// Func::= [a-z]+




    void require(char c);

    Node* recursive_simplificate(Node* node, FILE* fp = NULL);
    Node* recursive_copy(Node* node);
    double recursive_evaluate(Node* node);
    void do_dump(Node* node, FILE* fp);
    Node* recursive_derivative(Node* node, int var);

    void recursive_tex_print(Node* node, FILE* fp);
    void print_coordinates_x(FILE* fp, double left, double right, double step); 
    void print_before(Node* node, FILE* fp);
    void print_after(Node* node, FILE* fp);

    void recursive_delete(Node* node);

};


    void tree_load_tree(Tree* tree, const char* file);






