#include "DIMACS.h"
#include "SMT_solver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

partial_interpretation inicio_partial_interpretation(CNF *formula){//a função chama struct cnf como parametro e aloca memória para a formula, além de percorrer a lista e declara como undefined e retornamos a variavel criada 
    partial_interpretation new_options;
    
    new_options.atributions = malloc(sizeof(int) *(formula->total_literals + 1));

    for (int i = 0; i < formula->total_literals + 1; i++){
        new_options.atributions[i] = UNDEFINED;
    }
    return new_options;
}

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
/*aqui vai substituir a funcao L_valor_exp do codigo original e precisamos de uma funcao q transforme a string numa struct que consiga ler os coeficientes, operador e a constante
as variaveis serao escritas como x<indice>, o coeficiente q nn aparece é 1 (tipo"X1 = 1*X1), o sinal negativo vai direto e os espaços sao pulados(e ignorados)*/

static const char *ler_operador(const char *s, Operador *op){/*por ser static, nn aparece fora desse arquivo, o q deixa o nome livre p ser assumido por outra funcao enquanto, msm assim, cumpre com os aspectos determinados aqui*/
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

/*agr vms receber a string e preencher a struct expressaoLIA*/
bool parse_expressao_lia(const char *entrada, ExpressaoLIA *out, int n_vars){
    memset(out, 0, sizeof(*out));
    out->n_vars = n_vars;

    const char *caractere = entrada;

    /*le o lado esquerdo: termos como 2*x1, x2, -3*x2 */
    while (*caractere){
        while (*caractere == ' ') caractere++; /*pula espacos*/

        /*chegou no operador: lado esquerdo acabou*/
        if (*caractere == '<' || *caractere == '>' || *caractere == '=' || *caractere == '!'){
            break;
        }
        if (*caractere == '\0'){
            break;
        }

        /*le o sinal do termo*/
        int sinal = 1;
        if(*caractere == '+'){
            sinal =  1;
            caractere++;
        }
        else if (*caractere == '-'){
            sinal = -1;
            caractere++;
        }

        while (*caractere == ' '){
            caractere++;
        }

        /*le o coeficiente numerico*/
        int coef = 0;
        bool tem_coef = false;
        while (isdigit(*caractere)){
            coef = coef * 10 + (*caractere - '0');
            tem_coef = true;
            caractere++;
        }

        /*pula o '*' entre coeficiente e variavel*/
        if (*caractere == '*'){
            caractere++;
        } 

        /*le a variavel: deve ser x<indice>*/
        if (*caractere != 'x'){
            fprintf(stderr, "Esperava 'x<n>', recebi '%c'\n", *caractere);
            return false;
        }
        caractere++;

        int indice = 0;
        if (!isdigit(*caractere)){
            fprintf(stderr, "Indice ausente\n");
            return false;
        }
        while (isdigit(*caractere)){
            indice = indice * 10 + (*caractere - '0');
            caractere++;
        }

        if (indice < 1 || indice > MAX_VARS){
            fprintf(stderr, "Indice %d fora do intervalo\n", indice);
            return false;
        }

        if (!tem_coef) coef = 1; /*coeficiente omitido = 1*/
        out->coeficientes[indice - 1] += sinal * coef;
    }

    /*le o operador */
    while (*caractere == ' '){
        caractere++;
    } 

    const char *dps = ler_operador(caractere, &out->op);
    if (!dps){
        fprintf(stderr, "Operador nao encontrado\n");
        return false;
    }
    caractere = dps;

    /*le a constante no lado direito */
    while (*caractere == ' '){
        caractere++;
    } 

    int negativo = 1;
    if (*caractere == '-'){
        negativo = -1;
        caractere++; }
    else if (*caractere == '+'){
        caractere++; }

    int ladoD = 0;
    if (!isdigit(*caractere)){
        fprintf(stderr, "Constante ausente no lado direito\n");
        return false;
    }
    while (isdigit(*caractere)){
        ladoD = ladoD * 10 + (*caractere - '0');
        caractere++;
    }

    out->constante = negativo * ladoD;
    return true;
}

/*aqui checamos as expressoes matematicas, receberemos os termos ja parseados e vamos conferir se a expressao eh vdd*/
bool avalia_lia(const ExpressaoLIA *exp, int *valores){
    int soma = 0;

    for (int i = 0; i < exp->n_vars; i++){
        soma += exp->coeficientes[i] * valores[i];/*multiplica cada coeficiente pelo valor da sua variavel*/
    }
    switch (exp->op){
        case OP_MENORQ:      return soma <  exp->constante;
        case OP_MAIORQ:      return soma >  exp->constante;
        case OP_MENOR_IGUAL: return soma <= exp->constante;
        case OP_MAIOR_IGUAL: return soma >= exp->constante;
        case OP_IGUAL:       return soma == exp->constante;
        case OP_DIFERENTE:   return soma != exp->constante;
    }
    return false;/*se passar por todos os cases e nn retornar nd, vai retornar false*/
}

/*vms agr checar tudo: vms percorrer tds as restricoes e ver se houve alguma violacao, vai retornar -1 se tudo tiver ok e retorna o indice da restricao com problema caso tenha alguma violacao - o q ajuda a corrigir no backtracking*/
int checagem_lia(int *atrib_bool, TeoriaLIA *teoria){
    if (teoria == NULL || teoria->restricoes == NULL){
        return -1;/*sem teoria, nn tem o q checar*/
    }

    for (int i = 0; i < teoria->total; i++){
        if (atrib_bool[i + 1] != 1){
            continue;
        }
        if (!avalia_lia(&teoria->restricoes[i], teoria->valores_int)){
            return i; /*retorna qual foi a restricao causou o problema*/
        }
    }
    return -1; /*fora do for: nenhuma restricao foi violada*/
}
tree *no_da_arvore(){
    tree *nv_no = (tree*)malloc(sizeof(tree));
    nv_no->left = NULL;
    nv_no->right = NULL;
    nv_no->value = -1;/*-1 pq o resultado ainda nn foi definido*/
    nv_no->variable = -1; /*nenhuma variavel doi associada ainda*/
    return nv_no;
    }

tree *resposta_smt(CNF *formula, partial_interpretation now_interpretation, TeoriaLIA *teoria){
    tree *no_now = no_da_arvore();

    if(eh_sat(formula, &now_interpretation)){/*ver se a logica booleana eh vdd*/
    int conferelia = checagem_lia(now_interpretation.atributions, teoria);
    
        if (conferelia == -1){/*tanto o sat quanto a teoria nn tiveram problemas, ent a solucao foi encontrada*/
            no_now->value = 1;
            return no_now;
        }
    no_now->value = 0;
    no_now->variable = conferelia;
    return no_now;
}

    if(eh_unsat(formula, &now_interpretation)){/*ver se eh impossivel*/
        no_now->value = 0; /*eh impossivel*/
        return no_now;
    }

    int teste_variavel = -1;

    for(int i = 1; i <= formula->total_literals; i++){
        if (now_interpretation.atributions[i] == UNDEFINED){
            teste_variavel = i;
            break;/*vai pegar a primeira indefinida encontrada*/
        }
    }

    if(teste_variavel == -1){
        no_now->value = 0;
        return no_now;
    }

    no_now->variable = teste_variavel;

    partial_interpretation caso_vdd = unir(&now_interpretation, formula->total_literals, 1, teste_variavel);
    no_now->left = resposta_smt(formula, caso_vdd, teoria);
    free(caso_vdd.atributions);/*libera a copia apos ser usada*/

    partial_interpretation caso_falso = unir(&now_interpretation, formula->total_literals, 0, teste_variavel);
    no_now->right = resposta_smt(formula, caso_falso, teoria);
    free(caso_falso.atributions);

    // O resultado do nó é verdadeiro se pelo menos um dos lados for verdadeiro
    no_now->value = no_now->left->value || no_now->right->value;

    return no_now;
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