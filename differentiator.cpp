#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "differentiator.h"
#include <ctype.h>
#include <sys/stat.h>
#include "own_functions.h"
#include "begin.h"
#include "end.h"
#include <time.h>
void Node::constructor(Node_Type type, double value){
    this->type = type;
    this->value = value;
}

Node* node_New(Node_Type type, double value){
    Node* res = (Node*)calloc(1, sizeof(Node));
    res->constructor(type, value);
    return res;
}



void Tree::constructor(){
    this->variables = (double*)calloc(variables_count, sizeof(double));
}

bool Tree::check(){
    if(this->wrong_symbol == -1){
        return true;
    }else{
        return false;
    }
}

void Tree::init(const char* line){
    recursive_delete(this->root);
    this->wrong_symbol = -1;
    this->curr_pointer = line;
    this->root = getG(line);
}


void Tree::require(char c){
    if(c != *(this->curr_pointer)){
        this->wrong_symbol = *(this->curr_pointer);
    }
}

void Tree::errors_processing(){
    printf("%c (%d)- i don't know what to do with this symbol\n", this->wrong_symbol, this->wrong_symbol);
    printf("%s | full line after error\n", this->curr_pointer);
}

Node* Tree::getG(const char* line){
    this->curr_pointer = line;
    Node* val = Tree::getExpr();
    require('\0');
    if((this->wrong_symbol) != -1){
        errors_processing();
    }
    return val;
}


Node* Tree::getExpr(){
    Node* val = Tree::getTerm();

    while(*(this->curr_pointer) == '+' || *(this->curr_pointer) == '-'){
        char op = *(this->curr_pointer);
        (this->curr_pointer)++
        ;
        Node* right = getTerm();
        Node* left = val;
        
        val = (Node*)calloc(1, sizeof(Node));
        val->left = left;
        val->right = right;
        
        if(op == '+'){
            val->constructor(OPERATOR, PLUS);
        }else{
            val->constructor(OPERATOR, MINUS);
        }
    }
    return val;
}

Node* Tree::getTerm(){
    Node* val = getPow();

    while(*(this->curr_pointer) == '*' || *(this->curr_pointer) == '/'){
        char op = *(this->curr_pointer);
        (this->curr_pointer)++;
        
        Node* right = getPow();
        Node* left = val;
        
        val = (Node*)calloc(1, sizeof(Node));
        val->left = left;
        val->right = right;
        
        if(op == '*'){
            val->constructor(OPERATOR, MUL);
        }else{
            val->constructor(OPERATOR, DIV);
        }
    }
    return val;
}



Node* Tree::getPow(){
    Node* val = getPrim();

    while(*(this->curr_pointer) == '^'){
        char op = *(this->curr_pointer);
        (this->curr_pointer)++;
        
        Node* right = getPrim();
        Node* left = val;
        
        val = node_New(OPERATOR, POW);
        val->left = left;
        val->right = right;
        
    }
    return val;
}



Node* Tree::getPrim(){
    if(*(this->curr_pointer) == '('){
        (this->curr_pointer)++;
        Node* val = getExpr();
        require(')');
        (this->curr_pointer)++;
        return val;
    }else{
        if(!isalpha(*(this->curr_pointer))){
            return getNum();
        }else{
        
            if(isalpha(*(this->curr_pointer + 1))){
                return getFunc();
            }else{
                if(*(this->curr_pointer) == 'e'){
                    Node* res = node_New(VALUE, M_E);

                    (this->curr_pointer)++;
                    return res;
                }

                if(*(this->curr_pointer) == 'p'){
                    Node* res = node_New(VALUE, M_PI);

                    (this->curr_pointer)++;
                    return res;
                }
            
                return getVar();
            }
        
        }
    }
}

Node* Tree::getNum(){
    double val = 0;
    int count = 0;
           
    sscanf(this->curr_pointer, "%lg%n", &val, &count);

    this->curr_pointer+=(count);
    Node* res = node_New(VALUE, val);
    
    return res;
}

bool is_name_of_var(char c){
    if(c != 'e' && c != 'p' && c >= 'a' && c <= 'z'){
        return true;
    }else{
        return false;
    }
}


Node* Tree::getVar(){
    if(!is_name_of_var(*(this->curr_pointer))){
        return NULL;
    }

    Node* res = node_New(VARIABLE, *(this->curr_pointer) - 'a');

    (this->curr_pointer)++;

    
                
    return res;
}

int func_len(const char* s){
    int res = 0;
    while(isalpha(*s)){
        s++;
        res++;
    }
    return res;
}

Function_Types get_func_type(const char* s, int size){

    #define FUNCTION(name, num, code)           \
        if(strncmp(s, #name, size) == 0){       \
            return (Function_Types)num;         \
        }

    #include "functions.h"

    #undef FUNCTION

    return _error;
}



Node* Tree::getFunc(){
    int len = func_len(this->curr_pointer);
    Function_Types func = get_func_type(this->curr_pointer, len);
    if(func == _error){
        return NULL;
    }

    Node* res = node_New(FUNCTION, func);

    (this->curr_pointer) += len;
    res->right = getPrim();
    return res;
}



void Tree::recursive_delete(Node* node){
    if(node == NULL){
        return;
    }
    if(node->left != NULL){
        recursive_delete(node->left);
    }
    if(node->right != NULL){
        recursive_delete(node->right);
    }
    free(node);
}


void Tree::destructor(){
    if(this->root != NULL){
        recursive_delete(this->root);
        this->root = NULL;
    }
    if(this->variables != NULL){
        free(this->variables);
        this->variables = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////COPY BLOCK////////////////////////////////////////////////


Node* Tree::recursive_copy(Node* node){
    if(node == NULL){
        return NULL;
    }
    Node* res = (Node*)calloc(1, sizeof(Node));
    res->type = node->type;
    res->value = node->value;
    res->left = recursive_copy(node->left);
    res->right = recursive_copy(node->right);
    return res;
}


void Tree::copy(Tree* expression){
    recursive_delete(this->root);
    this->root = recursive_copy(expression->root);
}


/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////DUMP BLOCK////////////////////////////////////////////////

*/
void Tree::do_dump(Node* node, FILE* fp){
    fprintf(fp, "%ld[shape=record label = \"type = %d\\n", (size_t)node, node->type);

    if(node->type == OPERATOR){
        fprintf(fp, "value = %c", (int)node->value);
    }else{
        fprintf(fp, "value = %lg", node->value);

    }

    fprintf(fp, "|{left = %p|right = %p|addr = %p}\"]\n",node->left, node->right,  node);


    if(node->left != NULL){

        fprintf(fp, "%ld->%ld\n", (size_t)node, (size_t)node->left);        
        do_dump(node->left, fp);
    }
    
    if(node->right != NULL){
        fprintf(fp, "%ld->%ld\n", (size_t)node, (size_t)node->right);        
        do_dump(node->right, fp);
    }

}


void Tree::dump(const char* file){
    if(this->root == NULL){
        printf("Nullptr on root\n");
        return;
    }

    FILE* fp = fopen("res.gv", "w");
    
    fprintf(fp,"digraph G{\n");
    
    do_dump(this->root, fp);
    
    fprintf(fp,"}\n");
    fclose(fp);


    char* str = (char*)calloc(23+strlen(file), sizeof(char));
    strcat(str, "dot -Tpdf res.gv -o");
    strcat(str, file);
    system(str);
    free(str);
    remove("res.gv");
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

void Tree::fill_variables(const char* variables, ...){
    va_list args;
    va_start(args, variables);
    while(*variables != 0){
        double curr = va_arg(args, double);
        this->variables[*variables - 'a'] = curr;
        variables++;
    }
    
    va_end(args);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////EVALUATE BLOCK////////////////////////////////////////

double Tree::recursive_evaluate(Node* node){
  
    if(node == NULL){
        return -111;
    }

    if(node->type == VALUE){
        return node->value;
    }

    if(node->type == VARIABLE){
        return this->variables[(int)(node->value)];
    }

    double left = recursive_evaluate(node->left);
    double right = recursive_evaluate(node->right);

    if(node->type == FUNCTION){
        #define FUNCTION(name, num, code)                   \
            case num:{                                      \
                return name(right);                         \
            }

        switch((char)node->value){
            #include "functions.h"
        }    

        #undef FUNCTION
    }

    if(node->type == OPERATOR){
        switch((char)node->value){
            case '+':{
                return left + right;
            }

            case '-':{
                return left - right;
            }

            case '*':{
                return left * right;
            }

            case '/':{
                return left / right;
            }

            case '^':{
                return pow(left,right);
            }



        }
    }
    return -100;

}

double Tree::evaluate(){
    assert(this->root != NULL);
    return recursive_evaluate(this->root);
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////DERIVATIVE BLOCK////////////////////////////////////////

#define dLeft                                   \
    recursive_derivative(node->left, var)

#define dRight                                  \
    recursive_derivative(node->right, var)

#define cLeft                                   \
    recursive_copy(node->left)

#define cRight                                  \
    recursive_copy(node->right)    

#define Curr                                    \
    recursive_copy(node)    


static Node* _PLUS(Node* left, Node* right){
    Node* res = node_New(OPERATOR, PLUS);
    res->left = left;
    res->right = right;
    return res;
}

static Node* _MINUS(Node* left, Node* right){
    Node* res = node_New(OPERATOR, MINUS);
    res->left = left;
    res->right = right;
    return res;

}

static Node* _MUL(Node* left, Node* right){
    Node* res = node_New(OPERATOR, MUL);
    res->left = left;
    res->right = right;
    return res;
}

static Node* _DIV(Node* left, Node* right){
    Node* res = node_New(OPERATOR, DIV);

    res->left = left;
    res->right = right;
    return res;
}

static Node* _POW(Node* left, Node* right){
    Node* res = node_New(OPERATOR, POW);

    res->left = left;
    res->right = right;
    return res;
}

static Node* _NODE_BY_VAL(double val){
    Node* res = node_New(VALUE, val);

    return res;
}

static Node* _NODE_BY_VAR(const char var){
    Node* res = node_New(VARIABLE, var - 'a');
    return res;
}

#define FUNCTION(name, num, code)                       \
    static Node* node##name (Node* node){               \
        Node* res = node_New(FUNCTION, _##name);       \
        res->right = node;                              \
        return res;                                     \
    }

#include "functions.h"

#undef FUNCTION


Node* Tree::recursive_derivative(Node* node, int var){
    switch(node->type){
        case VALUE:{
            return _NODE_BY_VAL(0);
        }

        case VARIABLE:{
            if(node->value == var){
                return _NODE_BY_VAL(1);
            }else{
                return _NODE_BY_VAL(0);
            }
        }

        case OPERATOR:{
            switch((int)node->value){
                case PLUS:{
                    return _PLUS(dLeft, dRight);
                }

                case MINUS:{
                    return _MINUS(dLeft, dRight);
                }

                case MUL:{
                    return _PLUS(_MUL(dLeft, cRight), _MUL(cLeft, dRight));
                }
                case DIV:{
                    return _DIV(_MINUS(_MUL(dLeft, cRight), _MUL(cLeft, dRight)), _POW(cRight, _NODE_BY_VAL(2)));
                }

                case POW:{
                    return _MUL(_PLUS(_MUL(cRight, dLeft), _MUL(cLeft ,_MUL(nodelog(cLeft),dRight))), _POW(cLeft, _MINUS(cRight, _NODE_BY_VAL(1))));
                }
            }


            break;
        }

        case FUNCTION:{
            
            #define FUNCTION(name, num, code)   \
                case num: code                  
                                                
            switch((int)node->value){
                #include "functions.h"
            }
            #undef FUNCTION

            break;
        }

    }

    return NULL;

}



void Tree::derivative(Tree* expression, const char var){
    if(var == 'e' || var == 'p' || var < 'a' || var > 'z'){
        printf("wrong variable %c \n", var);
        return;
    }

    Node* res = recursive_derivative(expression->root, var - 'a');

    recursive_delete(this->root);
    this->root = res;

}

void Tree::taylor(Tree* expression, const char var, int count, double point){
    assert(count > 0);
    assert(expression != NULL);
    recursive_delete(this->root);

    Tree tmp = {};
    tmp.constructor();
    tmp.copy(expression);


    char* variable = (char*)calloc(2, sizeof(char));
    *variable = var;
    tmp.fill_variables(variable, point);

    this->root = node_New(VALUE, tmp.evaluate());
    double fact = 1;

    for(int i = 1; i <= count; i++){
        fact*=i;
        tmp.derivative(&tmp, var);
        tmp.simplificate_expression();
        double res = tmp.evaluate();
        if(res > 0){
            this->root = _PLUS (this->root, _MUL(_DIV(_NODE_BY_VAL( res),_NODE_BY_VAL(fact)), _POW(_MINUS(_NODE_BY_VAR(var), _NODE_BY_VAL(point)), _NODE_BY_VAL(i))));
        }else{
            this->root = _MINUS(this->root, _MUL(_DIV(_NODE_BY_VAL(-res),_NODE_BY_VAL(fact)), _POW(_MINUS(_NODE_BY_VAR(var), _NODE_BY_VAL(point)), _NODE_BY_VAL(i))));
        }
    }
    tmp.destructor();

    free(variable);
    this->simplificate_expression();
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////SIMPLIFICATE BLOCK////////////////////////////////////////

const double precision = 1e-8;
bool double_cmp(double a, double b){
    if(fabs(a-b) < precision){
        return true;
    }else{
        return false;
    }
}


bool is_int(double x){
    return double_cmp(x, (double)((int)x));
}


int find_gcd(int a, int b){
    if(a < 0){
        a*=(-1);
    }
    if(b < 0){
        b*=(-1);
    }

    while(a != b){
        if(a > b){
            a -= b;
        }else{
            b -= a;
        }
    }
    return a;
}


bool value_node_cmp(Node* left, Node* right){
    if(left->type == VALUE && right->type == VALUE && abs(left->value - right->value) < precision){
        return true;
    }else{
        return false;
    }
}

void Tree::print_before(Node* node, FILE* fp){
    if(fp != NULL){
        int d = rand()%begin_count;
        fprintf(fp, "\\begin{center}\n%s", begin[d]);
        if(d == 7){
            fprintf(fp, "\\cite{necro}");
        }
        fprintf(fp, "\n\n$g(x) = ");
        recursive_tex_print(node, fp);
        fprintf(fp, "$\n\\end{center}");
    }
}


void Tree::print_after(Node* node, FILE* fp){
    if(fp != NULL){
        fprintf(fp, "\\begin{center}\n%s\n\n$g(x) = ", end[rand()%end_count]);
        recursive_tex_print(node, fp);
        fprintf(fp, "$\\end{center}\n\n\n\n\n\n");
    }
}


bool isValue(Node* node){
    if(node->type == VALUE){
        return true;
    }else{
        return false;
    }
}

bool isVariable(Node* node){
    if(node->type == VARIABLE){
        return true;
    }else{
        return false;
    }
}

bool isOperator(Node* node){
    if(node->type == OPERATOR){
        return true;
    }else{
        return false;
    }
}

bool isZero(Node* node){
    if(double_cmp(node->value, 0)){
        return true;
    }else{
        return false;
    }
}

bool isOne(Node* node){
    if(double_cmp(node->value, 1)){
        return true;
    }else{
        return false;
    }
}

Node* Tree::simplify_plus(Node* node, FILE* fp){
    if(isValue(node->left) && isZero(node->left)){
        print_before(node, fp);

        free(node->left);
        Node* res = node->right;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(isValue(node->right) && isZero(node->right)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(value_node_cmp(node->left, node->right)){
        print_before(node, fp);

        recursive_delete(node->right->right);
        recursive_delete(node->right->left);
        node->right->type = VALUE;
        node->right->value = 2;
        node->value = MUL;

        Node* res = recursive_simplificate(node);

        print_after(res, fp);
        return res;
    }
    return node;
}



Node* Tree::simplify_mul(Node* node, FILE* fp){
    if(isValue(node->left) && isOne(node->left)){
        print_before(node, fp);

        free(node->left);
        Node* res = node->right;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(isValue(node->right) && isOne(node->right)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }
    
    if(isValue(node->left) && isZero(node->left)){
        print_before(node, fp);

        recursive_delete(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }

    if(isValue(node->right) && isZero(node->right)){
        print_before(node, fp);

        recursive_delete(node->left);
        Node* res = node->right;
        free(node);

        print_after(res, fp);
        return res;
    }

    if(value_node_cmp(node->left, node->right)){
        print_before(node, fp);

        recursive_delete(node->right->right);
        recursive_delete(node->right->left);
        node->right->type = VALUE;
        node->right->value = 2;
        node->value = POW;

    
        print_after(node, fp);
        return recursive_simplificate(node, fp);;
    }

    if(isVariable(node->left ) &&
       isVariable(node->right) &&
       double_cmp(node->left->value, node->right->value)){
            print_before(node, fp);

            recursive_delete(node->right->right);
            recursive_delete(node->right->left);
            node->right->type = VALUE;
            node->right->value = 2;
            node->value = POW;
            
            print_after(node, fp);
            return recursive_simplificate(node, fp);;
    }

    if(isVariable(node->right) && isOperator(node->left) &&
      node->left->value == POW && isVariable(node->left->left)){
        if(double_cmp(node->right->value, node->left->left->value)){
            print_before(node, fp);

            Node* res = node->left;
            node->left = res->right;
            res->right = node;
            res->right->type = OPERATOR;
            res->right->value = PLUS;
            res->right->right->type = VALUE;
            res->right->right->value = 1;

            print_after(res, fp);
            return recursive_simplificate(res, fp);;
        }
    }


    if(isVariable(node->left) && isOperator(node->right) &&
    node->right->value == POW && isVariable(node->right->left)){
        if(double_cmp(node->left->value, node->right->left->value)){
            print_before(node, fp);

            Node* res = node->right;
            node->right = res->right;
            res->right = node;
            res->right->type = OPERATOR;
            res->right->value = PLUS;
            res->right->right->type = VALUE;
            res->right->right->value = 1;
            
            print_after(res, fp);
            return recursive_simplificate(res, fp);;
    
        }
    }
    
    return node;

}

Node* Tree::simplify_minus(Node* node, FILE* fp){
    if(isValue(node->right) && isZero(node->right)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(isValue(node->left) && isZero(node->left)){
        print_before(node, fp);

        node->left->value = -1;
        node->value= MUL;

        print_after(node, fp);
        return node;
    }
    return node;
}

Node* Tree::simplify_div(Node* node, FILE* fp){
    if(isValue(node->left) && isZero(node->left)){
        print_before(node, fp);

        recursive_delete(node->right);
        Node* res = node->left;
        free(node);
        
        print_after(res, fp);
        return res;
    }
    if(isValue(node->left ) &&
       isValue(node->right) && 
       is_int(node->left ->value)  && 
       is_int(node->right->value)){

            int left = round(node->left->value);
            int right = round(node->right->value);
            int g = find_gcd(left, right);
            if(g != 1){
                print_before(node, fp);

            }
            node->left ->value = left /g;
            node->right->value = right/g;
            if(g != 1){
                print_after(node, fp);
                return simplify_div(node, fp);
            }
    }

    if(isValue(node->right) && isOne(node->right)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }

    if(value_node_cmp(node->left, node->right)){
        print_before(node, fp);

        recursive_delete(node->right);
        recursive_delete(node->left);
        node->value = 1;
        node->type = VALUE;
        
        print_after(node, fp);
        return node;
    }
    return node;
}


Node* Tree::simplify_pow(Node* node, FILE* fp){
    if(isValue(node->right) && isOne(node->right)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);
    
        print_after(res, fp);
        return res;
    }
    if(isValue(node->right)&& isZero(node->right)){
        print_before(node, fp);

        free(node->left);
        Node* res = node->right;
        res->value = 1;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(isValue(node->left) && isOne(node->left)){
        print_before(node, fp);

        free(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }
    if(isValue(node->left) && isZero(node->left)){
        print_before(node, fp);

        recursive_delete(node->right);
        Node* res = node->left;
        free(node);

        print_after(res, fp);
        return res;
    }
    return node;
}

Node* Tree::recursive_simplificate(Node* node, FILE* fp){
    if(node == NULL || node->type == VALUE || node->type == VARIABLE){
        return node;
    }

    node->left  = recursive_simplificate(node->left , fp);
    node->right = recursive_simplificate(node->right, fp);

    switch(node->type){
        case OPERATOR:{
           
            if(isValue(node->right) && isValue(node->left) && node->value != DIV &&
               !double_cmp(node->right->value, M_PI) && !double_cmp(node->right->value, M_E) &&
               !double_cmp(node->left ->value, M_PI) && !double_cmp(node->left ->value, M_E)){
                print_before(node, fp);

                double res = recursive_evaluate(node);

                free(node->left );
                free(node->right);
                
                node->left  = NULL;
                node->right = NULL;
                node->value = res;
                node->type = VALUE;
                
                print_after(node, fp);
                return node;
            }

            switch((int)node->value){
                case PLUS:{
                    return simplify_plus(node, fp);
                }
                case MINUS:{
                    return simplify_minus(node, fp);
                }
                
                case MUL:{
                    return simplify_mul(node, fp);
                }
                
                case DIV:{
                    return simplify_div(node, fp);
                }
                
                case POW:{
                    return simplify_pow(node, fp);
                }
            }
            
            break;
        }
    }
    return node;
}


void Tree::simplificate_expression(FILE* fp){
    this->root = recursive_simplificate(this->root, fp);
}
