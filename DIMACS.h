#ifndef DIMACS_H
#define DIMACS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** * Declaração opaca avançada da estrutura TeoriaLIA.
 * Permite que a função de leitura use o tipo como parâmetro antes de sua 
 * definição completa no resolvedor SMT.
 */
typedef struct TeoriaLIA TeoriaLIA;

/**
 * @struct literal_node
 * @brief Elemento de uma lista encadeada que representa um literal booleano.
 * O valor armazenado indica a variável e sua polaridade (negativa se valor < 0).
 */
typedef struct literal_node 
{
    int value;                  /**< Valor numérico identificador do literal. */
    struct literal_node* next;  /**< Ponteiro para o próximo literal da cláusula. */
} literal_node;

/**
 * @struct clause
 * @brief Representa uma única cláusula na fórmula CNF (disjunção de literais).
 */
typedef struct clause{
    literal_node* literals;     /**< Ponteiro para o primeiro nó da lista de literais. */
    int size;                   /**< Quantidade de literais presentes nesta cláusula. */
    struct clause* next;        /**< Ponteiro para a próxima cláusula do problema. */
} clause;

/**
 * @struct CNF
 * @brief Estrutura que encapsula a fórmula lógica global em Forma Normal Conjuntiva.
 */
typedef struct CNF{
    clause* clauses;            /**< Ponteiro para a cabeça da lista de cláusulas. */
    int total_clauses;          /**< Contagem total de cláusulas registradas. */
    int total_literals;         /**< Contagem total de variáveis/literals distintos. */
} CNF;

/**
 * Lê e processa um arquivo de entrada no formato DIMACS estendido para SMT.
 * * @param filename Caminho ou nome do arquivo de entrada.
 * @param formula Ponteiro para a estrutura CNF que receberá os dados lógicos.
 * @param teoria Ponteiro para o contexto que receberá as restrições da teoria LIA.
 */
void readcnffile (const char *filename, CNF *formula, TeoriaLIA *teoria);

#endif