<div align="center">
  <br>
  <strong>UNIVERSIDADE FEDERAL DE ALAGOAS - UFAL</strong><br>
  <strong>INSTITUTO DE COMPUTAÇÃO - IC</strong><br>
  Disciplina: Estrutura de Dados (ED)<br>
  Professor: Dr. Márcio Ribeiro
</div>

<hr>

# Projeto Final: SMT Solver Dinâmico em C

<div align="center">
</div>

## Resumo do Projeto

Este repositório contém a implementação de um resolvedor SMT (*Satisfiability Modulo Theories*) desenvolvido na linguagem de programação C. O sistema é projetado para processar arquivos de entrada baseados no formato padronizado **DIMACS CNF** (Conjunctive Normal Form), determinando a existência de uma atribuição lógica que satisfaça o conjunto de cláusulas fornecido enquanto mapeia a teoria de Aritmética Linear Inteira (LIA).

O principal objetivo técnico deste projeto foi a aplicação prática de estruturas de dados dinâmicas para otimização de memória e robustez do *software*.

## Detalhes da Implementação

A solução proposta destaca-se pela superação de limitações de *arrays* estáticos através do uso exclusivo de alocação dinâmica de memória. As principais implementações incluem:

1.  **Representação Dinâmica do Problema (DIMACS Estendido):** Utilização de **Listas Encadeadas Duplas** (nós de literais dentro de nós de cláusulas) para armazenar o problema CNF. Além disso, o leitor foi adaptado para armazenar expressões matemáticas em vetores dinâmicos de *strings* associados às variáveis. Esta abordagem elimina os limites rígidos de tamanho de cláusulas ou quantidade de literais anteriormente impostos por *buffers* estáticos, garantindo que o programa suporte arquivos de complexidades variadas sem risco de *overflow*.
2.  **Algoritmo de Resolução:** Implementação de uma **Árvore Binária de Decisão** construída dinamicamente. O SMT Solver explora o espaço de soluções recursivamente através da árvore (`resposta_smt`), utilizando técnicas de *backtracking* para retroceder em atribuições insatisfatíveis e integrando uma checagem de teoria utilizando Árvores Sintáticas Abstratas (`expmat`).

## Estrutura do Código Fonte

* `DIMACS.h`: Definição dos tipos abstratos de dados (`literal_node`, `clause`, `CNF`) baseados em nós dinâmicos.
* `readfilecnf.c`: Rotinas de *parsing* e tratamento de arquivos, garantindo a manipulação correta de ponteiros de arquivo, a construção das listas encadeadas e a leitura das equações da teoria LIA ao final do arquivo.
* `SMT_solver.h` / `SMT_solver.c`: Definição e implementação da árvore binária de decisão, das estruturas da teoria (`teoria_smt`, `expmat`) e da lógica de satisfatibilidade.
* `Main.c`: Ponto de entrada do sistema, integrando os módulos de leitura e resolução, além de executar a liberação de memória.

## Compilação e Execução

Para a compilação do projeto, recomenda-se a utilização de um compilador C padrão (como o GCC). O processo pode ser realizado através do terminal:

```bash
gcc Main.c readfilecnf.c SMT_solver.c -o smtsolver
```

Equipe Desenvolvedora

```Este projeto foi realizado pelas seguintes discentes:
Ana Carolina Cavalcante de Jesus

Maria Luisa Silva Nunes de Souza

Sophia Byernes Carvalho Duarte
