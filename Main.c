
int main(){

    CNF *formula = (CNF*)malloc(sizeof(CNF)); //aloca memoria para formula
    teoria_smt *teoria = (teoria_smt*)malloc(sizeof(teoria_smt));

    teoria-> total_expressoes = 0;
    teoria->expressoes = NULL; /*inicializacao segura da teoria, garante q tudo ta zerado*/
    
    readcnffile("arquivo.cnf", formula, teoria);//chama funcao para leitura do arquivo
    
    if (formula->clauses == NULL && teoria->expressoes == NULL){
        printf("Erro: Arquivo vazio ou nao encontrado.\n");
        return 1;
}

printf("--- TESTE DE LEITURA ---\n");
printf("Total de literais lidos: %d\n", formula->total_literals);
printf("Total de clausulas lidas: %d\n", formula->total_clauses);
printf("Total de expressoes na teoria: %d\n", teoria->total_expressoes);
printf("------------------------\n");
    partial_interpretation options = inicio_partial_interpretation(formula); //inicializaçao da interpretacao parcial
    
    /*a gnt comeca a teoria vazia p nn dar erro*/
   tree *resposta = calcular_smt(formula, options, teoria);

   if (resposta != NULL && resposta->value == 1){
    printf("SAT\n");
    imprimir(resposta, formula->total_literals);
   }

   else{
    printf("UNSAT\n");
   }

   free_tree(resposta);
   free(options.atributions);
   free_cnf(formula);

   free(teoria);
   return 0;
}