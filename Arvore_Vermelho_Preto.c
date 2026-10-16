#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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

int main() {
}