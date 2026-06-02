# Projeto Final: SMT Solver Dinâmico em C (Teoria LIA)

## Resumo do Projeto

Este repositório contém a implementação em progresso de um resolvedor SMT (*Satisfiability Modulo Theories*) desenvolvido na linguagem de programação C. O sistema utiliza uma abordagem híbrida: processa o núcleo lógico através de arquivos no formato **DIMACS CNF** estendido e vincula as variáveis lógicas a expressões matemáticas de Aritmética Linear Inteira (LIA), declaradas ao final do arquivo.

O principal objetivo técnico deste projeto é a aplicação prática de estruturas de dados dinâmicas para otimização de memória, resolução de satisfatibilidade booleana por meio de espaço de busca recursivo e a futura validação de restrições aritméticas, garantindo que o programa suporte arquivos de complexidades variadas sem risco de *overflow*.

## Detalhes da Implementação Atual

A solução proposta destaca-se pela superação de limitações de *arrays* estáticos através do uso exclusivo de alocação dinâmica de memória em todas as suas etapas. As principais implementações incluem:

1. **Representação Dinâmica do Problema (CNF Estendido):**
* Utilização de **Listas Encadeadas Duplas** (nós de literais dentro de nós de cláusulas) para armazenar o problema em formato CNF.
* O leitor foi adaptado para identificar linhas de equações (iniciadas por `e`) ao final do arquivo DIMACS, armazenando as expressões matemáticas associadas às variáveis lógicas através de ponteiros de *strings*.


2. **Algoritmo de Resolução:** Implementação de uma **Árvore Binária de Decisão** construída dinamicamente. O SMT Solver explora o espaço de soluções recursivamente através da árvore (`resposta_smt`), utilizando técnicas de *backtracking* para retroceder em atribuições insatisfatíveis.
3. **Gerenciamento de Memória:** Implementação de varredura profunda (`free_tree` e `free_cnf`) para liberação de cada nó alocado, prevenindo vazamentos de memória (*memory leaks*).

## Estrutura do Código Fonte

* `DIMACS.h`: Definição dos tipos abstratos de dados para o núcleo lógico (`literal_node`, `clause`, `CNF`) baseados em listas encadeadas.
* `readfilecnf.c`: Rotinas de *parsing* e tratamento de arquivos. Lê o cabeçalho DIMACS, monta a estrutura de cláusulas e armazena as *strings* da teoria matemática associada.
* `SMT_solver.h` / `SMT_solver.c`: Definição e implementação da árvore binária de decisão, contendo o núcleo de satisfatibilidade booleana (`eh_sat`, `eh_unsat`) e a base para a Árvore Sintática Abstrata (AST) da teoria matemática (`expmat`).
* `Main.c`: Ponto de entrada do sistema, integrando os módulos de leitura, resolução e limpeza de memória.

## Compilação e Execução

Para a compilação do projeto, recomenda-se a utilização de um compilador C padrão (como o GCC). O processo pode ser realizado através do terminal:

```bash
gcc Main.c readfilecnf.c SMT_solver.c -o smtsolver

```

Equipe Desenvolvedora

Este projeto foi realizado pelas seguintes discentes:

```
Ana Carolina Cavalcante de Jesus

Maria Luisa Silva Nunes de Souza

Sophia Byernes Carvalho Duarte

```
