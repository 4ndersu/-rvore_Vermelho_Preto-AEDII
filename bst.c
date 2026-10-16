#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

/*------------------------------------------------------------------------------
 * Árvore de Busca Binária
 */

struct Node {
    int key;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};
typedef struct Node Node;

Node * node_alloc(int key)
{
    Node *nd = (Node *)malloc(sizeof(Node));
    if (nd) {
        nd->key = key;
        nd->parent = NULL;
        nd->left = NULL;
        nd->right = NULL;
    }
    return nd;
}

void node_free(Node *nd)
{
    free(nd);
}

struct BinarySearchTree {
    struct Node *root;
};
typedef struct BinarySearchTree BST;

BST * bst_alloc()
{
    BST *T = (BST *)malloc(sizeof(BST));
    if (T) {
        T->root = NULL;
    }
    return T;
}

void bst_freeRec(Node *nd)
{
    if (nd) {
        bst_freeRec(nd->left);
        bst_freeRec(nd->right);
        node_free(nd);
    }
}

void bst_free(BST *T)
{
    bst_freeRec(T->root);
    free(T);
}

void bst_insert(BST *T, Node *z)
{
    Node *y = NULL;
    Node *x = T->root;

    while (x != NULL) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }

    z->parent = y;

    if (y == NULL) {
        T->root = z;
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }
}

Node * bst_minimum(Node *x)
{
    while (x->left != NULL) {
        x = x->left;
    }
    return x;
}

Node * bst_maximum(Node *x)
{
    while (x->right != NULL) {
        x = x->right;
    }
    return x;
}

Node * bst_successor(Node *x)
{
    if (x->right != NULL) {
        return bst_minimum(x->right);
    }
    Node *y = x->parent;
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y;
}

Node * bst_predecessor(Node *x)
{
    if (x->left != NULL) {
        return bst_maximum(x->left);
    }
    Node *y = x->parent;
    while (y != NULL && x == y->left) {
        x = y;
        y = y->parent;
    }
    return y;
}

Node * bst_search(Node *x, int key)
{
    while (x != NULL && key != x->key) {
        if (key < x->key) {
            x = x->left;
        } else {
            x = x->right;
        }
    }
    return x;
}

void transplant(BST *T, Node *u, Node *v)
{
    if (u->parent == NULL) {
        T->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }

    if (v != NULL) {
        v->parent = u->parent;
    }
}

void bst_delete(BST *T, Node *z)
{
    if (z->left == NULL) {
        transplant(T, z, z->right);
    } else if (z->right == NULL) {
        transplant(T, z, z->left);
    } else {
        Node *y = bst_minimum(z->right);

        if (y->parent != z) {
            transplant(T, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }

        transplant(T, z, y);
        y->left = z->left;
        y->left->parent = y;
    }

    node_free(z);
}

int bst_size(Node *x)
{
    if (x == NULL)
        return 0;
    else
        return 1 + bst_size(x->left) + bst_size(x->right);
}

void bst_printRec(Node *nd)
{
    if (nd) {
        bst_printRec(nd->left);
        printf("%02d ", nd->key);
        bst_printRec(nd->right);
    }
}

void bst_print(BST *T)
{
    printf("BST: [ ");
    bst_printRec(T->root);
    printf("]\n");
}

void bst_store(Node *nd, int *arr, int *index)
{
    if (nd) {
        bst_store(nd->left, arr, index);
        arr[(*index)++] = nd->key;
        bst_store(nd->right, arr, index);
    }
}

int int_comp(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

bool bst_check(Node *nd, const int * const data, const int N)
{
    const int tsize = bst_size(nd);

    if (tsize != N) {
        return false;
    }
    if (N == 0) {
        return true;
    }

    int *arr = (int *)malloc(sizeof(int) * N);
    memcpy(arr, data, sizeof(int) * N);
    qsort(arr, N, sizeof(int), int_comp);

    int *tarr = (int *)malloc(sizeof(int) * tsize);
    int index = 0;
    bst_store(nd, tarr, &index);

    // for (int i = 0; i < N; i++)
    //    printf("(%02d ; %02d)\n", arr[i], tarr[i]);
    // fflush(stdout);

    bool match = true;
    for (int i = 0; i < N && match; i++) {
        if (arr[i] != tarr[i]) {
            match = false;
        }
    }

    free(tarr);
    free(arr);

    return match;
}

void bst_printTreeRec(Node *root, int space)
{
    if (root != NULL) {
        space += 4;

        bst_printTreeRec(root->right, space);

        printf("\n");
        for (int i = 4; i < space; i++) {
            printf(" ");
        }
        printf("%02d\n", root->key);

        bst_printTreeRec(root->left, space);
    }
}

void bst_printTree(Node *root)
{
    bst_printTreeRec(root, 0);
}

/*------------------------------------------------------------------------------
 * Permutação de um Arranjo
 *
 * Implementan o algoritmo iterativo de Narayana Pandita para a permutação de
 * um arranjo em ordem lexicográfica.
 */

void swap(int *a, int *b)
{
    const int temp = *a;
    *a = *b;
    *b = temp;
}

void perm_invert(int *arr, int inicio, int fim)
{
    while (inicio < fim) {
        swap(&arr[inicio], &arr[fim]);
        inicio++;
        fim--;
    }
}

bool perm_next(int *arr, int tamanho)
{
    int i = tamanho - 2;

    while (i >= 0 && arr[i] >= arr[i + 1]) {
        i--;
    }

    if (i < 0) {
        return false;
    }

    int j = tamanho - 1;
    while (arr[j] <= arr[i]) {
        j--;
    }

    swap(&arr[i], &arr[j]);

    perm_invert(arr, i + 1, tamanho - 1);

    return true;
}

/*------------------------------------------------------------------------------
 * Funções Auxiliares
 */

void data_print(const int * const data, const int N)
{
    printf("data: [ ");
    for (int i = 0; i < N; i++) {
        printf("%02d ", data[i]);
    }
    printf("]\n");
}

int arr_remove(int *arr, int N, int value)
{
    for (int i = 0; i < N; i++) {
        if (arr[i] == value) {
            for (int j = i; j < N - 1; j++) {
                arr[j] = arr[j + 1];
            }
            N--;
            i--;
        }
    }

    return N;
}

/*------------------------------------------------------------------------------
 * Programa Principal
 */

int main()
{
/*
    BST *T = bst_alloc();
    bst_insert(T, node_alloc(10));
    bst_insert(T, node_alloc(15));
    bst_insert(T, node_alloc(5));
    bst_insert(T, node_alloc(1));
    bst_insert(T, node_alloc(3));
    bst_insert(T, node_alloc(25));
    bst_insert(T, node_alloc(20));
    bst_print(T);
    Node *min = bst_minimum(T->root);
    printf("min: %02d\n", min->key);
    Node *max = bst_maximum(T->root);
    printf("max: %02d\n", max->key);
    Node *succ = bst_successor(T->root);
    printf("succ: %02d\n", succ->key);
    Node *pred = bst_predecessor(T->root);
    printf("pred: %02d\n", pred->key);
    bst_free(T);
*/
    int DATA_INSERT[] = {1, 2, 3, 4, 5}; // DEVE estar ordenado!
    int DATA_REMOVE[] = {1, 2, 3, 4, 5}; // DEVE estar ordenado!
    const int N = 5; // tamanho dos arranjos de inserção e remoção

    int *data_insert, *data_remove;

    data_insert = (int *)malloc(sizeof(int) * N);
    memcpy(data_insert, DATA_INSERT, sizeof(int) * N);
    
    do { // Loop de Inserção
    
        data_remove = (int *)malloc(sizeof(int) * N);
        memcpy(data_remove, DATA_REMOVE, sizeof(int) * N);

        do { // Loop de Remoção
            BST *T = bst_alloc();

            printf("--------------------------------------------\n");
            printf("Dados para Insercao:\n\t");
            data_print(data_insert, N);
            
            for (int i = 0; i < N; i++) {
                printf("Inserindo: %02d\n", data_insert[i]);
                Node *nd = node_alloc(data_insert[i]);
                bst_insert(T, nd);
                bst_print(T);
                assert(bst_check(T->root, data_insert, i + 1));
            }
            
            printf("Arvore apos todas as INSERCOES:\n");
            bst_printTree(T->root);

            printf("Dados para Remocao:\n\t");
            data_print(data_remove, N);
            int *arr = (int *)malloc(sizeof(int) * N);
            int asize = N;
            memcpy(arr, data_insert, sizeof(int) * asize);
            
            for (int i = 0; i < N; i++) {
                printf("Removendo: %02d\n", data_remove[i]);
                bst_delete(T, bst_search(T->root, data_remove[i]));
                bst_print(T);
                asize = arr_remove(arr, asize, data_remove[i]);
                assert(bst_check(T->root, arr, asize));
            }
            
            printf("Arvore apos todas as REMOCOES:\n");
            bst_printTree(T->root);

            free(arr);
            bst_free(T);

        } while (perm_next(data_remove, N));
        
        free(data_remove);

    } while (perm_next(data_insert, N));
    
    free(data_insert);

    return EXIT_SUCCESS;
}