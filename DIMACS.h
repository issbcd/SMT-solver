#ifndef DIMACS_h
#define DIMACS_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// nó para cada literal 
typedef struct literal_node 
{
    int value;
    struct literal_node* next;
} literal_node;

/* clause (clausula) representa uma única cláusula do nosso arquivo cnf com alguns literais booleanos. 
na lógica booleana, eles são valores representativos para verdadeiro ou falso.*/
typedef struct clause 
{
    literal_node* literals; // literais booleanos 
    int size; // tamanho da nossa cláusula
    struct clause* next; // ponteiro para a próxima cláusula na lista encadeada

} clause;

typedef struct CNF
{
    clause* clauses; // todas as nossas cláusulas
    int total_clauses; // número total de cláusulas
    int total_literals; // número total de literais
} CNF;

#endif
