#include <stdio.h>
#include "latex_dump.h"
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
    fgets(expr, max_expr, stdin);
    format_line(expr);

    Tree tree = {};
    Tree der = {};
    tree.constructor();
    tree.init(expr);
    if(tree.check()){
        tree.dump("results.pdf");
        tree.generate_article();
    }

    free(expr);

    // der.derivative(&tree, 'x');
    // der.simplificate_expression();

    // FILE* fp = begining_of_the_article("tex.tex");

    // tree.print_tex_expression(fp);
    // tree.print_tex_plot_x(fp, -30, 30, 0.1);
    // fprintf(fp, "\n\n It was awful, but then some magic happend and it became awesome!\n\n");
    
    // tree.simplificate_expression();

    // tree.print_tex_expression(fp);
    // der.dump("check.pdf");
    // der.print_tex_expression(fp);
    // der.simplificate_expression();
    // der.print_tex_expression(fp);

    // end_of_the_article(fp);
    // system("pdflatex tex.tex");

    // //printf("%lg\n", evaluate(&tree));

    // //fill_variables(&tree, "x", 10.);
    // //printf("%lg", evaluate(&tree));

    // der.destructor();
    tree.destructor();
}