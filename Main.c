#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>


int main(){

    CNF *formula = (CNF*)malloc(sizeof(CNF)); //aloca memoria e inicializa a formula
    formula->clauses = NULL;
    formula->total_literals = 0;
    formula->total_clauses = 0;

    TeoriaLIA *teoria = malloc(sizeof(TeoriaLIA));/*aloca e inicializa a teoriaLIA*/
    teoria->total = 0;
    teoria->n_vars = 0;
    teoria->restricoes = NULL;
    teoria->valores_int = NULL;

    readcnffile("arquivo.cnf", formula, teoria);//chama funcao para leitura do arquivo    
    
    if (formula->clauses == NULL && teoria->restricoes == NULL){
        printf("Erro: Arquivo vazio ou nao encontrado.\n");
        free(formula);
        free(teoria);
        return 1;
}

printf("--- LEITURA ---\n");
printf("Variaveis booleanas: %d\n", formula->total_literals);
printf("Clausulas: %d\n", formula->total_clauses);
printf("Restricoes LIA: %d\n", teoria->total);
printf("Variaveis inteiras: %d\n", teoria->n_vars);

if(teoria->n_vars > 0 && teoria->valores_int){
    printf("Valores inteiros: ");
    for (int i = 0; i < teoria->n_vars; i++){
        printf (" x%d = %d", i + 1, teoria->valores_int[i]);
    }
    printf("\n");
}
   printf("---------------\n");

    partial_interpretation options = inicio_partial_interpretation(formula); //inicializaçao da interpretacao parcial
    
    /*a gnt comeca a teoria vazia p nn dar erro*/
   tree *resposta = resposta_smt(formula, options, teoria);

   if (resposta != NULL && resposta->value == 1){
    printf("SAT\n");
    imprimir(resposta, formula->total_literals);
   }
   else{
    printf("UNSAT\n");
   }

   free_tree(resposta);
   free(options.atributions);
   free(teoria->restricoes);
   free(teoria->valores_int);
   free_cnf(formula);
   free(teoria);
   return 0;
}