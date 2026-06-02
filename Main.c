
int main(){
    CNF *formula = (CNF*)malloc(sizeof(CNF)); //aloca memoria para formula
    readcnffile("arquivo.cnf", formula);     //chama funcao para leitura do arquivo
    printf("--- TESTE DE LEITURA ---\n");
printf("Total de Literais lidos: %d\n", formula->total_literals);
printf("Total de Clausulas lidas: %d\n", formula->total_clauses);
printf("------------------------\n");
    partial_interpretation options = inicio_partial_interpretation(formula); //inicializaçao da interpretacao parcial
    
    /*a gnt comeca a teoria vazia p nn dar erro*/
    teoria_smt *teoria = (teoria_smt*)malloc(sizeof(teoria_smt));
    teoria->total_expressoes = 0;
    teoria->expressoes = NULL;

    tree *resposta = resposta_smt(formula, options, teoria); //chama funcao da construcao da arvore de decisao, passa a formula lida e as opcoes iniciais

    if(resposta != NULL && resposta->value == 1)  //verificacao e impressao, aqui, se a resposta for 1, significa que ao menos uma das folhas resultou em sat
    {
        printf("SAT\n");
        imprimir(resposta, formula->total_literals); //chama a funcao imprimir que vai navegar a arvore para encontrar a folha sat e imprimir os valores
    }
    else
    {
        printf("UNSAT\n"); // se nao, é unsat
    }

    free_tree(resposta);
    free(options.atributions);
    free_cnf(formula); //libera  a estrutura de formula, o array de atriubuicoes e o no da raiz
    free(teoria);
    return 0;
}
