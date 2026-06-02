#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void readcnffile (const char* filename, CNF* problem, teoria_smt *teoria){
    FILE* file = fopen(filename, "r");

    if(file == NULL){/*ve se o codigo existe*/
        printf("error acessing cnf file\n PLEASE TRY AGAIN");
        exit(1);
    }

    char lines[256];/*buffer p ler as palavras do cabecalho*/
    problem->clauses = NULL; // inicializando a lista encadeada do problema

    /* ####### DISCLAIMER #######
    nessa área usei algumas funcoes de leitura e de tratamento de strings que podem não serem familiares.
    entretanto, foi a forma mais simples de fazer isso.
    "glossário:"
    *ANTERIORMENTE* fopen - abre um arquivo de forma que podemos lidar com ele
    fscanf - parecido com o scanf, mas o f inicial significa "file". vai ler nosso arquivo pulando espaços/quebras.
    strcmp - string compare, faz comparação 
    fgetc - file get char, recebe um caractere do arquivo (lembra getchar)
    feof - file end of file, verifica se chegamos ao final da funcao
    fclose - file close, fecha o arquivo que abrimos com fopen
    */
    while (fscanf(file, "%s", lines) == 1) // ele vai ler o cabeçalho 
    {
        if (strcmp(lines, "c") == 0) 
        {
            while (fgetc(file) != '\n' && !feof(file)); // ele vai pular os comentários
        } 

        else if (strcmp(lines, "p") == 0) 
        {
            // ele vai ler nosso cabeçalho
            fscanf(file, " cnf %d %d", &problem->total_literals, &problem->total_clauses);
            break; // agora todo o resto do nosso arquivo é apenas cláusulas
        }
    }

    // agora nós vamos ler as cláusulas
    clause* current_clause = NULL; // ponteiro auxiliar para montar a lista de cláusulas

    for (int i = 0; i < problem->total_clauses; i++) 
    {
        // aloca uma nova cláusula
        clause* new_clause = (clause*)malloc(sizeof(clause));
        new_clause->size = 0; // inicia c tamanho nulo
        new_clause->literals = NULL;
        new_clause->next = NULL;

        // conecta a cláusula na lista do problema
        if (problem->clauses == NULL) 
        {
            problem->clauses = new_clause;
        } 
        
        else 
        {
            current_clause->next = new_clause;
        }

        current_clause = new_clause;

        int literal;
        literal_node* current_literal = NULL; // ponteiro auxiliar para montar a lista de literais
        
        while (fscanf(file, "%d", &literal) == 1 && literal != 0) // lê toda a cláusula
        {
            // alocação do literal para a cláusula
            literal_node* new_literal = (literal_node*)malloc(sizeof(literal_node));
            new_literal->value = literal;
            new_literal->next = NULL;
            
            // conecta o literal na lista da cláusula atual
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
    /*AS ATERACOES P SMT TAO QSE TDS DAQUI P BAIXO*/
    /*alocamos espaco e deixamos o indice ser a variavel */
    /*agora vamos ler a teoria matematica do final do arquivo*/
    teoria->total_expressoes = problem->total_literals;
    teoria->expressoes = (char**)malloc(sizeof(char*) * (problem->total_literals + 1));/*um array de ponteiros eh alocado pras strings pois cada posicao seria uma equacao*/
    
    /*inicializa tudo NULL p evitar lixo de memoria*/
    for (int i = 0; i <= problem->total_literals; i++){
        teoria->expressoes[i] = NULL;
    }

    /*vamos procurar por linhas q comecem c equacao*/
    char buffer_eq[256];/*le a string da equacao*/
    int num_variavel;/*armazena a variavel associada a equacao*/
    
    while(fscanf(file, "%s", lines) == 1){/*le o resto do arquivo atras de linhas q comecam c "e"*/
        if(strcmp(lines, "e") == 0){
            fscanf(file, "%d", &num_variavel);/*le a identidade da variavel*/
            fgets(buffer_eq, sizeof(buffer_eq), file);/*le a linha td, a equacao matematica*/
            buffer_eq[strcspn(buffer_eq, "\n")] = 0; /*remove a quebra da linha no final(o enter)*/
            
            int offset = 0;/*calcula o deslocamento p pular os espacos em branco e é chamado de offset por convencao*/
            while(buffer_eq[offset] == ' '){
                offset++;
            }
            
            teoria->expressoes[num_variavel] = strdup(buffer_eq + offset);/*salva a equacao na posicao certa no array*/
        }
    }
    
    fclose(file);/*fecha o arquivo e libera o sistema*/
}
