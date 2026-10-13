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
    short height;//altura do nó
} Node;

//Função para alocar memória para um novo nó
Node* nodeAlloc(int data){
    Node *novo = malloc(sizeof(Node));
    if(novo){
        novo->data = data;
        novo->color = RED;//novo nó é sempre vermelho
        novo->left = NULL;
        novo->right = NULL;
        novo->parent = NULL;
        novo->height = 0;
    } 
    return novo;
}

//Função para liberar a memória de um nó
void nodeFree(Node* node){
    free(node);
}

//Definição da estrutura da árvore vermelho-preto
typedef struct {
    Node *root;
    Node *nil;
} RBTree;

//Função para alocar memória para uma nova árvore vermelho-preto
RBTree* rbTreeAlloc(){
    RBTree *T = malloc(sizeof(RBTree));
    if(T){
        T->nil = nodeAlloc(0);//nó nil é sempre preto
        T->nil->color = BLACK;
        T->root = T->nil;
    }

    return T;
}

//Função para liberar a memória da árvore vermelho-preto recursivamente
void rbTreeFreeRec(Node* node){
    if(node){
        rbTreeFreeRec(node->left);
        rbTreeFreeRec(node->right);
        nodeFree(node);
    }
}

//Função para liberar a memmória da árvore vermelho-preto
void rbTreeFree(RBTree* T){
    if(T){
        rbTreeFreeRec(T->root);
        free(T);
    }
}

int main() {

}