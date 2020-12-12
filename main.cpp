#include <stdio.h>

#include "differentiator.h"
#include <time.h>


void format_line(char* line){
    char* left  = line;
    char* right = line;
    while(*right != 0){
        if(*right != ' ' && *right != '\n' && *right != '\t'){
            *left = *right;
            left++;
        }
        right++;
    }
    *left = '\0';
}



int main(){
    srand(time(NULL));
    const int max_expr = 100;
    char* expr = (char*)calloc(max_expr + 1, sizeof(char));
    FILE* fp = fopen("input.txt", "r");
    fgets(expr, max_expr, fp);
    format_line(expr);
    fclose(fp);

    Tree tree = {};
    
    tree.constructor();
    tree.init(expr);
    if(tree.check()){
        tree.dump("results.pdf");
        tree.generate_article();
    }

    free(expr);
    tree.destructor();
}