#include <math.h>
#include <assert.h>

#include <stdio.h>


#include "latex_dump.h"

FILE* begining_of_the_article(const char* file){
    assert(file != NULL);
    FILE* fp = fopen(file, "w");
    fprintf(fp,"\\documentclass[]{article}\n"
               "\\usepackage[T1, T2A]{fontenc}\n"
               "\\usepackage{hyperref}\n"
               "\\usepackage[utf8]{inputenc}\n"
               "\\usepackage[english,russian]{babel}\n"
               "\\usepackage{indentfirst}\n"
            //    "\\usepackage[left=1cm,right=1cm,top=2cm,bottom=2cm,bindingoffset=0cm]{geometry}"
               "\\usepackage{misccorr}\n"
               "\\usepackage{graphicx}\n"
               "\\usepackage{pgfplots}"
               "\\usepackage{amsmath}\n"
               "\\begin{document}\n"
               "\\begin{titlepage}\n"     
               "\\newpage \n"           
               "\\begin{center}\n" 
               "{\\bfseries Министерство образования и науки Российской Федерации \\\\\n"
               "Московский физико-технический институт}\n"
               "\\vspace{1cm}\n"
               "%МОСКОВСКИЙ ФИЗИКО-ТЕХНИЧЕСКИЙ ИНСТИТУТ \\\\* \n"                     
               "%НАЦИОНАЛЬНЫЙ ИССЛЕДОВАТЕЛЬСКИЙ УНИВЕРСИТЕТ \\\\* \n"                         
               "\\end{center}\n"
                "\\begin{center}\n" 
               "%{КАФЕДРА ОТДЫХА И ПРОКРАСТИНАЦИИ}\n"            
               "Кафедра отдыха и прокрастинации \n"                       
               "\\end{center}\n"
               "\\vspace{6em}\n"               
               "%\\vspace{2.0em}\n"                            
               "\\vspace{1.2em}\n"              
               "%\\textsc{\\textbf{}} \n"
               "\\vspace{5em}\n"                                         
               "\\begin{center}\n"
               "\\Large{ "          
               "Исследование функций под воздейстивем факторов разрушающих организм}\n"          
               " \\end{center}\n"
               "\\vspace{6em}\n"
               "%\\begin{center}\n"
               "%\\begin{tabbing}\n"
               "%\\begin{center}\n"
               "%\\end{tabbing}\n"
               "%\\end{center}\n"                 
               "\\vspace{\\fill}\n"          
               "\\begin{center}\n"
               "Москва 2020 \n"              
               "\\end{center}\n"                                                                                    
               "\\end{titlepage}\n"
	           "\\title\n\n\n\n" 
               );

    return fp;
}

void end_of_the_article(FILE* fp){
    assert(fp != NULL);
    fprintf(fp,     "\\begin{thebibliography}{3}"
                    "\\bibitem{matan}"
                    "Л.Д Кудрявцев и Co.: Сборник задач по математическому анализу\n"
                    "\\bibitem{Zadav}"
                    "Хз кто авторы: Задавальник на 1 семестр 2020 года\n"
                    "\\bibitem{github}"
                    "\\href{https://github.com/wandrll/differentiator}{https://github.com/wandrll/differentiator}\n"
                    "\\bibitem{necro}"
                    "Некрономикон, издание 3, исправленно, адаптированное для детей от 3х лет\n"
                    "\\end{thebibliography}\n"
                    "\\end{document}\n");
    fclose(fp);
}






void print_function(int number, FILE* fp){
    #define FUNCTION(name, num, code)   \
        case num:{                      \
            fprintf(fp, #name);         \
            return;                     \
        }  

    switch(number){
        #include "functions.h"
    }
    #undef FUNCTION

    fprintf(fp, "ERROR(%d)", number);    
}



bool check_node(Node* node){
    if(node->type == OPERATOR && node->value != MUL && node->value != POW && node->value != DIV){
        return true;
    }else{
        return false;
    }
}


void Tree::recursive_tex_print(Node* node, FILE* fp){
    
    switch(node->type){
        case FUNCTION:{
            print_function(node->value, fp);
            fprintf(fp, "\\left(");
            recursive_tex_print(node->right, fp);
            fprintf(fp, "\\right)");
            break;
        }

        case OPERATOR:{
             switch((char)node->value){
                case '+':{
                    recursive_tex_print(node->left, fp);
                    fprintf(fp, "+");
                    recursive_tex_print(node->right, fp);
                    break;
                }

                case '-':{
                  
                    recursive_tex_print(node->left, fp);
                    fprintf(fp, "-");

                    if(check_node(node->right)){
                        fprintf(fp, "\\left(");
                        recursive_tex_print(node->right, fp);
                        fprintf(fp, "\\right)");
                    }else{
                        recursive_tex_print(node->right, fp);

                    }

                    break;
                }

                case '*':{
                    if(check_node(node->left)){
                        fprintf(fp, "\\left(");
                        recursive_tex_print(node->left, fp);
                        fprintf(fp, "\\right)");
                    }else{
                        recursive_tex_print(node->left, fp);

                    }
                    
                    fprintf(fp, "\\cdot ");

                    if(check_node(node->right)){
                        fprintf(fp, "\\left(");
                        recursive_tex_print(node->right, fp);
                        fprintf(fp, "\\right)");
                    }else{
                        recursive_tex_print(node->right, fp);

                    }
                    break;
                }

                case '/':{

                    fprintf(fp, "\\frac{");
                    recursive_tex_print(node->left, fp);

                    fprintf(fp, "}{");

                    recursive_tex_print(node->right, fp);
                    fprintf(fp, "}");

                    break;
                }

                case '^':{
                    if(node->left->type != VALUE && node->left->type != VARIABLE){
                        fprintf(fp, "\\left(");
                        recursive_tex_print(node->left, fp);
                        fprintf(fp, "\\right)");
                    }else{
                        recursive_tex_print(node->left, fp);

                    }

                    fprintf(fp, "^{");

                    recursive_tex_print(node->right, fp);
                    fprintf(fp, "}");

                    break;
                }



            }
            break;
        }

        case VALUE:{
            if(node->value == M_PI){
                fprintf(fp, "\\pi ");
                break;
            }
            if(node->value == M_E){
                fprintf(fp, "e");
                break;
            }
            if(node->value < 0){
                fprintf(fp, "\\left(");
            }
            fprintf(fp, "%lg", node->value);
            if(node->value < 0){
                fprintf(fp, "\\right)");
            }
            break;
        }

        case VARIABLE:{
            fprintf(fp, "%c", (int)node->value + 'a');
            break;
        }

    }

}


void Tree::print_tex_expression(FILE* fp){
    assert(fp != NULL);
    assert(this->root != NULL);

    fprintf(fp, "\n$");

    recursive_tex_print(this->root, fp);

    fprintf(fp, "$\n");
}



void Tree::print_coordinates_x(FILE* fp, double left, double right, double step){
    while(left <= right){
        this->fill_variables("x", left);
        fprintf(fp, "(%lg , %lg)\n", left, this->evaluate());
        left+=step;
    }
} 

void Tree::print_tex_plot_x(FILE* fp, double left, double right, double step){
    assert(fp != NULL);
    fprintf(fp, "\\begin{center}\n"
                "\\begin{tikzpicture}\n"
                "\\begin{axis}[\n"
                "axis lines=middle,\n"
                "restrict y to domain=-100:100,\n"
                "	xlabel = {$x$},\n"
                "	ylabel = {$y$},\n"
                "	minor tick num = 2\n"
                "]\n"
                "\\addplot[mark=none] coordinates{\n");
    print_coordinates_x(fp, left, right, step); 

    fprintf(fp, "\n};\n"
                "\\end{axis}\n"
                "\\end{tikzpicture}\n"
                "\\end{center}\n");
}

void Tree::print_tex_plot_x(Tree* expression, FILE* fp, double left, double right, double step){
    assert(fp != NULL);
    fprintf(fp, "\\begin{center}\n"
                "\\begin{tikzpicture}\n"
                "\\begin{axis}[\n"
                "axis lines=middle,\n"
                    "restrict y to domain=-100:100,\n"
                "	xlabel = {$x$},\n"
                "	ylabel = {$y$},\n"
                "	minor tick num = 2\n"
                "]\n"
                "\\legend{" 
                "	First," 
                "	Second, "
                "};"
                "\\addplot[mark=none, blue] coordinates{\n");
    this->print_coordinates_x(fp, left, right, step); 

    fprintf(fp, "\n};\n"
                "\\addplot[mark=none, red] coordinates{\n");
    expression->print_coordinates_x(fp, left, right, step); 

    fprintf(fp, "\n};\n"
                "\\end{axis}\n"
                "\\end{tikzpicture}\n"
                "\\end{center}\n");
}

void recursive_find_var(Node* node, bool* res){
    assert(res != NULL);
    if(node == NULL){
        return;
    }
    recursive_find_var(node->left, res);
    recursive_find_var(node->right, res);

    if(node->type == VARIABLE){
        res[(int)node->value] = true;
    }
}



void Tree::print_ful_derivative(FILE* fp){
    bool* check_var = (bool*)calloc(26, sizeof(bool));
    recursive_find_var(this->root, check_var);
    fprintf(fp, "$df(");
    int curr = 0;
    while( curr < 26 && !check_var[curr]){
        curr++;
    }
    if(curr == 26){
        fprintf(fp, ") = 0$\n\n");
        free(check_var);
        return;
    }else{
        fprintf(fp, "%c", curr + 'a');
    }
    for(int i = curr+1; i < 26; i++){
        if(check_var[i]){
            fprintf(fp, ", %c", i+'a');
        }
    }
    fprintf(fp, ")$ = ");

    Tree tmp = {};
    tmp.constructor();

    for(int i = 0; i < 26; i++){
        if(check_var[i]){
            fprintf(fp, "(");
            tmp.derivative(this, i + 'a');
            tmp.simplificate_expression();
            tmp.print_tex_expression(fp);
            fprintf(fp, " $) \\cdot d%c$ + ", i+'a');
        }
    }
    fprintf(fp, "0\n\n");
    tmp.destructor();
    free(check_var);
}

void Tree::generate_article(){

    FILE* fp = begining_of_the_article("Article.tex");
    fprintf(fp,"\\section{}");
    fprintf(fp, "\\par Добро пожаловать в эту статью дорогой читатель, в этой статье мы поизучаем функцию\n\n f(x) = ");
    this->print_tex_expression(fp);

    fprintf(fp, "\n\\parДавайте для начала немного упростим это выражение\n\n");
    this->simplificate_expression(fp);

    fprintf(fp, "\n\\parИтоговое выражение будет иметь слелующий вид:\n\n $f(x) = $");
    this->print_tex_expression(fp);

    Tree tayl = {};
    fprintf(fp,"\\section{}");
    
    fprintf(fp, "\\parИнтереснейшая на мой взгляд функция, которая обладает блаблаблабла\n"
                "\nА вот таким образом выглядит ее график\n");
    this->print_tex_plot_x(fp, -30, 30, 0.01);

    fprintf(fp, "\\parУдивительной красоты график! Давайте теперь возьмем производную этой функции по x. Легко заметить что она будет иметь следующий вид:\n\n\n");
    Tree tmp = {};

    tmp.constructor();
    tmp.derivative(this, 'x');

    fprintf(fp, "$ f^{(1)}(x) = $");
    tmp.print_tex_expression(fp);
    fprintf(fp, "\n\n\\parДавайте немного упростим это и без того не сложное выражение\n\n");


    tmp.simplificate_expression(fp);
    fprintf(fp,"\\section{}");
    fprintf(fp, "\n\n \\parИтоговая формула будет иметь следущий вид:\n\n\n$ f^{(1)}(x) = $");
    

    tmp.print_tex_expression(fp);

    tmp.print_tex_plot_x(fp, -30, 30, 0.01);
    fprintf(fp, "\\parНетрудно заметить что так выглядит график проивзодной");
    fprintf(fp,"\\section{}");

    fprintf(fp, "\n\n\\parМало кто знает, но существует такая шикарная вещь как формула Тейлора, позволяющая апроксимировать функцию в точке\n\n"
                "\n$ при x \\rightarrow x_0 $\n\n$f(x) = f(x_0) + \\frac{f^{(1)}(x_0)}{1!} \\cdot (x-x_0)^{1}"
                                                        " + \\frac{f^{(2)}(x_0)}{2!} \\cdot (x-x_0)^{2}" 
                                                        " + \\frac{f^{(3)}(x_0)}{3!} \\cdot (x-x_0)^{3}"
                                               " + \\dots + \\frac{f^{(n)}(x_0)}{n!} \\cdot (x-x_0)^{n} + o(x^n) $\n\n");

    fprintf(fp, "\n\n \\parДавайте разложим функцию в точке 0 по Тейлору и сравним графики полученной функции и первоначального выражения\n\n");
    tayl.constructor();
    tayl.taylor(this, 'x', 6, 0);

    fprintf(fp, "$ f^{(1)}(x) = $");
    tayl.print_tex_expression(fp);

    tayl.print_tex_plot_x(this, fp, -1.5, 1.5, 0.001);

    fprintf(fp, "Очевидно, что 2 эти графика практически совпадают друг с другом\n\n\n");
    fprintf(fp, "\n\nТакже, можно записать полный дифференциал данной функции\n\n");

    this->print_ful_derivative(fp);


    fprintf(fp,"\\section{}");

    fprintf(fp,"\n\n\\parКакой же вывод из всего этого можно сделать? Во-первых, Латех будт падать, если "
                "попробовать нарисовать график с огромным количеством точек. Во-вторых, "
                "слишком большие формулы будут тупо вылезать за экран, и с этим ничего не поделать."
                "На этой положительной ноте я завершаю статью");
    end_of_the_article(fp);



    tmp.destructor();
    tayl.destructor();
    
    system("pdflatex Article.tex");
}
