#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "DIMACS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define UNDEFINED 2

typedef struct partial_interpretation{
    int *atributions; 
    int tam_variavel;/*medimos o tamanho limite do array aqui*/
}partial_interpretation;

typedef struct tree 
{ 
    int variable; 
    int value; 
    bool eh_folha; /*retorna 1 ou 0 p saber se chegamos a uma decisao final*/

    struct tree* left; 
    struct tree* right; 
} tree;

typedef enum{/*usamos essa struct p nomear os numeros inteiros, ent o pc vai ler soma como o numero 0, e assim por diante*/
    op_soma,
    op_subtracao,/*usamos a virgula pq estamos separando elementos de uma lista*/
    op_multi,
    op_divisao,
    op_igual,
    op_variavel,
    op_const,
    op_maiorque,
    op_menorque,
}op_mat;

typedef struct expmat{
op_mat tipo;
int valor;

    struct expmat* left; 
    struct expmat* right; 
}expmat;

typedef struct teoria_smt{
    expmat **expressoes;
    int total_expressoes;
}teoria_smt;

partial_interpretation inicio_partial_interpretation(CNF *formula);
bool eh_sat(CNF *formula, partial_interpretation *options);
bool eh_unsat(CNF *formula, partial_interpretation *options);
partial_interpretation unir(partial_interpretation *inicio, int total_literals, int value, int xi);
tree *resposta_smt(CNF *formula, partial_interpretation now, teoria_smt *teoria);
bool checagem(partial_interpretation *options, teoria_smt *teoria);
#endif