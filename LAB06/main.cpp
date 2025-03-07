#include <iostream>
#include "btree.h"

using namespace std;

int main() {
    int order, menu, k, keyToSearch, keyToRemove;
    long addr;

    cout << "Digite a Ordem: ";
    cin >> order;
    // Cria a Árvore B com mínimo = order e máximo = 2*order chaves por nó
    BTree tree(order);

    do {
        cout << "\nEscolha uma opção:\n";
        cout << "1 - Buscar\n2 - Inserir\n3 - Remover\n4 - Sair\n";
        cout << "Opção: ";
        cin >> menu;

        switch (menu) {
            case 1: {
                cout << "Digite a chave que será buscada: ";
                cin >> keyToSearch;
                BTreeNode *result = tree.search(keyToSearch);
                if (result)
                    cout << "Chave " << keyToSearch << " encontrada na árvore.\n";
                else
                    cout << "Chave " << keyToSearch << " não encontrada na árvore.\n";
                break;
            }
            case 2: {
                cout << "\nDigite a chave para inserção: ";
                cin >> k;
                cout << "\nDigite o endereço:";
                cin >> addr;
                tree.insert(Record(k, addr));
                cout << "\nChave " << k << " inserida\n";
                cout << "\nEstrutura da Árvore B após inserção:\n";
                tree.print();
                break;
            }
            case 3: {
                cout << "\nDigite a chave para remoção: ";
                cin >> keyToRemove;
                tree.remove(keyToRemove);
                cout << "Estrutura da Árvore B após remoção:\n";
                tree.print();
                break;
            }
            case 4: {
                cout << "\nSaindo...\n";
                break;
            }
            default:
                cout << "Opção inválida! Tente novamente.\n";
                break;
        }
    } while (menu != 4);

    return 0;
}
