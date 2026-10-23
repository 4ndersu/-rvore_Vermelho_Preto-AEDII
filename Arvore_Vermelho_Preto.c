#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// Declaração antecipada: usada em rb_check antes de ser definida
int int_comp(const void *a, const void *b);

//Cores possíveis de um nó
typedef enum Color { 
    RED, BLACK //Vermelho e preto
} Color;

//Definição da estrutura do nó
typedef struct Node{
    Color color;
    int data;//valor do nó
    //ponteiros para os filhos e o pai do nó
    struct Node* left;
    struct Node* right;
    struct Node* parent;
} Node;

//Definição da estrutura da árvore vermelho-preto
typedef struct {
    Node *root;
    Node *nil; //nó sentinela para representar os nós nulos
} RBTree;

//Função para alocar memória para uma arvore vermelho-preto
RBTree* rbTreeAlloc(){
    //Aloca memória para a árvore
    RBTree *T = malloc(sizeof(RBTree));
    //Verifica se a alocação foi bem-sucedida
    if(T){
        //Aloca memória para o nó sentinela nil
        T->nil = malloc(sizeof(Node));
        T->nil->data = 0;
        T->nil->color = BLACK; //nó nil é sempre preto
        //nó nil aponta para si mesmo
        T->nil->left = T->nil; 
        T->nil->right = T->nil;
        T->nil->parent = T->nil;
        //raiz da árvore aponta para o nó nil
        T->root = T->nil;
    }
    //Retorna o ponteiro para a árvore alocada
    return T;
}

//Função para alocar memória para um novo nó
Node* nodeAlloc(RBTree* T, int data){
    Node *novo = malloc(sizeof(Node));
    if(novo){
        novo->data = data;
        novo->color = RED;//novo nó é sempre vermelho
        //ponteiros do novo nó apontam para o nó nil da árvore
        novo->left = T->nil;
        novo->right = T->nil;
        novo->parent = T->nil;
    } 
    return novo;
}

//Função para liberar a memória de um nó
void nodeFree(Node* node){
    free(node);
}

//Função para liberar a memória da árvore vermelho-preto recursivamente
void rbTreeFreeRec(RBTree* T, Node* node){
    //Verifica se o nó não é o nó nil e não é nulo
    if(node!= T->nil && node!=NULL){
        rbTreeFreeRec(T, node->left);
        rbTreeFreeRec(T, node->right);
        nodeFree(node);
    }
}

//Função para liberar árvore da memória
void rbTreeFree(RBTree* T){
    //Se a árvore existir
    if(T){
        //remover recursivamente
        rbTreeFreeRec(T, T->root);
        //Libera o nó sentinela se tiver sido alocado
        if(T->nil){
            free(T->nil);
        }
        //Por fim libera a árvore
        free(T);
    }
}

//Função para contar a quantidade de nós pretos de um nó até a folha
int bh(RBTree* T, Node* node){
    int bheight = 0;
    Node *atual = node;

    if(atual == T->nil)
        return 0;

    atual = atual->left;

    //Desce pela esquerda contando os nós pretos
    while (atual != T->nil)
    {
        if(atual->color == BLACK)
            bheight++;
        
        atual = atual->left;
    }

    //conta a raiz preta, que não é contada dentro do laço.
    bheight++;
    
    return bheight;
}

//Rotação à esquerda em node: o filho direito y sobe e node desce para ser o filho esquerdo de y.
void leftRotate(RBTree* T, Node *node){
    Node *y = node->right; //y será a nova raiz da subárvore

    node->right = y->left; //subárvore esquerda de y passa a ser filha direita de node

    //atualiza o pai de y->left
    if(y->left != T->nil)
        y->left->parent = node;
    
    //y assume o lugar de node na árvore
    y->parent = node->parent;

    //Ajusta o ponteiro do pai de node para apontar para y
    if(node->parent == T->nil)
        T->root = y; //node era a raiz
    else if(node == node->parent->left)
        node->parent->left = y;
    else
        node->parent->right = y;
    
    y->left = node; // node vira filho esquerdo de y
    node->parent = y;
}

//Rotação à direita: simétrica à rotação à esquerda.
//O filho esquerdo y sobe e node desce para ser o filho direito de y.
void rightRotate(RBTree* T, Node *node){
    Node *y = node->left; //y será a nova raiz da subárvore

    node->left = y->right;// subárvore direita de y passa a ser filha esquerda de node

    if(y->right != T->nil)
        y->right->parent = node;
    
    y->parent = node->parent; //y assume o lugar de node na árvore

    //Ajusta o ponteiro do pai de node para apontar para y
    if(node->parent == T->nil)
        T->root = y; //node era a raiz
    else if(node == node->parent->right)
        node->parent->right = y;
    else
        node->parent->left = y;
    
    y->right = node; //node vira filho direito de y
    node->parent = y;
}


//Funções de Inserção

//Função para restaurar as propriedades vermelho-preto após inserir o nó vermelho z.
//Variáveis: z = nó atual, z->parent = pai, z->parent->parent = avô, y = tio.
void rbInsertFixUp(RBTree *T, Node* z) {

    //Enquanto o a cor do pai for vermelho
    while (z->parent->color == RED) {
        //Se o pai de z é o filho esquerdo do avô
        if (z->parent == z->parent->parent->left) {
            Node *y = z->parent->parent->right; //O tio está à direita
            
            //CASO 1: O tio é VERMELHO -> apenas recolore
            //pai e tio viram pretos, avô vira vermelho e o problema sobe
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Sobe o ponteiro z para o avô
            } else {
                //CASO 2: O tio é PRETO e z é filho direito (Zig-Zag)
                //Rotaciona para transformar no caso 3 (linha reta)
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(T, z);
                }
                //CASO 3: O tio é PRETO e z é filho esquerdo (Linha reta)
                //Recolore e rotaciona o avô; o laço termina em seguida
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(T, z->parent->parent);
            }
        } 
        //CASO SIMÉTRICO: O pai de z é o filho direito do avô
        else {
            Node *y = z->parent->parent->left; //O tio está à esquerda
            
            //CASO 1: O tio é VERMELHO -> apenas recolore
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; //Sobe o ponteiro z para o avô
            } else {
                //CASO 2: O tio é PRETO e z é filho esquerdo (Zig-Zag)
                //Rotaciona para transformar no caso 3 (linha reta)
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(T, z);
                }
                //CASO 3: O tio é PRETO e z é filho direito (Linha reta)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(T, z->parent->parent);
            }
        }
    }
    // A raiz deve sempre ser PRETA
    T->root->color = BLACK;
}
//Função para inserir a chave "key" na árvore
//inserção normal de BST, com o novo nó vermelho, também corrige com o insert_fixup
void rb_insert(RBTree* T, int key) {
    Node *z = nodeAlloc(T, key);//novo nó vermelho
    Node *y = T->nil;//y acompanha o pai de x
    Node *x = T->root;//x percorre a árvore

    //Desce até encontrar a posição nil onde z deve ficar
    while (x != T->nil) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;    
    }
    
    //Pai de z passa a ser y
    z->parent = y;

    //Liga z ao pai: raiz, filho esquerdo ou filho direito
    if (y == T->nil)
        T->root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    //Filhos do novo nó apontam para nil e ajusta sua cor para vermelho
    z->left = T->nil;
    z->right = T->nil;
    z->color = RED;

    //Conserta possíveis violações das propriedades da árvore vermelho-preto
    rbInsertFixUp(T, z);
}

//Função para substituir a subárvore com raiz em u pela subárvore com raiz em v
//ajusta o pai de u e o pai de v e quem chama cuida dos filhos de v
void rb_transplant(RBTree* T, Node* u, Node* v){
    if(u->parent == T->nil)
        T->root = v; //u era a raiz
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

//Função auxiliar para encontrar o nó com o menor valor a partir de um nó x
Node* rbMin(RBTree* T, Node* x) {
    while (x->left != T->nil) {
        x = x->left;
    }
    return x;
}

//Restaura as propriedades após a remoção de um nó preto.
//x carrega um preto extra que precisa ser absorvido
void rbDeleteFixup(RBTree* T, Node* x) {
    
    Node* w;//w é o irmão de x

    while (x != T->root && x->color == BLACK) {

       //x é filho esquerdo
        if (x == x->parent->left) {
            w = x->parent->right;

            //Caso 1: O irmão w é VERMELHO
            //Recolore e rotaciona para cair nos casos 2, 3 ou 4 (w preto)
            if (w->color == RED) {
                w->color = BLACK;                 
                x->parent->color = RED;           
                leftRotate(T, x->parent);         
                w = x->parent->right;             
            }

            //Caso 2: Ambos os filhos do irmão w são PRETO
            //Pinta w de vermelho e empurra o preto extra para o pai
            if (w->left->color == BLACK && w->right->color == BLACK) { 
                w->color = RED;                   
                x = x->parent;                 
            } 
            else {
                //Caso 3: O filho direito de w é PRETO (e o esquerdo é VERMELHO)
                //Rotaciona w para transformar no caso 4
                if (w->right->color == BLACK) {   
                    w->left->color = BLACK;      
                    w->color = RED;              
                    rightRotate(T, w);         
                    w = x->parent->right;       
                }

                //Caso 4: O filho direito de w é VERMELHO
                //Rotação final que absorve o preto extra; encerra o laço
                w->color = x->parent->color;     
                x->parent->color = BLACK;        
                w->right->color = BLACK;         
                leftRotate(T, x->parent);  
                x = T->root;                      
            }
        } 
        //CASO SIMÉTRICO: x == x.p.direita
        else {
            w = x->parent->left; // Irmão de x (à esquerda)

            //Caso 1 simétrico
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(T, x->parent);
                w = x->parent->left;
            }

            //Caso 2 simétrico
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                //Caso 3 simétrico
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(T, w);
                    w = x->parent->left;
                }

                //Caso 4 simétrico
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(T, x->parent);
                x = T->root;
            }
        }
    }
    
    //Absorve o preto extra ou garante raiz preta
    x->color = BLACK;
}

//Função para remover da árvore o nó z
void rbDelete(RBTree* T, Node* z) {
    Node* y = z;                            
    Color y_cor_original = y->color;    //guarda a cor de y antes de mexer   
    Node* x;

    if (z->left == T->nil) {    
        //Sem filho esquerdo: o filho direito toma o lugar de z            
        x = z->right;                       
        rb_transplant(T, z, z->right);       
    } 
    else if (z->right == T->nil) {    
        //Sem filho direito: o filho esquerdo toma o lugar de z     
        x = z->left;                     
        rb_transplant(T, z, z->left);       
    } 
    else {
        // Dois filhos: o sucessor menor da subárvore direita toma o lugar de z                                  
        y = rbMin(T, z->right);       
        y_cor_original = y->color;          
        x = y->right;                       
        
        if (y->parent == z) {    
            //Garante que x->parent seja válido mesmo se x for nil           
            x->parent = y;                 
        } else {    
            //Remove y de sua posição e põe o filho direito de y no lugar                        
            rb_transplant(T, y, y->right);  
            y->right = z->right;            
            y->right->parent = y;           
        }
        
        //y substitui z, herda seu filho esquerdo e sua cor
        rb_transplant(T, z, y);             
        y->left = z->left;                  
        y->left->parent = y;               
        y->color = z->color;                
    }

    //Se o nó removido/movido era preto, a black-height foi afetada: corrige
    if (y_cor_original == BLACK) {          
        rbDeleteFixup(T, x);                
    }

    nodeFree(z);                            // Libera a memória do nó removido
}

//Função de busca iterativa que retorna o nó com a chave key ou T->nil se não existir
Node* rb_search(RBTree* T, Node* x, int key) {
    while (x != T->nil && key != x->data) {
        if (key < x->data)
            x = x->left;
        else
            x = x->right;
    }
    return x;
}

//Função para retornar o nó com o maior valor da subárvore de x, o mais à direita
Node* rbMax(RBTree* T, Node* x) {
    while (x->right != T->nil)
        x = x->right;
    return x;
}

//Função para contar o número de nós da subárvore de x recursivamente
int rb_size(RBTree* T, Node* x) {
    if (x == T->nil)
        return 0;
    return 1 + rb_size(T, x->left) + rb_size(T, x->right);
}

//Função para armazenar os valores em arr em ordem crescente
//index é a próxima posição livre do vetor e compartilhada entre as chamadas
void rb_store(RBTree* T, Node* nd, int *arr, int *index) {
    if (nd != T->nil) {
        rb_store(T, nd->left, arr, index);
        arr[(*index)++] = nd->data;
        rb_store(T, nd->right, arr, index);
    }
}

//Função para imprmir a árvore em ordem cada nó como valor + cor (V = vermelho, P = preto)
void rb_printRec(RBTree* T, Node* nd) {
    if (nd != T->nil) {
        rb_printRec(T, nd->left);
        printf("%02d%c ", nd->data, nd->color == RED ? 'V' : 'P');
        rb_printRec(T, nd->right);
    }
}

//Imprime a árvore em uma linha em ordem crescente
void rb_print(RBTree* T) {
    printf("RBT: [ ");
    rb_printRec(T, T->root);
    printf("]\n");
}

//Imprime a árvore "deitada" (raiz à esquerda): primeiro a subárvore direita,
//depois o nó, depois a esquerda; space controla a indentação por nível
void rb_printTreeRec(RBTree* T, Node* root, int space) {
    if (root != T->nil) {
        space += 6;

        rb_printTreeRec(T, root->right, space);

        printf("\n");
        for (int i = 6; i < space; i++)
            printf(" ");

        printf("%02d(%c)\n", root->data, root->color == RED ? 'V' : 'P');

        rb_printTreeRec(T, root->left, space);
    }
}

//Interface para imprimir a árvore em formato de árvore
void rb_printTree(RBTree* T) {
    rb_printTreeRec(T, T->root, 0);
}

/*------------------------------------------------------------------------------
 * Verificação das propriedades da árvore vermelho-preto
 */

//Verifica se a raiz é preta(propriedade 2)
bool rb_checkRoot(RBTree* T) {
    return T->root == T->nil || T->root->color == BLACK;
}

//Verifica se nenhum nó vermelho tem filho vermelho(propriedade 4)
bool rb_checkRedChildren(RBTree* T, Node* nd) {
    if (nd == T->nil)
        return true;
    if (nd->color == RED) {
        if (nd->left->color == RED || nd->right->color == RED)
            return false;
    }
    return rb_checkRedChildren(T, nd->left) && rb_checkRedChildren(T, nd->right);
}

//Verifica se todos os caminhos têm a mesma black-height (propriedade 5)
//bh_count acumula os nós pretos do caminho; ao chegar em nil compara com o esperado
bool rb_checkBlackHeight(RBTree* T, Node* nd, int bh_count, int expected_bh) {
    if (nd == T->nil)
        return bh_count == expected_bh;
    if (nd->color == BLACK)
        bh_count++;
    return rb_checkBlackHeight(T, nd->left, bh_count, expected_bh) &&
           rb_checkBlackHeight(T, nd->right, bh_count, expected_bh);
}

//Verifica todas as propriedades RB + conteúdo dos dados
//data são os N valores que deveriam estar na árvore (em qualquer ordem)
bool rb_check(RBTree* T, const int * const data, const int N) {
    const int tsize = rb_size(T, T->root);

    // Quantidade de nós precisa bater com o esperado
    if (tsize != N)
        return false;
    if (N == 0)
        return rb_checkRoot(T);

    // Propriedades estruturais
    if (!rb_checkRoot(T))
        return false;
    if (!rb_checkRedChildren(T, T->root))
        return false;

    int expected_bh = bh(T, T->root);
    if (!rb_checkBlackHeight(T, T->root, 0, expected_bh))
        return false;

    //Conteúdo: ordena uma cópia dos dados esperados
    int *arr = (int *)malloc(sizeof(int) * N);
    memcpy(arr, data, sizeof(int) * N);
    qsort(arr, N, sizeof(int), int_comp);

    //e extrai os valores da árvore em ordem crescente
    int *tarr = (int *)malloc(sizeof(int) * tsize);
    int index = 0;
    rb_store(T, T->root, tarr, &index);

    //Os dois vetores ordenados devem ser idênticos
    bool match = true;
    for (int i = 0; i < N && match; i++) {
        if (arr[i] != tarr[i])
            match = false;
    }

    //Libera os valores auxiliares da memória
    free(tarr);
    free(arr);

    //Retorna se é uma árvore vermelho-preto
    return match;
}

/*------------------------------------------------------------------------------
 * Permutação de um Arranjo
 */

//Troca os valores apontados por a e b
void swap(int *a, int *b) {
    const int temp = *a;
    *a = *b;
    *b = temp;
}

//Inverte o trecho do vetor entre as posições inicio e fim
void perm_invert(int *arr, int inicio, int fim) {
    while (inicio < fim) {
        swap(&arr[inicio], &arr[fim]);
        inicio++;
        fim--;
    }
}

//Gera a próxima permutação em ordem lexicográfica
//retorna false quando o vetor já está na última permutação
bool perm_next(int *arr, int tamanho) {
    int i = tamanho - 2;

    while (i >= 0 && arr[i] >= arr[i + 1])
        i--;

    if (i < 0)
        return false;

    int j = tamanho - 1;
    while (arr[j] <= arr[i])
        j--;

    swap(&arr[i], &arr[j]);
    perm_invert(arr, i + 1, tamanho - 1);
    return true;
}

//Comparador de inteiros para o qsort para ordem crescente
int int_comp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/*------------------------------------------------------------------------------
 * Funções Auxiliares
 */

//Imprime o vetor de dados
void data_print(const int * const data, const int N) {
    printf("data: [ ");
    for (int i = 0; i < N; i++)
        printf("%02d ", data[i]);
    printf("]\n");
}

//Remove todas as ocorrências de value do vetor, desloca seus elementos
//seguintes para a esquerda e retorna o novo tamanho do vetor
int arr_remove(int *arr, int N, int value) {
    for (int i = 0; i < N; i++) {
        if (arr[i] == value) {
            for (int j = i; j < N - 1; j++)
                arr[j] = arr[j + 1];
            N--;
            i--;//reexamina a posição i, que agora tem um novo elemento
        }
    }
    return N;
}

/*------------------------------------------------------------------------------
 * Programa Principal - Bateria de Testes
 *
 * Testa TODAS as combinações de (permutação de inserção) x (permutação de
 * remoção) para os valores 1..N. Após cada operação, o assert confirma que a
 * árvore ainda respeita as propriedades vermelho-preto e tem os dados corretos.
 */

int main() {
    int DATA_INSERT[] = {10, 23, 39, 50, 67};
    int DATA_REMOVE[] = {10, 23, 39, 50, 67};
    const int N = 5;

    //Vetor de inserção começa na primeira permutação (ordem crescente)
    int *data_insert, *data_remove;

    data_insert = (int *)malloc(sizeof(int) * N);
    memcpy(data_insert, DATA_INSERT, sizeof(int) * N);

    //Laço externo: percorre todas as permutações de inserção
    int total_testes = 0;

    do {
        //Para cada ordem de inserção, reinicia as permutações de remoção
        data_remove = (int *)malloc(sizeof(int) * N);
        memcpy(data_remove, DATA_REMOVE, sizeof(int) * N);
        
        //Laço interno: percorre todas as permutações de remoção
        do {
            RBTree *T = rbTreeAlloc();

            printf("============================================\n");
            printf("Dados para Insercao:\n\t");
            data_print(data_insert, N);

            //Fase de inserção: verifica a árvore após cada inserção
            for (int i = 0; i < N; i++) {
                printf("Inserindo: %02d\n", data_insert[i]);
                rb_insert(T, data_insert[i]);
                rb_print(T);
                rb_printTree(T);
                //Compara a árvore com os i + 1 primeiros valores de data_insert, que são os que já foram inseridos
                assert(rb_check(T, data_insert, i + 1));
            }

            printf("Arvore apos todas as INSERCOES:\n");
            rb_printTree(T);

            printf("Dados para Remocao:\n\t");
            data_print(data_remove, N);

            //arr guarda os valores que ainda devem estar na árvore
            int *arr = (int *)malloc(sizeof(int) * N);
            int asize = N;
            memcpy(arr, data_insert, sizeof(int) * asize);

            //Fase de remoção: busca, remove e verifica a cada passo
            for (int i = 0; i < N; i++) {
                printf("Removendo: %02d\n", data_remove[i]);
                Node *nd = rb_search(T, T->root, data_remove[i]);
                //Confere que o valor a remover existe na árvore.
                assert(nd != T->nil);//Sem ele, rb_search devolveria T->nil e o rbDelete receberia um nó inválido
                rbDelete(T, nd);
                rb_print(T);
                rb_printTree(T);
                asize = arr_remove(arr, asize, data_remove[i]);
                //compara a árvore com o array arr, de onde o arr_remove vai tirando cada valor removido, arr é a resposta certa do que deveria sobrar
                assert(rb_check(T, arr, asize));
            }

            printf("Arvore apos todas as REMOCOES:\n");
            rb_printTree(T);

            free(arr);
            rbTreeFree(T);

            total_testes++;

        } while (perm_next(data_remove, N)); //próxima ordem de remoção

        free(data_remove);

    } while (perm_next(data_insert, N));//próxima ordem de inserção

    free(data_insert);

    printf("\n========================================\n");
    printf("TODOS OS %d TESTES PASSARAM!\n", total_testes);
    printf("========================================\n");

    return EXIT_SUCCESS;
}
