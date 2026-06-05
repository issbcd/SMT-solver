#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "DIMACS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define UNDEFINED -1 
#define MAX_VARS 64 /*vms declarar o max de variaveis q uma unica equacao pode ter*/

typedef enum{/*usamos essa struct p nomear os numeros inteiros, ent o pc vai ler soma como o numero 0, e assim por diante*/
    OP_MENORQ,/*<*/
    OP_MAIORQ,/*>*/
    OP_MENOR_IGUAL,/*<=*/
    OP_MAIOR_IGUAL,/*>=*/
    OP_IGUAL,/*==*/
    OP_DIFERENTE,/*!=*/
}Operador;

typedef struct ExpressaoLIA{
    int coeficientes[MAX_VARS];/*vai guardar os multiplicadores, tipo  2*X1, guarda o 2 */
    int n_vars;/*quantas variaveis estao na expressao*/
    Operador op; /*qual o simbolo da operacao*/
    int constante; /*o numero do lado direito, tipo o 10 em x+y=10*/
}ExpressaoLIA;

typedef struct TeoriaLIA{/*vai guardar tds as equacoes do problema e os valores atuais*/
ExpressaoLIA *restricoes;/*array dinamico q guarda tds as equacoes lidas*/
int total;/*numero total de equacoes do array*/
int n_vars;/*numero de variaveis inteiras distintas no problema inteiro*/
int *valores_int; /*guarda o valor numerico de cada teste p cada variavel*/
}TeoriaLIA;

typedef struct partial_interpretation{
    int *atributions; 
}partial_interpretation;

typedef struct tree{ 
    int variable; 
    int value; 
    struct tree* left; 
    struct tree* right; 
} tree;
/*declaracao das funcoes de fluxo booleano */
partial_interpretation inicio_partial_interpretation(CNF *formula);
bool eh_sat(CNF *formula, partial_interpretation *options);
bool eh_unsat(CNF *formula, partial_interpretation *options);
partial_interpretation unir(partial_interpretation *inicio, int total_literals, int value, int xi);

/*declaracao das funcoes da TeoriaLIA*/
bool parse_expressao_lia(const char *entrada, ExpressaoLIA *out, int n_vars);
bool avaliar_lia (const ExpressaoLIA *exp, int *valores);
int checagem_lia(int *atrib_bool, TeoriaLIA *teoria);

/*funcoes da arvore de busca e a execucao principal*/
tree *no_da_arvore();
tree *resposta_smt(CNF *formula, partial_interpretation now, TeoriaLIA *teoria);

/*as funcoes de limpeza e de saida*/
bool imprimir(tree *node, int total_literals);
void free_tree(tree *node);
void free_cnf(CNF *formula);

#endif