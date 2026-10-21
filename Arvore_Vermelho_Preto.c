#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

int int_comp(const void *a, const void *b);

typedef enum Color { 
    RED, BLACK //cores dos nós da árvore vermelho-preto
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

int bh(RBTree* T, Node* node){
    int bheight = 0;
    Node *atual = node;

    if(atual == T->nil)
        return 0;

    atual = atual->left;

    while (atual != T->nil)
    {
        if(atual->color == BLACK)
            bheight++;
        
        atual = atual->left;
    }

    bheight++;
    
    return bheight;
}

void leftRotate(RBTree* T, Node *node){
    Node *y = node->right;

    node->right = y->left;

    if(y->left != T->nil)
        y->left->parent = node;
    
    y->parent = node->parent;

    if(node->parent == T->nil)
        T->root = y;
    else if(node == node->parent->left)
        node->parent->left = y;
    else
        node->parent->right = y;
    
    y->left = node;
    node->parent = y;
}

void rightRotate(RBTree* T, Node *node){
    Node *y = node->left;

    node->left = y->right;

    if(y->right != T->nil)
        y->right->parent = node;
    
    y->parent = node->parent;

    if(node->parent == T->nil)
        T->root = y;
    else if(node == node->parent->right)
        node->parent->right = y;
    else
        node->parent->left = y;
    
    y->right = node;
    node->parent = y;
}

void rbInsertFixUp(RBTree *T, Node* z) {
    while (z->parent->color == RED) {
        // Se o pai de z é o filho esquerdo do avô
        if (z->parent == z->parent->parent->left) {
            Node *y = z->parent->parent->right; // O tio está à direita
            
            // CASO 1: O tio é VERMELHO -> apenas recolore
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Sobe o ponteiro z para o avô
            } else {
                // CASO 2: O tio é PRETO e z é filho direito (Zig-Zag)
                if (z == z->parent->right) { // Corrigido: '==' em vez de '='
                    z = z->parent;
                    leftRotate(T, z);
                }
                // CASO 3: O tio é PRETO e z é filho esquerdo (Linha reta)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(T, z->parent->parent);
            }
        } 
        // CASO SIMÉTRICO: O pai de z é o filho direito do avô
        else {
            Node *y = z->parent->parent->left; // Corrigido: O tio está à esquerda
            
            // CASO 1: O tio é VERMELHO -> apenas recolore
            if (y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Sobe o ponteiro z para o avô
            } else {
                // CASO 2: O tio é PRETO e z é filho esquerdo (Zig-Zag)
                if (z == z->parent->left) { // Corrigido: '==' em vez de '='
                    z = z->parent;
                    rightRotate(T, z);
                }
                // CASO 3: O tio é PRETO e z é filho direito (Linha reta)
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(T, z->parent->parent);
            }
        }
    }
    // A raiz deve sempre ser PRETA
    T->root->color = BLACK;
}

void rb_insert(RBTree* T, int key) {
    Node *z = nodeAlloc(T, key);
    Node *y = T->nil;
    Node *x = T->root;

    while (x != T->nil) {
        y = x;
        if (z->data < x->data)
            x = x->left;
        else
            x = x->right;    
    }
    
    z->parent = y;

    if (y == T->nil)
        T->root = z;
    else if (z->data < y->data)
        y->left = z;
    else
        y->right = z;

    z->left = T->nil;
    z->right = T->nil;
    z->color = RED;

    rbInsertFixUp(T, z);
}

void rb_transplant(RBTree* T, Node* u, Node* v){
    if(u->parent == T->nil)
        T->root = v;
    else if(u == u->parent->left)
        u->parent->left = v;
    else
        u->parent->right = v;

    v->parent = u->parent;
}

// Função auxiliar para encontrar o nó com o menor valor a partir de um nó x
Node* rbMin(RBTree* T, Node* x) {
    while (x->left != T->nil) {
        x = x->left;
    }
    return x;
}

void rbDeleteFixup(RBTree* T, Node* x) {
    Node* w;

    while (x != T->root && x->color == BLACK) {

       
        if (x == x->parent->left) {
            w = x->parent->right;

            // Caso 1: O irmão w é VERMELHO
            if (w->color == RED) {
                w->color = BLACK;                 
                x->parent->color = RED;           
                leftRotate(T, x->parent);         
                w = x->parent->right;             
            }

            // Caso 2: Ambos os filhos do irmão w são PRETOs
            if (w->left->color == BLACK && w->right->color == BLACK) { 
                w->color = RED;                   
                x = x->parent;                 
            } 
            else {
                // Caso 3: O filho direito de w é PRETO (e o esquerdo é VERMELHO)
                if (w->right->color == BLACK) {   
                    w->left->color = BLACK;      
                    w->color = RED;              
                    rightRotate(T, w);         
                    w = x->parent->right;       
                }

                // Caso 4: O filho direito de w é VERMELHO
                w->color = x->parent->color;     
                x->parent->color = BLACK;        
                w->right->color = BLACK;         
                leftRotate(T, x->parent);  
                x = T->root;                      
            }
        } 
        // CASO SIMÉTRICO: x == x.p.direita
        else {
            w = x->parent->left; // Irmão de x (à esquerda)

            // Caso 1 simétrico
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(T, x->parent);
                w = x->parent->left;
            }

            // Caso 2 simétrico
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } 
            else {
                // Caso 3 simétrico
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(T, w);
                    w = x->parent->left;
                }

                // Caso 4 simétrico
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(T, x->parent);
                x = T->root;
            }
        }
    }

    x->color = BLACK;
}

void rbDelete(RBTree* T, Node* z) {
    Node* y = z;                            
    Color y_cor_original = y->color;        
    Node* x;

    if (z->left == T->nil) {                
        x = z->right;                       
        rb_transplant(T, z, z->right);       
    } 
    else if (z->right == T->nil) {         
        x = z->left;                     
        rb_transplant(T, z, z->left);       
    } 
    else {                                  
        y = rbMin(T, z->right);       
        y_cor_original = y->color;          
        x = y->right;                       
        
        if (y->parent == z) {               
            x->parent = y;                 
        } else {                            
            rb_transplant(T, y, y->right);  
            y->right = z->right;            
            y->right->parent = y;           
        }
        
        rb_transplant(T, z, y);             
        y->left = z->left;                  
        y->left->parent = y;               
        y->color = z->color;                
    }

    if (y_cor_original == BLACK) {          
        rbDeleteFixup(T, x);                
    }

    nodeFree(z);                            // Libera a memória do nó removido
}

Node* rb_search(RBTree* T, Node* x, int key) {
    while (x != T->nil && key != x->data) {
        if (key < x->data)
            x = x->left;
        else
            x = x->right;
    }
    return x;
}

Node* rbMax(RBTree* T, Node* x) {
    while (x->right != T->nil)
        x = x->right;
    return x;
}

int rb_size(RBTree* T, Node* x) {
    if (x == T->nil)
        return 0;
    return 1 + rb_size(T, x->left) + rb_size(T, x->right);
}

void rb_store(RBTree* T, Node* nd, int *arr, int *index) {
    if (nd != T->nil) {
        rb_store(T, nd->left, arr, index);
        arr[(*index)++] = nd->data;
        rb_store(T, nd->right, arr, index);
    }
}

void rb_printRec(RBTree* T, Node* nd) {
    if (nd != T->nil) {
        rb_printRec(T, nd->left);
        printf("%02d%c ", nd->data, nd->color == RED ? 'V' : 'P');
        rb_printRec(T, nd->right);
    }
}

void rb_print(RBTree* T) {
    printf("RBT: [ ");
    rb_printRec(T, T->root);
    printf("]\n");
}

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

void rb_printTree(RBTree* T) {
    rb_printTreeRec(T, T->root, 0);
}

/*------------------------------------------------------------------------------
 * Verificação das propriedades da árvore vermelho-preto
 */

// Verifica se a raiz é preta
bool rb_checkRoot(RBTree* T) {
    return T->root == T->nil || T->root->color == BLACK;
}

// Verifica se nenhum nó vermelho tem filho vermelho
bool rb_checkRedChildren(RBTree* T, Node* nd) {
    if (nd == T->nil)
        return true;
    if (nd->color == RED) {
        if (nd->left->color == RED || nd->right->color == RED)
            return false;
    }
    return rb_checkRedChildren(T, nd->left) && rb_checkRedChildren(T, nd->right);
}

// Verifica se todos os caminhos têm a mesma black-height
bool rb_checkBlackHeight(RBTree* T, Node* nd, int bh_count, int expected_bh) {
    if (nd == T->nil)
        return bh_count == expected_bh;
    if (nd->color == BLACK)
        bh_count++;
    return rb_checkBlackHeight(T, nd->left, bh_count, expected_bh) &&
           rb_checkBlackHeight(T, nd->right, bh_count, expected_bh);
}

// Verifica todas as propriedades RB + conteúdo dos dados
bool rb_check(RBTree* T, const int * const data, const int N) {
    const int tsize = rb_size(T, T->root);

    if (tsize != N)
        return false;
    if (N == 0)
        return rb_checkRoot(T);

    if (!rb_checkRoot(T))
        return false;
    if (!rb_checkRedChildren(T, T->root))
        return false;

    int expected_bh = bh(T, T->root);
    if (!rb_checkBlackHeight(T, T->root, 0, expected_bh))
        return false;

    int *arr = (int *)malloc(sizeof(int) * N);
    memcpy(arr, data, sizeof(int) * N);
    qsort(arr, N, sizeof(int), int_comp);

    int *tarr = (int *)malloc(sizeof(int) * tsize);
    int index = 0;
    rb_store(T, T->root, tarr, &index);

    bool match = true;
    for (int i = 0; i < N && match; i++) {
        if (arr[i] != tarr[i])
            match = false;
    }

    free(tarr);
    free(arr);
    return match;
}

/*------------------------------------------------------------------------------
 * Permutação de um Arranjo
 */

void swap(int *a, int *b) {
    const int temp = *a;
    *a = *b;
    *b = temp;
}

void perm_invert(int *arr, int inicio, int fim) {
    while (inicio < fim) {
        swap(&arr[inicio], &arr[fim]);
        inicio++;
        fim--;
    }
}

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

int int_comp(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

/*------------------------------------------------------------------------------
 * Funções Auxiliares
 */

void data_print(const int * const data, const int N) {
    printf("data: [ ");
    for (int i = 0; i < N; i++)
        printf("%02d ", data[i]);
    printf("]\n");
}

int arr_remove(int *arr, int N, int value) {
    for (int i = 0; i < N; i++) {
        if (arr[i] == value) {
            for (int j = i; j < N - 1; j++)
                arr[j] = arr[j + 1];
            N--;
            i--;
        }
    }
    return N;
}

/*------------------------------------------------------------------------------
 * Programa Principal - Bateria de Testes
 */

int main() {
    int DATA_INSERT[] = {1, 2, 3, 4, 5};
    int DATA_REMOVE[] = {1, 2, 3, 4, 5};
    const int N = 5;

    int *data_insert, *data_remove;

    data_insert = (int *)malloc(sizeof(int) * N);
    memcpy(data_insert, DATA_INSERT, sizeof(int) * N);

    int total_testes = 0;

    do {
        data_remove = (int *)malloc(sizeof(int) * N);
        memcpy(data_remove, DATA_REMOVE, sizeof(int) * N);

        do {
            RBTree *T = rbTreeAlloc();

            printf("============================================\n");
            printf("Dados para Insercao:\n\t");
            data_print(data_insert, N);

            for (int i = 0; i < N; i++) {
                printf("Inserindo: %02d\n", data_insert[i]);
                rb_insert(T, data_insert[i]);
                rb_print(T);
                assert(rb_check(T, data_insert, i + 1));
            }

            printf("Arvore apos todas as INSERCOES:\n");
            rb_printTree(T);

            printf("Dados para Remocao:\n\t");
            data_print(data_remove, N);
            int *arr = (int *)malloc(sizeof(int) * N);
            int asize = N;
            memcpy(arr, data_insert, sizeof(int) * asize);

            for (int i = 0; i < N; i++) {
                printf("Removendo: %02d\n", data_remove[i]);
                Node *nd = rb_search(T, T->root, data_remove[i]);
                assert(nd != T->nil);
                rbDelete(T, nd);
                rb_print(T);
                asize = arr_remove(arr, asize, data_remove[i]);
                assert(rb_check(T, arr, asize));
            }

            printf("Arvore apos todas as REMOCOES:\n");
            rb_printTree(T);

            free(arr);
            rbTreeFree(T);

            total_testes++;

        } while (perm_next(data_remove, N));

        free(data_remove);

    } while (perm_next(data_insert, N));

    free(data_insert);

    printf("\n========================================\n");
    printf("TODOS OS %d TESTES PASSARAM!\n", total_testes);
    printf("========================================\n");

    return EXIT_SUCCESS;
}
