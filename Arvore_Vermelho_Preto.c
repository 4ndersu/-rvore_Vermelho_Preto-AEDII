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

int main() {

}