#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h> /*vms usar uma funcao isdigit, q pertence a essa biblioteca e eh usada p verificar se o caractere seria um digito numerico e sera usada p identificar se a linha atual seria uma clausula DIMACS, comeca c um nomero ou um sinal negativo*/

void readcnffile (const char* filename, CNF* formula, TeoriaLIA *teoria){
    FILE* file = fopen(filename, "r");

    if(file == NULL){/*ve se o codigo existe*/
        printf("Erro ao abrir o arquivo: %s\n", filename);
        exit(1);
    }

    char lines[256];/*buffer p ler as palavras do cabecalho*/
    formula->clauses = NULL; /*por seguranca inicializa tudo c zero p evitar memory leak*/
    formula->total_literals = 0;
    formula->total_clauses = 0;
    teoria->total = 0;
    teoria->num_variavel = 0;
    teoria->restricoes = NULL;
    teoria->valores_int = NULL;
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
        if (strcmp(lines, "c") == 0) {/*se comecar com "c", seria so um comentario ent ignora*/
            while (fgetc(file) != '\n' && !feof(file)); // vai bater caractere por caractere ate o final ou ate a quebra de linha
        } 
        else if (strcmp(lines, "p") == 0){/*encontrou a linha de declaracao do formato*/
        int int_temporaria = 0; /*variavel temporaria p novo parametro, as variaveis inteiras LIA*/
        
        fscanf(file, " cnf %d %d %d", &formula->total_literals, &formula->total_clauses, &int_temporaria);

        teoria->n_vars = int_temporaria; /*vai salvar a quantidade de variaveis inteiras */
        teoria->valores_int = calloc(int_temporaria, sizeof(int)); /*alocamos espaco no array de inteiros e ja o preenche c 0, ent se o arquivo nn tiver linha v, as variaveis comecam c 0 e assim evita lixo de memoria*/
        break;
    }
}

    // agora nós vamos ler as cláusulas na parte de analise booleana - SAT
    clause* current_clause = NULL; // ponteiro auxiliar para montar a lista de cláusulas

    for (int i = 0; i < formula->total_clauses; i++){
        // aloca uma nova cláusula
        clause* new_clause = (clause*)malloc(sizeof(clause));
        new_clause->size = 0; // inicia c tamanho nulo
        new_clause->literals = NULL;
        new_clause->next = NULL;

        // conecta a cláusula na lista do problema
        if (formula->clauses == NULL){
            formula->clauses = new_clause;
        } 
        
        else{
            current_clause->next = new_clause;
        }
        current_clause = new_clause;

        int literal;
        literal_node* current_literal = NULL; // ponteiro auxiliar para montar a lista de literais
        
        while (fscanf(file, "%d", &literal) == 1 && literal != 0) // lê toda a cláusula
        { // alocação do literal para a cláusula
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
    int controle_rest = 0; /*controlar o tamanho do array de restricoes e evita um malloc em todas as linhas*/

    while (fgets(lines, sizeof(lines), file)){/*vai ler a linha td, até encontrar a quebra*/
        lines[strcspn(lines, "\n")] = '\0'; /*aqui ela troca a quebra de linha pelo nulo*/
    
        if(lines[0] == 't' && lines[1] == ' '){/*convencionamos chamar a restricao LIA de t e o lines[1]==' * evita falso positivos pq garante q o t seria so um marcador*/
            const char *exp_str = lines + 2; /*vamos pular os espacos extras e ler so a matematica*/
            while (*exp_str == ' '){
                exp_str++;/*pula os espacos do comeco*/
            }
        if (teoria->total >= controle_rest){/*medida de controle: se passamos do limite, vms precisar de mais espaço*/
            if(controle_rest == 0){
                controle_rest = 8;/*sendo a primeira restricao lida, ainda nn tem espaco alocado. a reserva de espaco para 8 é apenas pq de um em um seria ineficiente */
            }
            else{
                controle_rest = 2 * controle_rest;/*dobramos o tamanho atual, alem de crescer em exponencial, garantimos q as realocacoes sejam pequenas msm c mtas restricoes*/
            }
            teoria->restricoes = realloc(teoria->restricoes, controle_rest *sizeof(ExpressaoLIA));
        }

        if(parse_expressao_lia (exp_str, &teoria->restricoes[teoria->total], teoria->n_vars)){/*o parse vai transformar o texto em um formato legivel p o programa*/
            teoria->total++; /*ou seja, registra a existencia de mais de uma restricao, caso tenha*/
        }

        else{
            fprintf(stderr, "Aviso! Falha no parse: %s\n", exp_str);/*o fprint permite imprimir no espaco de erro que o aviso e o stderr é uma funcao convencional para indicar erros e avisos*/
        }
        }

        else if(lines[0] == 'v' && lines[1] == ' '){/*aqui ele vai ver se a linha seria de valores (v)*/
            char *tok = strtok(lines+2, " ");/*essa funcao corta a string em varios pedacos e vai quebrar onde encontrar espaco, o line + 2 garante q pula o v e o espaço ent so vai pegar a expressao matematica*/
            int indice = 0; /*controla a posicao do array em q o prox numero vai ser guardado, comeca do zero pq é a primeira variavel*/
            while(tok && indice < teoria->n_vars){/* o loop continua enquanto ainda tem nn acabou a srting e ainad tem variavel p preeencher (ou nn passou do limite)*/
                teoria->valores_int[indice++] = atoi(tok); /*converte o texto - cahr - pro inteiro - int e guarda esse valor na posicao do indice do array, dps incrementa*/
                tok = strtok(NULL, " "); /*vai p prox pedaco da msm string, retornando NULL qnd acabar */
            }              
        }   
    }
    fclose(file);
}