#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/**
 * Lê e processa um arquivo de entrada no formato DIMACS estendido para SMT (LIA).
 * Realiza o parse do cabeçalho, aloca e constrói a lista encadeada de cláusulas booleanas,
 * e extrai as restrições aritméticas lineares (LIA) junto com as atribuições de valores iniciais.
 * * @param filename Caminho do arquivo a ser lido.
 * @param formula Ponteiro para a estrutura CNF que armazenará a fórmula booleana.
 * @param teoria Ponteiro para a estrutura TeoriaLIA que armazenará as restrições matemáticas.
 */
void readcnffile (const char* filename, CNF* formula, TeoriaLIA *teoria){
    FILE* file = fopen(filename, "r");

    if(file == NULL){
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    char lines[256];
    
    // Inicialização de segurança para evitar lixo de memória
    formula->clauses = NULL; 
    formula->total_literals = 0;
    formula->total_clauses = 0;
    teoria->total = 0;
    teoria->n_vars = 0;
    teoria->restricoes = NULL;
    teoria->valores_int = NULL;

    // Processamento do cabeçalho do arquivo
    while (fscanf(file, "%s", lines) == 1) 
    {
        if (strcmp(lines, "c") == 0) {
            // Ignora linhas de comentários até o fim da linha ou do arquivo
            while (fgetc(file) != '\n' && !feof(file)); 
        } 
        else if (strcmp(lines, "p") == 0){
            int int_temporaria = 0; 
            
            fscanf(file, " cnf %d %d %d", &formula->total_literals, &formula->total_clauses, &int_temporaria);

            teoria->n_vars = int_temporaria; 
            // Aloca e zera o array para evitar lixo de memória caso a linha 'v' esteja ausente
            teoria->valores_int = calloc(int_temporaria, sizeof(int)); 
            break;
        }
    }

    // Leitura e montagem da estrutura das cláusulas booleanas (SAT)
    clause* current_clause = NULL; 

    for (int i = 0; i < formula->total_clauses; i++){
        clause* new_clause = (clause*)malloc(sizeof(clause));
        new_clause->size = 0; 
        new_clause->literals = NULL;
        new_clause->next = NULL;

        if (formula->clauses == NULL){
            formula->clauses = new_clause;
        } 
        else{
            current_clause->next = new_clause;
        }
        current_clause = new_clause;

        int literal;
        literal_node* current_literal = NULL; 
        
        // Lê os literais da cláusula atual até encontrar o delimitador '0'
        while (fscanf(file, "%d", &literal) == 1 && literal != 0) 
        { 
            literal_node* new_literal = (literal_node*)malloc(sizeof(literal_node));
            new_literal->value = literal;
            new_literal->next = NULL;
            
            if (current_clause->literals == NULL){
                current_clause->literals = new_literal;
            } 
            else{
                current_literal->next = new_literal;
            }
            current_literal = new_literal;
            current_clause->size++;
        }
    }
    
    int controle_rest = 0; // Controla a capacidade alocada do vetor de restrições

    // Processamento do corpo do arquivo (Restrições LIA e Valores)
    while (fgets(lines, sizeof(lines), file)){
        lines[strcspn(lines, "\n")] = '\0'; // Remove quebras de linha
    
        // Identifica linha de restrição da teoria LIA ('t ')
        if(lines[0] == 't' && lines[1] == ' '){
            const char *exp_str = lines + 2; 
            while (*exp_str == ' '){
                exp_str++;
            }
            
            // Gerenciamento dinâmico de memória (dobra a capacidade por eficiência)
            if (teoria->total >= controle_rest){
                if(controle_rest == 0){
                    controle_rest = 8;
                }
                else{
                    controle_rest = 2 * controle_rest;
                }
                teoria->restricoes = realloc(teoria->restricoes, controle_rest * sizeof(ExpressaoLIA));
            }

            if(parse_expressao_lia (exp_str, &teoria->restricoes[teoria->total], teoria->n_vars)){
                teoria->total++; 
            }
            else{
                fprintf(stderr, "Aviso! Falha no parse: %s\n", exp_str);
            }
        }
        // Identifica linha de definição de valores inteiros das variáveis ('v ')
        else if(lines[0] == 'v' && lines[1] == ' '){
            char *tok = strtok(lines + 2, " ");
            int indice = 0; 
            
            while(tok && indice < teoria->n_vars){
                teoria->valores_int[indice++] = atoi(tok); 
                tok = strtok(NULL, " "); 
            }              
        }   
    }
    fclose(file);
}