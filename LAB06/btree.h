#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <algorithm>

// Estrutura que representa um registro (par chave e endereço)
struct Record {
    int key;
    long address;
    Record(int k, long addr) : key(k), address(addr) {}
    Record() : key(0), address(0) {}
};

// Classe que representa um nó (página) da Árvore B
class BTreeNode {
public:
    bool leaf;       // true se o nó for folha
    int n;           // número atual de chaves
    int order;       // valor informado: mínimo de chaves em um nó (exceto raiz)
                     // máximo de chaves permitido é 2*order.
    std::vector<Record> keys;         // vetor de registros (chave, endereço)
    std::vector<BTreeNode*> children;   // vetor de ponteiros para os filhos

    // Construtor
    BTreeNode(int _order, bool _leaf);

    // Pesquisa uma chave na subárvore enraizada neste nó
    BTreeNode* search(int key);

    // Insere um novo registro na subárvore enraizada neste nó.
    // (Note que, nesta implementação, um nó pode temporariamente ficar com 2*order+1 chaves,
    //  e então é dividido)
    void insertNonFull(const Record &record);

    // Divide o filho y que está na posição i deste nó.
    // Assume que y está “overfull” (possui 2*order+1 chaves).
    void splitChild(int i, BTreeNode *y);

    // Funções auxiliares para remoção
    int findKey(int key);
    void remove(int key);
    void removeFromLeaf(int idx);
    void removeFromNonLeaf(int idx);
    Record getPred(int idx);
    Record getSucc(int idx);
    void fill(int idx);
    void borrowFromPrev(int idx);
    void borrowFromNext(int idx);
    void merge(int idx);

    // Função de impressão recursiva (exibe a estrutura hierárquica)
    void print(int level = 0);

    // Destrutor – libera recursivamente a memória dos nós filhos
    ~BTreeNode();
};

// Classe que representa a Árvore B
class BTree {
public:
    BTreeNode *root; // ponteiro para a raiz da árvore
    int order;       // mínimo de chaves por nó; máximo = 2*order

    // Construtor
    BTree(int _order) : root(nullptr), order(_order) {}

    // Pesquisa uma chave na árvore
    BTreeNode* search(int key) {
        return (root == nullptr) ? nullptr : root->search(key);
    }

    // Insere um novo registro na árvore
    void insert(const Record &record);

    // Remove uma chave da árvore
    void remove(int key);

    // Imprime a estrutura da árvore
    void print() {
        if(root)
            root->print();
        else
            std::cout << "Árvore vazia.\n";
    }

    // Destrutor
    ~BTree();
};

#endif // BTREE_H
