#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

partial_interpretation inicio_partial_interpretation(CNF *formula){//a função chama struct cnf como parametro e aloca memória para a formula, além de percorrer a lista e declara como undefined e retornamos a variavel criada 
    partial_interpretation new_options;
    
    new_options.atributions = malloc(sizeof(int) *(formula->total_literals + 1));

    for (int i = 0; i < formula->total_literals + 1; i++){
        new_options.atributions[i] = UNDEFINED;
    }
    return new_options;
}
/*essa função retorna um booleano 
recebe ponteiro para formula e a interpretação parcial (vou usar ponteiro para a interpretação mas poderia ser apenas a variavel(nesse caso faria uma copia da estrutura para usar na funçao))
verifica se a formula é satisfativel com base nos valores da interpretação parcial
considere: para a formula ser verdadeira todas as clausulas precisam ser verdadeiras, para a clausula ser verdadeiras pelo menos um literal seja verdadeiro*/
bool eh_sat(CNF *formula, partial_interpretation *options){
    clause * clauses_now = formula->clauses; //primeira clausula da forumula

    while(clauses_now != NULL){ //vamos navegar pelas clausulas enquanto houver
        bool satisfeita = false; //é falso até que se prove o contrario
        literal_node * literal_now = clauses_now->literals; //primeiro literal da clausula

        while(literal_now != NULL){ //navega pelos literais
            int valores = options->atributions[abs(literal_now->value)]; //busca no array qual valor foi atribuido na variavel (usa o abs pois se a variavel vier negada sera com o sinal -, entao o modulo tira esse sinal para que possamos acessar o array corretamente))

            if(valores !=UNDEFINED){
                if((literal_now->value > 0 && valores == 1) || (literal_now->value < 0 && valores == 0)){ //se o numero do literal for maior que zero e a atribuicao for verdadeira (1) ou se o numero do literal for menor que zero e a atribuição for falsa retorna verdadeira 
                    satisfeita = true; // a clausula foi atendida, pelo menos um literal é verdadeiro
                    break; //interrompe o loop de literais da clausula
                }
            }
            literal_now = literal_now->next; //se o literal nao for verdadeiro vai para o proximo
        } 
            if(!satisfeita) // se nenhum literal for verdadeiro, retorna falso
            {
                return false;
            }
        clauses_now = clauses_now->next; // proxima clausula
    }   
    return true; //se saiu do loop sem dar "return false" é verdadeira
}
/*a funcao vai retornar booleano tb, chama os msms parametros da eh_sat*/
bool eh_unsat(CNF *formula, partial_interpretation *options){/*vamos verificar se tem algum caso completamente falso*/
/*novamente iniciamos c a primeira clausula e o ponteiro apontará p ela*/
clause *clauses_now = formula->clauses;

while(clauses_now != NULL){
    /*se presume q ela seja falsa até q se prove verdadeira e chamamos a lista encadeada atual que será analisada*/
    bool clause_still_valid = false;
    literal_node *literal_now = clauses_now->literals;

    while (literal_now != NULL){/*ou seja, enquanto tiver literais pra analise dentro dessa clausula*/
        int valores = options->atributions[abs(literal_now->value)];/*novamnete, como no eh_sat, vamos usar o abs p considerar o modulo caso a atribuicao seja negativa*/
    
    if (valores == UNDEFINED){/*se tiver valoresw indefinidos, nao foram testados ainda, ent podem ser verdadeiros*/
        clause_still_valid = true;
        break;/*mantem a calsula por enquanto e quebra o loop*/
    }

    /*esse if diz que se o literal for >0 e verdadeiro (1) ou se o literal for <0 e falso, entao a calsula eh vdd e corta o loop*/
    if((literal_now->value > 0 && valores == 1) || (literal_now->value < 0 && valores == 0)){
        clause_still_valid = true;
        break;
    }
        literal_now = literal_now->next;/*se tiver acusado falso, pula para o prox literal*/
}
    if(!clause_still_valid){
        return true;/*se dps de tudo, ainda continua falsa, eh um caso insoluvel e retorna true pq a formula toda eh unsat nesse caminho*/
    }
    clauses_now = clauses_now->next; /*avanca p proxima clasula*/
}
return false; //se percorreu tudo e nao se deu total falsa, nao da p dizer q é unsat (pode ser indefinida e depender de outras coisas)
}

/*vms unir o sat e o unsat pra poder a arvore ter onde percorrer ou saber onde deve voltar*/
partial_interpretation unir(partial_interpretation *inicio, int total_literals, int value, int xi){
    /*repito o processo da primeira funcao, mas agr uso o valor inicial e aplico 0 ou q na variavel xi*/
    partial_interpretation new_atributions;/*crio a variavel nova e em seguida aloco memoria no array interno*/
    
    new_atributions.atributions = malloc(sizeof(int) *(total_literals + 1));

    for (int i = 0; i < total_literals + 1; i++){/*os dados de inicio vao pra o new_atributions*/
        new_atributions.atributions[i] = inicio->atributions[i];
    }
    new_atributions.atributions[xi] = value;/*definir o valor na variavel que ta sendo testada*/
    return new_atributions;//retorna p nova estrutura

}
/*aqui checamos as arvores matematicas*/
bool checagem(partial_interpretation *options, teoria_smt *teoria){

    return true;
}
tree *resposta_smt(CNF *formula, partial_interpretation now, teoria_smt *teoria){
    tree *node = (tree*)malloc(sizeof(tree));/*alocar memoria pro no atual*/
    /*pra começar, inicializamos os dois lados, esquerda e direita, com NULL*/
    node->left = NULL;
    node->right = NULL;


    if(eh_sat (formula, &now)){/*se o SAT deu verdadeiro, vemos se a mametatica tb da*/
        if(checagem (&now, teoria) == true){
        node->value = 1;
    }//ou seja a logica tb deu vdd, 
        else{
            return node;/*retorna o nó*/
        }
    }

    if(eh_unsat(formula, &now)){/*usando o msm raciocinio, vamos considerar que nao seja satisfeita a formula*/
    /*retornarmos o nó e definimos o valor como zero, ou seja erro/falha/falso*/
    node->value = 0;
    return node;
    }

    int xi = -1; /*declarando xi = -1 garantimos que nao assuma valores de lixo de memória, é tb um valor impossível para o nosso problema, o q facilita a resolucao de problemas */

    /*criamos um array para percorrer todos os casos que nao estao dentro do caso base 1 ou 2*/
    for (int i = 1; i <= formula->total_literals; i++){/*o array vai bater todas as atribuicoes e ele comeca com i = 1 pois estamos usando variaveis x1, x2... nao exisitndo variavel x0, ja q o zero significa o fim de linha clausula e ao considerar, no malloc, o total_literals + 1, a gnt deixa o indice 0 vazio*/ 
        if (now.atributions[i] == UNDEFINED){
            xi = i; /*guardamos o indice em xi e interrompemos o loop pq achamos um valor pra ser testado*/
            break;
        }
    }
        if(xi == - 1){/*coloquei esse if pq se xi for -1, temos q sair antes de salvar o valor inválido do nó e antes de chamar unir*/
            node->value = 0;
            return node;
        }

        node->variable = xi;/*assim, garantimos q chegamos aqui c um xi válido. aqui a gnt salva a variavel excolhida p fzr a analise direita e esquerda*/

    partial_interpretation L_true = unir(&now, formula->total_literals, 1, xi);/*criamos, entao uma nova funcao atribuindo a xi o valor de vdd (1)*/
    node->left = resposta_smt(formula, L_true, teoria);/*acao recursiva pra esquerda*/
    free(L_true.atributions);

    /*mesma coisa p lado direito, mas considerando q seja falso, uma vez q as opcoes sao apenas vdd ou falso*/
    partial_interpretation R_false = unir(&now, formula->total_literals, 0,xi);
    node->right = resposta_smt(formula, R_false, teoria);
    free(R_false.atributions);
    /*sendo o nó o resultado da uniao dos filho, se a esquerda ou a direita der 1, ent o nó resulta em 1*/
    node->value = node->left->value || node->right->value;
    
    return node;
}

void free_tree (tree *node){/*criei essa void pq o free() libera um bloco por vez, ou seja, daria memory leak pq nao iria percorrer tds os nos da arvore, liberaria so a raiz. a free_tree desce recursivamente e sobe liberando nó por nó*/
        if (node == NULL){
            return;
        }

        free_tree(node->left);
        free_tree(node->right);
        free (node);
    }

void free_cnf(CNF *formula){/*com o msm raciocínio, essa void percorre as duas listas e libera cada no antes de liberar a struct externa, todas as clausulas e evita memory leak*/
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

/*função imprimir recebe a árvore e o total de literais
faz uma busca de profundidade*/
bool imprimir(tree *node, int total_literals) 
{
    if(node == NULL) //se o no da raiz é nulo retorna falso
    {
        return false;
    }

    //se for uma folha (nao tem filhos) e for sat, executa esse if
    if(node->value == 1 && node->left == NULL && node->right == NULL)  
    {
        printf("Configuracao encontrada:\n");
        return true; 
    }

    if(node->left != NULL && node->left->value == 1){/*nao sendo uma folha, vms analisar o lado esquerdo (1) */
        if (imprimir(node->left, total_literals)){
            if (node->variable != -1){
                printf("%d = 1\n", node->variable); /*foi p esquerda, a variavel atual foi testada como 1*/
            } 
            return true;
        }
    }
    
    if(node->right != NULL && node->right->value == 1){/*nao sendo uma folha, vms analisar o lado direito (0) */
        if (imprimir(node->right, total_literals)){
            if (node->variable != -1){
                printf("%d = 0\n", node->variable);/*foi p direita, a variavel atual foi testada como 0*/
            }
            return true;
        }
    }
    
    return false;
}
