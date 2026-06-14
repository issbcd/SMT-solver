#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "DIMACS.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/** Definição para variáveis com valor lógico ainda não determinado. */
#define UNDEFINED -1 

/** Capacidade máxima de variáveis suportada por uma única expressão LIA. */
#define MAX_VARS 64 

/**
 * @enum Operador
 * @brief Identificadores numéricos para os operadores relacionais da teoria LIA.
 */
typedef enum{
    OP_MENORQ,       /**< Operador menor que ('<') */
    OP_MAIORQ,       /**< Operador maior que ('>') */
    OP_MENOR_IGUAL,  /**< Operador menor ou igual ('<=') */
    OP_MAIOR_IGUAL,  /**< Operador maior ou igual ('>=') */
    OP_IGUAL,        /**< Operador de igualdade ('==') */
    OP_DIFERENTE,    /**< Operador de desigualdade ('!=') */
}Operador;

/**
 * @struct ExpressaoLIA
 * @brief Representação estruturada de uma restrição aritmética linear inteira.
 */
typedef struct ExpressaoLIA{
    int coeficientes[MAX_VARS]; /**< Vetor de multiplicadores de cada variável aritmética. */
    int n_vars;                 /**< Quantidade de variáveis ativas nesta expressão. */
    Operador op;                /**< Operador relacional da restrição. */
    int constante;              /**< Constante numérica isolada no lado direito (b). */
}ExpressaoLIA;

/**
 * @struct TeoriaLIA
 * @brief Contexto global que gerencia o conjunto de restrições lineares e seus valores atuais.
 */
typedef struct TeoriaLIA{
    ExpressaoLIA *restricoes; /**< Vetor dinâmico contendo todas as equações/restrições. */
    int total;                /**< Quantidade total de restrições armazenadas. */
    int n_vars;               /**< Total de variáveis inteiras distintas no problema global. */
    int *valores_int;         /**< Vetor contendo a atribuição inteira atual de cada variável. */
}TeoriaLIA;

/**
 * @struct partial_interpretation
 * @brief Estado atual das atribuições booleanas no solucionador.
 */
typedef struct partial_interpretation{
    int *atributions; /**< Vetor indexado de atribuições lógicas (0, 1 ou UNDEFINED). */
}partial_interpretation;

/**
 * @struct tree
 * @brief Estrutura de nó para a árvore de decisão do algoritmo SMT.
 */
typedef struct tree{ 
    int variable;        /**< Índice da variável booleana que sofreu ramificação no nó. */
    int value;           /**< Resultado do nó: 1 (SAT), 0 (UNSAT) ou -1 (interno). */
    int *solucao;        /**< Ponteiro para o vetor final de atribuições caso o nó seja SAT. */
    int n_vars;          /**< Quantidade total de variáveis gravadas na solução. */
    struct tree* left;   /**< Ponteiro para o ramo da atribuição verdadeira (1). */
    struct tree* right;  /**< Ponteiro para o ramo da atribuição falsa (0). */
} tree;

/* ========================================================================== */
/* Funções de Fluxo Booleano                            */
/* ========================================================================== */

/**
 * Inicializa uma interpretação parcial vazia baseada na fórmula CNF.
 * @param formula Ponteiro para a fórmula CNF de referência.
 * @return Estrutura de interpretação alocada com variáveis em UNDEFINED.
 */
partial_interpretation inicio_partial_interpretation(CNF *formula);

/**
 * Avalia se a fórmula CNF foi totalmente satisfeita pela atribuição atual.
 * @param formula Ponteiro para a fórmula CNF.
 * @param options Ponteiro para o estado atual das atribuições booleanas.
 * @return true se todas as cláusulas forem verdadeiras, false caso contrário.
 */
bool eh_sat(CNF *formula, partial_interpretation *options);

/**
 * Avalia se a fórmula CNF tornou-se impossível de ser satisfeita (conflito precoce).
 * @param formula Ponteiro para a fórmula CNF.
 * @param options Ponteiro para as atribuições atuais.
 * @return true se a fórmula for comprovadamente insatisfatível, false caso contrário.
 */
bool eh_unsat(CNF *formula, partial_interpretation *options);

/**
 * Duplica uma interpretação aplicando uma nova ramificação de valor lógico.
 * @param inicio Interpretação original de base.
 * @param total_literals Quantidade total de literais do problema.
 * @param value Valor lógico a ser forçado (0 ou 1).
 * @param xi Índice da variável alvo da ramificação.
 * @return Nova interpretação parcial modificada.
 */
partial_interpretation unir(partial_interpretation *inicio, int total_literals, int value, int xi);

/* ========================================================================== */
/* Funções de Teoria LIA                              */
/* ========================================================================== */

/**
 * Interpreta textualmente uma string e formata o resultado em uma ExpressaoLIA.
 * @param entrada String bruta contendo a inequação linear.
 * @param out Ponteiro de destino da estrutura a ser populada.
 * @param n_vars Número limite de variáveis booleanas/inteiras.
 * @return true se o parse foi concluído com sucesso, false se houver erro de sintaxe.
 */
bool parse_expressao_lia(const char *entrada, ExpressaoLIA *out, int n_vars);

/**
 * Avalia o valor de verdade de uma expressão LIA substituindo suas variáveis por inteiros.
 * @param exp Ponteiro para a restrição avaliada.
 * @param valores Vetor contendo a atribuição numérica de teste das variáveis inteiras.
 * @return true se a relação aritmética for válida, false caso seja violada.
 */
bool avalia_lia(const ExpressaoLIA *exp, int *valores);

/**
 * Verifica se o estado lógico atual viola alguma restrição geométrica/aritmética da teoria.
 * @param atrib_bool Vetor das atribuições booleanas atuais.
 * @param teoria Contexto geral com as equações lineares e seus valores inteiros.
 * @return O índice da restrição LIA que gerou conflito, ou -1 se estiver tudo consistente.
 */
int checagem_lia(int *atrib_bool, TeoriaLIA *teoria);

/* ========================================================================== */
/* Funções de Busca e Árvore                            */
/* ========================================================================== */

/**
 * Aloca dinamicamente um novo nó zerado para compor a árvore de decisão.
 * @return Ponteiro para a estrutura de nó criada.
 */
tree *no_da_arvore();

/**
 * Executa o motor principal do solucionador Lazy SMT por meio de busca DPLL com Backtracking.
 * @param formula Ponteiro para a fórmula lógica proposicional (CNF).
 * @param now Estado atual da interpretação das variáveis lógicas.
 * @param teoria Ponteiro para a estrutura que gerencia o estado da teoria LIA.
 * @return Nó raiz de decisão contendo o mapeamento completo do espaço de busca.
 */
tree *resposta_smt(CNF *formula, partial_interpretation now, TeoriaLIA *teoria);

/* ========================================================================== */
/* Funções de Limpeza e Saída                           */
/* ========================================================================== */

/**
 * Varre a árvore de decisão para encontrar e exibir em tela a primeira atribuição válida.
 * @param node Nó inicial/atual da busca recursiva.
 * @param total_literals Total de variáveis cujo estado deve ser impresso.
 * @return true se encontrou e exibiu uma solução satisfatível, false caso contrário.
 */
bool imprimir(tree *node, int total_literals);

/**
 * Libera recursivamente toda a memória ramificada associada à árvore SMT.
 * @param node Nó inicial do processo de desalocação.
 */
void free_tree(tree *node);

/**
 * Limpa e desaloca as estruturas de listas encadeadas internas de uma fórmula CNF.
 * @param formula Estrutura CNF alvo da limpeza de memória.
 */
void free_cnf(CNF *formula);

#endif

