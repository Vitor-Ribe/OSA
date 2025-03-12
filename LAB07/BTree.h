#ifndef BTREE_H
#define BTREE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

using namespace std;

using KeyValue = pair<int, string>;

struct ArvoreId{
    int id;         //id do nó único
    uintptr_t enderecono; //endereço real do nó 
    int pai;        //id do pai, se for raiz é -1
    uintptr_t enderecopai; //endereço do pai, se for raiz é 0
    int ehfolha;    //se é folha = 1 ou não = 0
    int qntfilho;   //numero de filhos
    int idfilho[101]; //id dos filhos
    int qntchave;      //numero de chaves armazenadas
    int chave[200]; //vetor fixo de chaves armazenadas
    uintptr_t endereco[101]; //endereço dos filhos
};


class BTreeNode {
    public:
        int m;                // mínimo de chaves em nós não‑raiz (m = ordem - 1)
        bool leaf;            // indica se é nó folha
        vector<KeyValue> keys;     // vetor de pares (chave, endereço)
        vector<BTreeNode*> children; // vetor de ponteiros para os filhos
        BTreeNode* parent;    // ponteiro para o nó pai
    
        // Construtor
        BTreeNode(int m, bool leaf = false, BTreeNode* parent = nullptr);
    
        // Métodos da árvore
        pair<BTreeNode*, int> search(int key);
        int find_key_index(int key);
        BTreeNode* insert_nonfull(int key, const string& address);
        void split_child(int i);
    
        void remove(int key,const string& binFilename) ;
        void remove_from_leaf(int idx);
        void remove_from_nonleaf(int idx, const string& binFilename); 
    
        KeyValue get_predecessor(int idx);
        KeyValue get_successor(int idx);
        void fill(int idx);
        void borrow_from_prev(int idx);
        void borrow_from_next(int idx);
        void merge(int idx);
        void rotate_internal_left();
        void fix_deficiency_upwards();
        void print_node(int level = 0);
    };
    
    class BTree {
    public:
        int m;            // mínimo de chaves em nós não‑raiz (m = ordem - 1)
        BTreeNode* root;  // ponteiro para a raiz da árvore
    
        // Construtor: 'ordem' é o valor informado (por exemplo, 3). Internamente m = ordem - 1.
        BTree(int ordem);
        pair<BTreeNode*, int> search(int key);
        void insert(int key, const string& address);
        void remove(int key);
        void print_tree();
        void saveToFile(const string& filename);
        static pair<bool, uintptr_t> searchInFile(const string& binFilename, int key);
        void convertBinToTxt(const string& binFilename, const string& txtFilename); 
    };
    

#endif // BTREE_H
