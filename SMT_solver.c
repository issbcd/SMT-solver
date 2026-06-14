#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/**
 * Inicializa uma interpretação parcial, alocando memória e definindo 
 * todas as atribuições iniciais como UNDEFINED.
 * * @param formula Ponteiro para a fórmula CNF de referência.
 * @return Estrutura de interpretação parcial inicializada.
 */
partial_interpretation inicio_partial_interpretation(CNF *formula){
    partial_interpretation new_options;
    
    new_options.atributions = malloc(sizeof(int) *(formula->total_literals + 1));

    for (int i = 0; i < formula->total_literals + 1; i++){
        new_options.atributions[i] = UNDEFINED;
    }
    return new_options;
}

/**
 * Verifica se a fórmula CNF é satisfeita (SAT) pelas atribuições atuais.
 * * @param formula Ponteiro para a fórmula CNF.
 * @param options Ponteiro para as atribuições de variáveis atuais.
 * @return true se todas as cláusulas forem satisfeitas, false caso contrário.
 */
bool eh_sat(CNF *formula, partial_interpretation *options){
    clause * clauses_now = formula->clauses; 

    while(clauses_now != NULL){ 
        bool satisfeita = false; 
        literal_node * literal_now = clauses_now->literals; 

        while(literal_now != NULL){ 
            int valores = options->atributions[abs(literal_now->value)]; 

            if(valores !=UNDEFINED){
                if((literal_now->value > 0 && valores == 1) || (literal_now->value < 0 && valores == 0)){ 
                    satisfeita = true; 
                    break; 
                }
            }
            literal_now = literal_now->next; 
        } 
            if(!satisfeita) 
            {
                return false;
            }
        clauses_now = clauses_now->next; 
    }   
    return true; 
}

/**
 * Verifica se a fórmula CNF é insatisfatível (UNSAT) pelas atribuições atuais.
 * Uma fórmula é UNSAT se contiver pelo menos uma cláusula onde todos os 
 * literais avaliados sejam falsos e não haja literais indefinidos.
 * * @param formula Ponteiro para a fórmula CNF.
 * @param options Ponteiro para as atribuições atuais.
 * @return true se a fórmula for comprovadamente UNSAT, false caso contrário.
 */
bool eh_unsat(CNF *formula, partial_interpretation *options){
clause *clauses_now = formula->clauses;

while(clauses_now != NULL){
    bool clause_still_valid = false;
    literal_node *literal_now = clauses_now->literals;

    while (literal_now != NULL){
        int valores = options->atributions[abs(literal_now->value)];
    
    if(valores == UNDEFINED){
        clause_still_valid = true;
        break;
    }

    if((literal_now->value > 0 && valores == 1) || (literal_now->value < 0 && valores == 0)){
        clause_still_valid = true;
        break;
    }
        literal_now = literal_now->next;
}
    if(!clause_still_valid){
        return true;
    }
    clauses_now = clauses_now->next; 
}
return false; 
}

/**
 * Cria uma nova interpretação parcial clonando a inicial e aplicando um 
 * novo valor booleano a uma variável específica (passo de ramificação).
 * * @param inicio Ponteiro para a interpretação parcial base.
 * @param total_literals Total de literais na fórmula.
 * @param value Valor a ser atribuído (0 ou 1).
 * @param xi Índice da variável que receberá o valor.
 * @return Nova estrutura de interpretação parcial com a alteração.
 */
partial_interpretation unir(partial_interpretation *inicio, int total_literals, int value, int xi){
    partial_interpretation new_atributions;
    
    new_atributions.atributions = malloc(sizeof(int) *(total_literals + 1));

    for(int i = 0; i < total_literals + 1; i++){
        new_atributions.atributions[i] = inicio->atributions[i];
    }
    new_atributions.atributions[xi] = value;
    return new_atributions;
}

/**
 * Função interna para fazer o parse do operador relacional LIA a partir de uma string.
 * * @param s String que aponta para o início do operador.
 * @param op Ponteiro onde o operador identificado será armazenado.
 * @return Ponteiro para o caractere imediatamente após o operador, ou NULL se inválido.
 */
static const char *ler_operador(const char *s, Operador *op){
if(strncmp(s, "<=", 2) == 0){
    *op = OP_MENOR_IGUAL;
    return s + 2;
}

if(strncmp(s, ">=", 2) == 0){
    *op = OP_MAIOR_IGUAL;
    return s + 2;
}

if(strncmp(s, "!=", 2) == 0){
    *op = OP_DIFERENTE;
    return s + 2;
}

if(strncmp(s, "==", 2) == 0){
    *op = OP_IGUAL;
    return s + 2;
}

if(*s == '<'){
    *op = OP_MENORQ;
    return s + 1;
}

if(*s == '>'){
    *op = OP_MAIORQ;
    return s + 1;
}
return NULL;
}

/**
 * Converte uma string de texto em uma estrutura ExpressaoLIA.
 * Realiza a leitura de coeficientes, variáveis (formato x<id>) e do lado direito.
 * * @param entrada String contendo a expressão LIA (ex: "2*x1 - x2 <= 5").
 * @param out Ponteiro de saída onde a estrutura preenchida será guardada.
 * @param n_vars Número total de variáveis.
 * @return true se o parse foi bem-sucedido, false em caso de erro de sintaxe.
 */
bool parse_expressao_lia(const char *entrada, ExpressaoLIA *out, int n_vars){
    memset(out, 0, sizeof(*out));
    out->n_vars = n_vars;

    const char *caractere = entrada;

    while(*caractere){
        while(*caractere == ' ') caractere++; 

        if(*caractere == '<' || *caractere == '>' || *caractere == '=' || *caractere == '!'){
            break;
        }
        if(*caractere == '\0'){
            break;
        }

        int sinal = 1;
        if(*caractere == '+'){
            sinal =  1;
            caractere++;
        }
        else if(*caractere == '-'){
            sinal = -1;
            caractere++;
        }

        while(*caractere == ' '){
            caractere++;
        }

        int coef = 0;
        bool tem_coef = false;
        while(isdigit(*caractere)){
            coef = coef * 10 + (*caractere - '0');
            tem_coef = true;
            caractere++;
        }

        if(*caractere == '*'){
            caractere++;
        } 

        if(*caractere != 'x'){
            fprintf(stderr, "Esperava 'x<n>', recebi '%c'\n", *caractere);
            return false;
        }
        caractere++;

        int indice = 0;
        if(!isdigit(*caractere)){
            fprintf(stderr, "Indice ausente\n");
            return false;
        }
        while(isdigit(*caractere)){
            indice = indice * 10 + (*caractere - '0');
            caractere++;
        }

        if(indice < 1 || indice > MAX_VARS){
            fprintf(stderr, "Indice %d fora do intervalo\n", indice);
            return false;
        }

        if(!tem_coef) coef = 1; 
        out->coeficientes[indice - 1] += sinal * coef;
    }

    while(*caractere == ' '){
        caractere++;
    } 

    const char *dps = ler_operador(caractere, &out->op);
    if(!dps){
        fprintf(stderr, "Operador nao encontrado\n");
        return false;
    }
    caractere = dps;

    while(*caractere == ' '){
        caractere++;
    } 

    int negativo = 1;
    if(*caractere == '-'){ 
        negativo = -1;
        caractere++; }
    else if (*caractere == '+'){
        caractere++; }

    int ladoD = 0;
    if(!isdigit(*caractere)){
        fprintf(stderr, "Constante ausente no lado direito\n");
        return false;
    }
    while(isdigit(*caractere)){
        ladoD = ladoD * 10 + (*caractere - '0');
        caractere++;
    }

    out->constante = negativo * ladoD;
    return true;
}

/**
 * Avalia matematicamente se uma expressão LIA é verdadeira ou falsa com base nos valores inteiros atuais.
 * * @param exp Ponteiro para a expressão LIA estruturada.
 * @param valores Vetor com os valores inteiros das variáveis aritméticas.
 * @return true se a relação aritmética for satisfeita, false caso contrário.
 */
bool avalia_lia(const ExpressaoLIA *exp, int *valores){
    int soma = 0;

    for(int i = 0; i < exp->n_vars; i++){
        soma += exp->coeficientes[i] * valores[i];
    }
    switch(exp->op){
        case OP_MENORQ:      return soma <  exp->constante;
        case OP_MAIORQ:      return soma >  exp->constante;
        case OP_MENOR_IGUAL: return soma <= exp->constante;
        case OP_MAIOR_IGUAL: return soma >= exp->constante;
        case OP_IGUAL:       return soma == exp->constante;
        case OP_DIFERENTE:   return soma != exp->constante;
    }
    return false;
}

/**
 * Verifica a consistência da teoria LIA em relação às atribuições booleanas atuais.
 * Detecta conflitos entre o valor que a variável booleana assumiu e a avaliação da restrição.
 * * @param atrib_bool Vetor de atribuições booleanas atuais.
 * @param teoria Ponteiro para a estrutura que guarda as restrições LIA.
 * @return O índice da restrição que causou violação (conflito), ou -1 se estiver consistente.
 */
int checagem_lia(int *atrib_bool, TeoriaLIA *teoria){
    if(teoria == NULL || teoria->restricoes == NULL){
        return -1;
    }

    for(int i = 0; i < teoria->total; i++){
        int val_bool = atrib_bool[i + 1];
        if (val_bool != UNDEFINED) {
            bool eval = avalia_lia(&teoria->restricoes[i], teoria->valores_int);
            if ((val_bool == 1 && !eval) || (val_bool == 0 && eval)) {
                return i; 
            }
        }
    }
    return -1; 
}

/**
 * Aloca e inicializa um novo nó vazio para a árvore de decisão SMT.
 * * @return Ponteiro para o nó da árvore recém-criado.
 */
tree *no_da_arvore(){
    tree *nv_no = (tree*)malloc(sizeof(tree));
    nv_no->left = NULL;
    nv_no->right = NULL;
    nv_no->value = -1;
    nv_no->variable = -1;
    nv_no->solucao = NULL;
    nv_no->n_vars = 0;
    return nv_no;
    }

/**
 * Executa o algoritmo DPLL estendido para SMT (Lazy SMT).
 * Combina a busca booleana com checagens da teoria LIA via backtracking.
 * * @param formula Ponteiro para a fórmula CNF.
 * @param now_interpretation Interpretação booleana parcial do escopo atual.
 * @param teoria Ponteiro para a teoria LIA associada.
 * @return Árvore de decisão representando o espaço de busca e a eventual solução.
 */
tree *resposta_smt(CNF *formula, partial_interpretation now_interpretation, TeoriaLIA *teoria){
    tree *no_now = no_da_arvore();

    if(eh_sat(formula, &now_interpretation)){
    int conferelia = checagem_lia(now_interpretation.atributions, teoria);
    
    if(conferelia == -1){
        no_now->value = 1;
        int n = formula->total_literals;
        no_now->n_vars = n;
        no_now->solucao = malloc(sizeof(int) * (n + 1));

        for(int i = 0; i <= n; i++){
            no_now->solucao[i] = now_interpretation.atributions[i];
        }

        return no_now;
    }
    
    }

    if(eh_unsat(formula, &now_interpretation)){
        no_now->value = 0; 
        return no_now;
    }

    int teste_variavel = -1;

    for(int i = 1; i <= formula->total_literals; i++){
        if (now_interpretation.atributions[i] == UNDEFINED){
            teste_variavel = i;
            break;
        }
    }

    if(teste_variavel == -1){
        no_now->value = 0;
        return no_now;
    }

    no_now->variable = teste_variavel;

    // Explora ramo verdadeiro (atribuição = 1)
    partial_interpretation caso_vdd = unir(&now_interpretation, formula->total_literals, 1, teste_variavel);
    no_now->left = resposta_smt(formula, caso_vdd, teoria);
    free(caso_vdd.atributions);

    // Explora ramo falso (atribuição = 0)
    partial_interpretation caso_falso = unir(&now_interpretation, formula->total_literals, 0, teste_variavel);
    no_now->right = resposta_smt(formula, caso_falso, teoria);
    free(caso_falso.atributions);

    no_now->value = no_now->left->value || no_now->right->value;

    return no_now;
    }

/**
 * Libera recursivamente toda a memória alocada para a árvore de decisão SMT.
 * * @param node Ponteiro para o nó raiz/atual da árvore.
 */
void free_tree(tree *node){
    if (node == NULL){
        return;
    }

    free_tree(node->left);
    free_tree(node->right);

    if (node->solucao != NULL){
        free(node->solucao); 
    }
    free(node);
}

/**
 * Libera recursivamente toda a memória alocada para a estrutura CNF.
 * Limpa literais, cláusulas e evita vazamento de memória (memory leaks).
 * * @param formula Ponteiro para a fórmula CNF a ser desalocada.
 */
void free_cnf(CNF *formula){
    clause *current_clause = formula->clauses;
    while(current_clause != NULL){
        literal_node *literal = current_clause->literals;

        while(literal != NULL){
        literal_node *aux = literal;
        literal = literal->next;
        free(aux);
        }
        clause *aux_clause = current_clause;
        current_clause = current_clause->next;
        free(aux_clause);
    }
    free(formula);
}

/**
 * Percorre a árvore de decisão em busca de um nó que contenha uma solução válida (SAT).
 * Se encontrado, imprime a configuração das variáveis no terminal.
 * * @param node Ponteiro para o nó atual da árvore.
 * @param total_literals Quantidade total de literais a serem impressos.
 * @return true se uma solução foi encontrada e impressa, false caso contrário.
 */
bool imprimir(tree *node, int total_literals){

    if(node == NULL){
        return false;
    }

    if(node->value == 1 && node->solucao != NULL){
        printf("Configuracao encontrada:\n");

        for(int i = 1; i <= total_literals; i++){
            printf("x%d = %d\n", i, node->solucao[i]); 
        }

        return true;
    }

    if(imprimir(node->left, total_literals))  return true; 
    if(imprimir(node->right, total_literals)) return true; 

    return false; 
}
