#include "BTree.h"
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>  
#include <map>
using namespace std;

int main() {
    string binFilename = "btree.bin";
    string txtFilename = "btree.txt"; 
    
    BTree btree(2); // Altere aqui a ordem da Árvore

    // Vetor de elementos a serem inseridos
    vector<int> elementos = {20, 40, 10, 30, 15, 35, 7, 26, 18, 22, 5, 42, 13, 46, 27, 8, 32, 38, 24, 45, 25};

        cout << "\n┌───────────────────────────────────────────────────────────────┐\n";
        cout << "│\t\t\tINSERÇÃO NA ÁRVORE B\t\t\t│";
        cout << "\n│---------------------------------------------------------------│" << endl;
    
    size_t i = 0;
    for (int chave : elementos) {
        i++;
        cout << "│ ✚ Inserindo  " << chave << setw(52 - to_string(chave).length()) << "│" << endl;
        cout << "│" << setw(66) << "│" << endl;
        btree.insert(chave, "End_" + to_string(chave));
        btree.print_tree();
        cout << "│" << setw(67) << "│\n";

        if (i < elementos.size()) {
            cout << "├───────────────────────────────────────────────────────────────┤\n";
        }
    }
    cout << "└───────────────────────────────────────────────────────────────┘\n" << endl;
    
    btree.saveToFile(binFilename); //Salva a árvore em arquivo
    btree.convertBinToTxt(binFilename, txtFilename); //Converte o arquivo binário para txt
    
    
    // Vetor de elementos a serem removidos
    vector<int> elementos2 = {25, 45, 24}; 

     cout << "\n┌───────────────────────────────────────────────────────────────┐\n";
     cout << "│\t\t\tREMOÇÃO NA ÁRVORE B\t\t\t│";
     cout << "\n│---------------------------------------------------------------│" << endl;
    
    size_t j = 0;
    for (int chave : elementos2) {
        j++;
        cout << "│ ➤ Removendo  " << chave << "..." << setw(47) << "│" << endl;
        btree.remove(chave);
        cout << "\n│---------------------------------------------------------------│" << endl;
        cout << "│ Árvore pós remoção:" << setw(47) << "│\n";
        cout << "│" << setw(66) << "│" << endl;
        btree.print_tree();
        cout << "│" << setw(67) << "│\n";

        if (j < elementos2.size()) {
            cout << "├───────────────────────────────────────────────────────────────┤\n";
        }
    }
    cout << "└───────────────────────────────────────────────────────────────┘\n" << endl;
    
    // Vetor de elementos a serem buscados na árvore através do arquivo binário
    vector<int> buscas = {3, 69, 15, 2};
   
    cout << "\n┌───────────────────────────────────────────────────────────────┐\n";
    cout << "│\t\t\tBUSCA NA ÁRVORE B\t\t\t│";
    cout << "\n│---------------------------------------------------------------│" << endl;
    
    for (int chave : buscas) {
        auto resultado = btree.searchInFile(binFilename, chave);
        if(resultado.first) {
            cout << "│ - Chave " << dec << chave << " - Encontrada no nó com endereço: 0x"
                << hex << resultado.second << dec << "\t│" << endl;
        } else {
            cout << "│ - Chave " << chave << " - Não encontrada!\t\t\t\t\t│" << endl;
        }
    }
    cout << "└───────────────────────────────────────────────────────────────┘\n" << endl;
    
    //Reconstruindo a arvore apartir do arquivo binario
    BTree btree2(2);
    map<int, ArvoreId> nodes = btree2.readAllNodesFromFile(binFilename);
    // Construir a árvore na memória
    map<int, BTreeNode*> treeNodes;
    for (const auto& [id, diskNode] : nodes) {
        BTreeNode* node = new BTreeNode(btree2.m, diskNode.ehfolha == 1);
        node->keys.resize(diskNode.qntchave);
        for (int i = 0; i < diskNode.qntchave; i++) {
            node->keys[i] = make_pair(diskNode.chave[i], ""); // Endereço vazio
        }
        treeNodes[id] = node;
    }

    BTreeNode* root = nullptr;
    for (const auto& [id, diskNode] : nodes) {
        BTreeNode* node = treeNodes[id];
        if (diskNode.pai != -1) {
            node->parent = treeNodes[diskNode.pai];
        } else {
            root = node;
        }
        for (int i = 0; i < diskNode.qntfilho; i++) {
            node->children.push_back(treeNodes[diskNode.idfilho[i]]);
        }
    }
    cout << "\n┌───────────────────────────────────────────────────────────────┐\n";
    cout << "│\tRECONSTRUÇÃO DA ÁRVORE B A PARTIR DO ARQUIVO BINÁRIO\t│";
    cout << "\n│---------------------------------------------------------------│";
    // Imprimir a árvore reconstruída
    if (root) {
        cout << "\n│ Árvore B reconstruída:";
        cout << setw(44) << "│\n";
        root->print_node();
    } else {
        cout << "\n│ Falha ao reconstruir a árvore.\n";
    }
    cout << "└───────────────────────────────────────────────────────────────┘\n" << endl;
    
    return 0;
}
