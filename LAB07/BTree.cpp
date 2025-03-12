#include "BTree.h"
#include <fstream>
#include <queue>
#include <map>
#include <iomanip>  
#include <cstring>      // para memset
#include <cstdint>      // para uintptr_t
#include <sys/stat.h>   // para stat

using namespace std;

BTreeNode::BTreeNode(int m, bool leaf, BTreeNode* parent)
    : m(m), leaf(leaf), parent(parent) { }

int BTreeNode::find_key_index(int key) {
    int idx = 0;
    while (idx < static_cast<int>(keys.size()) && keys[idx].first < key)
        idx++;
    return idx;
}


BTreeNode* BTreeNode::insert_nonfull(int key, const string& address) {
    if (leaf) {
        int i = keys.size() - 1;
        keys.push_back({0, ""}); // Realiza a alocação de espaço para a nova chave
        while (i >= 0 && key < keys[i].first) {
            keys[i+1] = keys[i];
            i--;
        }
        keys[i+1] = {key, address};
        // Se houver excesso de chaves  maior que (2M), a página deverá ser dividida
        if (static_cast<int>(keys.size()) > 2 * m) {
            if (parent != nullptr) {
                int idx = 0;
                for (int j = 0; j < static_cast<int>(parent->children.size()); j++) {
                    if (parent->children[j] == this) {
                        idx = j;
                        break;
                    }
                }
                parent->split_child(idx);
            } else {
                // Divisão da raiz
                BTreeNode* new_root = new BTreeNode(m, false);
                new_root->children.push_back(this);
                parent = new_root;
                new_root->split_child(0);
                return new_root;
            }
        }
        return nullptr;
    } else {
        int i = keys.size() - 1;
        while (i >= 0 && key < keys[i].first)
            i--;
        i++;
        if (static_cast<int>(children[i]->keys.size()) > 2 * m) {
            split_child(i);
            if (key > keys[i].first)
                i++;
        }
        BTreeNode* ret = children[i]->insert_nonfull(key, address);
        if (ret != nullptr)
            return ret;
        if (static_cast<int>(keys.size()) > 2 * m) {
            if (parent != nullptr) {
                int idx = 0;
                for (int j = 0; j < static_cast<int>(parent->children.size()); j++) {
                    if (parent->children[j] == this) {
                        idx = j;
                        break;
                    }
                }
                parent->split_child(idx);
            } else {
                BTreeNode* new_root = new BTreeNode(m, false);
                new_root->children.push_back(this);
                parent = new_root;
                new_root->split_child(0);
                return new_root;
            }
        }
        return nullptr;
    }
}

void BTreeNode::split_child(int i) {
    BTreeNode* y = children[i];                      // nó a ser dividido
    BTreeNode* z = new BTreeNode(m, y->leaf, this);    // novo nó que receberá as chaves finais
    KeyValue mid_key = y->keys[m];                   // chave mediana a ser promovida

    // z recebe as chaves de índice m+1 até o fim
    z->keys.assign(y->keys.begin() + m + 1, y->keys.end());
    // Se não for folha, transfere também os filhos
    if (!y->leaf) {
        z->children.assign(y->children.begin() + m + 1, y->children.end());
        for (auto child : z->children)
            child->parent = z;
        y->children.resize(m + 1);
    }
    // y mantém as chaves de índice 0 até m-1
    y->keys.resize(m);

    // Insere z como filho deste nó, logo após y, e promove a chave mediana
    children.insert(children.begin() + i + 1, z);
    keys.insert(keys.begin() + i, mid_key);
}

// Sobrecarga: agora o remove dos nós recebe o binFilename para utilizar searchInFile
void BTreeNode::remove(int key, const string& binFilename) {
    int idx = find_key_index(key);
    if (idx < static_cast<int>(keys.size()) && keys[idx].first == key) {
        if (leaf) {
            remove_from_leaf(idx);
            if (parent != nullptr && static_cast<int>(keys.size()) < m) {
                int idx_p = 0;
                for (int j = 0; j < static_cast<int>(parent->children.size()); j++) {
                    if (parent->children[j] == this) {
                        idx_p = j;
                        break;
                    }
                }
                parent->fill(idx_p);
                parent->fix_deficiency_upwards();
            }
        } else {
            remove_from_nonleaf(idx, binFilename);
        }
    } else {
        if (leaf)
            return; // chave não encontrada
        if (!BTree::searchInFile(binFilename, key).first &&
            static_cast<int>(children[idx]->keys.size()) == m)
            fill(idx);
        children[idx]->remove(key, binFilename);
    }
}

void BTreeNode::remove_from_leaf(int idx) {
    keys.erase(keys.begin() + idx);
}

void BTreeNode::remove_from_nonleaf(int idx, const string& binFilename) {
    int key_val = keys[idx].first;
    if (static_cast<int>(children[idx]->keys.size()) >= m) {
        KeyValue pred = get_predecessor(idx);
        keys[idx] = pred;
        children[idx]->remove(pred.first, binFilename);
    } else if (static_cast<int>(children[idx+1]->keys.size()) >= m) {
        KeyValue succ = get_successor(idx);
        keys[idx] = succ;
        children[idx+1]->remove(succ.first, binFilename);
    } else {
        merge(idx);
        children[idx]->remove(key_val, binFilename);
    }
}

KeyValue BTreeNode::get_predecessor(int idx) {
    BTreeNode* cur = children[idx];
    while (!cur->leaf)
        cur = cur->children.back();
    return cur->keys.back();
}

KeyValue BTreeNode::get_successor(int idx) {
    BTreeNode* cur = children[idx+1];
    while (!cur->leaf)
        cur = cur->children.front();
    return cur->keys.front();
}

void BTreeNode::fill(int idx) {
    if (idx != 0 && static_cast<int>(children[idx-1]->keys.size()) > m)
        borrow_from_prev(idx);
    else if (idx != static_cast<int>(keys.size()) && static_cast<int>(children[idx+1]->keys.size()) > m)
        borrow_from_next(idx);
    else {
        if (idx != 0)
            merge(idx-1);
        else
            merge(idx);
    }
}

void BTreeNode::borrow_from_prev(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx-1];
    child->keys.insert(child->keys.begin(), keys[idx-1]);
    if (!child->leaf) {
        child->children.insert(child->children.begin(), sibling->children.back());
        child->children.front()->parent = child;
        sibling->children.pop_back();
    }
    keys[idx-1] = sibling->keys.back();
    sibling->keys.pop_back();
}

void BTreeNode::borrow_from_next(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx+1];
    child->keys.push_back(keys[idx]);
    if (!child->leaf) {
        child->children.push_back(sibling->children.front());
        child->children.back()->parent = child;
        sibling->children.erase(sibling->children.begin());
    }
    keys[idx] = sibling->keys.front();
    sibling->keys.erase(sibling->keys.begin());
}

void BTreeNode::merge(int idx) {
    BTreeNode* child = children[idx];
    BTreeNode* sibling = children[idx+1];
    child->keys.push_back(keys[idx]);
    child->keys.insert(child->keys.end(), sibling->keys.begin(), sibling->keys.end());
    if (!child->leaf) {
        child->children.insert(child->children.end(), sibling->children.begin(), sibling->children.end());
        for (auto c : sibling->children)
            c->parent = child;
    }
    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);
    delete sibling;
}

void BTreeNode::rotate_internal_left() {
    if (children.size() < 2 || children[0]->keys.empty())
        return;
    BTreeNode* left_child = children[0];
    BTreeNode* right_child = children[1];
    KeyValue temp = left_child->keys.back();
    left_child->keys.pop_back();
    KeyValue old_key = keys[0];
    keys[0] = temp;
    right_child->keys.insert(right_child->keys.begin(), old_key);
    if (!left_child->leaf && !left_child->children.empty()) {
        BTreeNode* child_temp = left_child->children.back();
        left_child->children.pop_back();
        right_child->children.insert(right_child->children.begin(), child_temp);
        child_temp->parent = right_child;
    }
}

void BTreeNode::fix_deficiency_upwards() {
    BTreeNode* current = this;
    while (current->parent != nullptr && static_cast<int>(current->keys.size()) < current->m) {
        BTreeNode* parent_node = current->parent;
        int idx = 0;
        for (int j = 0; j < static_cast<int>(parent_node->children.size()); j++) {
            if (parent_node->children[j] == current) {
                idx = j;
                break;
            }
        }
        parent_node->fill(idx);
        current = parent_node;
    }
}

void BTreeNode::print_node(int level) {
    cout << "│";
    // Definindo cores para cada nível
    string color, colorLevel;
    switch(level) {
        case 0: color = "\033[95m"; colorLevel = "\033[45m"; break;   // Magenta para nível 0
        case 1: color = "\033[94m"; colorLevel = "\033[44m";break;   // Azul Claro para nível 1
        case 2: color = "\033[92m"; colorLevel = "\033[42m";break;   // Verde para nível 2
        case 3: color = "\033[31m"; colorLevel = "\033[41m";break;   // vermelho para nível 3
        default: color = "\033[37m"; colorLevel = "\033[47m";break;  // Branco para outros níveis
    }
    
    // Impressão do nível
    string indent(level * 8, ' ');  // Indentação de 8 espaços por nível
    if(level == 0)
        cout << color << indent << setw(20) << "🡺 Raiz:";
    else
        cout << indent << "\t↳ " << color << "Nível " << level << ":";

    cout << "\033[0m ";  // Reset para a cor padrão
    
    // Impressão das chaves
    cout << colorLevel; // aplica a cor para a chave
    for (size_t i = 0; i < keys.size(); i++) {
        cout << keys[i].first << " ";
    }
    cout << "\033[0m";  // Reset para a cor padrão

    // Ajuste para o total de chaves
    size_t total_width = 0;
    for (size_t i = 0; i < keys.size(); i++) {
        total_width += to_string(keys[i].first).length() + 1; // Adiciona 1 para o espaço entre as chaves
    }
    // Ajusta o setw para garantir que o "|" fique no mesmo lugar
    cout << setw(49 - total_width - level*8) << "│\n";
    //cout << total_width << level;
    
    if (!leaf) {
        for (auto child : children)
            child->print_node(level + 1);
    }
}

// Mudou aqui para grau minimo igual a ordem
BTree::BTree(int ordem) : m(ordem) {
    root = new BTreeNode(m, true);
}

void BTree::insert(int key, const string& address) {
    // Mudou aqui para grau maximo igual a 2 * ordem
    if (static_cast<int>(root->keys.size()) > 2 * m) {
        BTreeNode* s = new BTreeNode(m, false);
        s->children.push_back(root);
        root->parent = s;
        s->split_child(0);
        root = s;
    }
    BTreeNode* new_root = root->insert_nonfull(key, address);
    if (new_root != nullptr)
        root = new_root;
}

void BTree::remove(int key) {
    if (!root) {
        cout << "Árvore vazia.\n";
        return;
    }
    // Verifica se a chave existe no arquivo (e, consequentemente, na árvore)
    auto resultado = searchInFile("btree.bin", key);
    if (!resultado.first) {
        cout << "Chave " << key << " não encontrada na árvore.\n";
        return;
    }
    // Remove a chave da árvore em memória (o método remove dos nós já cuida de balancear a árvore)
    root->remove(key, "btree.bin");
    
    // Se a remoção fez com que a raiz fique sem chaves, ajusta a raiz
    if (root && root->keys.empty()) {
        if (!root->leaf) {
            BTreeNode* tmp = root->children[0];
            tmp->parent = nullptr;
            delete root;
            root = tmp;
        } else {
            delete root;
            root = nullptr;
        }
    }

    cout << "│ - Chave " << key << " removida!\t\t\t\t\t\t│";
    
    // Atualiza o arquivo binário e txt regravando a árvore atualizada
    saveToFile("btree.bin");
    convertBinToTxt("btree.bin", "btree.txt");
}

void BTree::print_tree() {
    if (root)
        root->print_node();
    else
        cout << "A Árvore B está vazia.\n";
}

// função para salvar a árvore em arquivo binário 
void BTree::saveToFile(const string& filename) {
    // Abre o arquivo em modo binário para escrita, sempre truncando o conteúdo anterior
    ofstream out(filename, ios::binary | ios::trunc);
    if (!out.is_open()) {
        cerr << "Não foi possível abrir o arquivo " << filename << " para escrita." << endl;
        return;
    }
    
    // Grava o cabeçalho com o valor de 'm'
    out.write(reinterpret_cast<const char*>(&m), sizeof(m));
    if (!root) {
        out.close();
        return;
    }
    
    // Se a árvore não está vazia, grava os nós usando BFS
    queue<BTreeNode*> fila;
    map<BTreeNode*, int> nodeId;  // associa cada nó a um ID inteiro
    int nextId = 1;
    fila.push(root);
    nodeId[root] = nextId++;
    
    while (!fila.empty()) {
        BTreeNode* current = fila.front();
        fila.pop();

        ArvoreId diskNode;
        memset(&diskNode, 0, sizeof(ArvoreId));  // Zera a memória para evitar lixo

        // Preenche os campos do nó atual
        diskNode.id = nodeId[current];
        diskNode.enderecono = reinterpret_cast<uintptr_t>(current);
        diskNode.pai = (current->parent ? nodeId[current->parent] : -1);
        diskNode.enderecopai = (current->parent ? reinterpret_cast<uintptr_t>(current->parent) : 0);
        diskNode.ehfolha = current->leaf ? 1 : 0;
        diskNode.qntchave = static_cast<int>(current->keys.size());

        // Grava as chaves (usando apenas o valor inteiro da chave)
        for (int i = 0; i < diskNode.qntchave; i++) {
            diskNode.chave[i] = current->keys[i].first;
        }

        // Grava as informações dos filhos
        diskNode.qntfilho = static_cast<int>(current->children.size());
        for (int i = 0; i < diskNode.qntfilho; i++) {
            BTreeNode* child = current->children[i];
            if (nodeId.find(child) == nodeId.end()) {
                nodeId[child] = nextId++;
                fila.push(child);
            }
            diskNode.idfilho[i] = nodeId[child];
            diskNode.endereco[i] = reinterpret_cast<uintptr_t>(child);
        }

        // Escreve o registro do nó no arquivo binário
        out.write(reinterpret_cast<const char*>(&diskNode), sizeof(diskNode));
    }

    out.close();
}

// função para buscar a chave no arquivo binário
pair<bool, uintptr_t> BTree::searchInFile(const string& binFilename, int key) {
    ifstream in(binFilename, ios::binary);

    if (!in.is_open()) {
        cerr << "!!!Não foi possível abrir o arquivo binário: " << binFilename << endl;
        return {false, 0};
    }

    ifstream checkFile("btree.bin", ios::binary);
        if (!checkFile) {
          cerr << "Erro: O arquivo 'btree.bin' não existe." << endl;
    }
    checkFile.close();

    ofstream createFile("btree.bin", ios::binary | ios::app);
    createFile.close();

    ifstream file("btree.bin", ios::in | ios::binary);
    if (!file.is_open()) {
        cerr << "Erro: Não foi possível abrir 'btree.bin'. Verifique se o arquivo existe e as permissões estão corretas." << endl;
        return {false, 0};
    }

    int file_m ;
    if (!in.read(reinterpret_cast<char*>(&file_m), sizeof(file_m))) {
        cerr << "Erro ao ler o cabeçalho do arquivo." << endl;
        return {false, 0};
    }

    
    // Verifica se há mais dados após o cabeçalho
    in.seekg(0, ios::end);
    if (in.tellg() == sizeof(file_m)) {
        // O arquivo contém apenas o cabeçalho, logo a árvore está vazia.
        return {false, 0};
    }
    
    // Retorna para a posição logo após o cabeçalho
    in.seekg(sizeof(file_m), ios::beg);

    ArvoreId current;
    if (!in.read(reinterpret_cast<char*>(&current), sizeof(ArvoreId))) {
        cerr << "Erro ao ler o nó raiz do arquivo." << endl;
        return {false, 0};
    }

    // Realiza a busca pela chave lendo os nós conforme necessário.
    while (true) {
        int i = 0;
        while (i < current.qntchave && key > current.chave[i])
            i++;
        if (i < current.qntchave && key == current.chave[i])
            return {true, current.enderecono};
        if (current.ehfolha == 1)
            return {false, 0};
        int childId = current.idfilho[i];
        if (!in.seekg(sizeof(file_m) + (childId - 1) * sizeof(ArvoreId), ios::beg))
            break;
        if (!in.read(reinterpret_cast<char*>(&current), sizeof(ArvoreId))) {
            cerr << "Erro ao ler o nó com id " << childId << endl;
            return {false, 0};
        }
    }
    
    return {false, 0};
}

// conversão do arquivo binário para texto
void BTree::convertBinToTxt(const string& binFilename, const string& txtFilename) {
    ifstream in(binFilename, ios::binary);
    if (!in.is_open()) {
        cerr << "Erro ao abrir o arquivo binario: " << binFilename << endl;
        return;
    }
    
    int file_m;
    if (!in.read(reinterpret_cast<char*>(&file_m), sizeof(file_m))) {
        cerr << "Erro ao ler o cabeçalho do arquivo binário." << endl;
        return;
    }
    
    // Se o arquivo estiver vazio após o cabeçalho, indica que a árvore está vazia.
    in.seekg(0, ios::end);
    if (in.tellg() == sizeof(file_m)) {
        ofstream out(txtFilename);
        if (out.is_open()) {
            out << "BTree vazia.\n";
            out.close();
        } else {
            cerr << "Não foi possível abrir o arquivo TXT para escrita." << endl;
        }
        in.close();
        return;
    }
    
    // Retorna para a posição logo após o cabeçalho e lê os nós.
    in.seekg(sizeof(file_m), ios::beg);
    vector<ArvoreId> nodes;
    while (true) {
        ArvoreId node;
        if (!in.read(reinterpret_cast<char*>(&node), sizeof(ArvoreId)))
            break;
        nodes.push_back(node);
    }
    in.close();
    
    // Grava os dados em arquivo TXT.
    ofstream out(txtFilename);
    if (!out.is_open()) {
        cerr << "Não foi possível abrir o arquivo de texto: " << txtFilename << endl;
        return;
    }
    
    out << "ÁRVORE B\n";
    out << "Ordem: " << file_m << "\n\n";
    out << left << setw(5)  << "ID"
        << left << setw(20) << " Endereco_Nó"
        << left << setw(10) << " ID_Pai"
        << left << setw(20) << " Endereco_Pai"
        << left << setw(6)  << " Folha?"
        << left << setw(10) << " Nº_Chaves"
        << left << setw(25) << " Chaves"
        << left << setw(13) << " Nº_Filhos"
        << left << setw(25) << " ID_Filhos"
        << left << setw(25) << " Endereco_Filho"
        << "\n";
    out << string(150, '-') << "\n";
    
    for (const auto &node : nodes) {
        out << left << setw(6) << node.id;{
            ostringstream oss;
            oss << "0x" << hex << node.enderecono;
            out << left << setw(20) << oss.str();
        }
        out << left << setw(10) << node.pai;{
            ostringstream oss;
            oss << "0x" << hex << node.enderecopai;
            out << left << setw(20) << oss.str();
        }
        out << left << setw(6) << node.ehfolha;
        out << left << setw(10) << node.qntchave;{
            ostringstream oss;
            for (int i = 0; i < node.qntfilho; i++) {
                oss << node.chave[i] << " ";
            }
            out << left << setw(25) << oss.str();
        }
        out << left << setw(12) << node.qntfilho;{
            ostringstream oss;
            for (int i = 0; i < node.qntfilho; i++) {
                oss << node.idfilho[i] << " ";
            }
            out << left << setw(25) << oss.str();
        }
        {
            ostringstream oss;
            for (int i = 0; i < node.qntfilho; i++) {
                oss << "0x" << hex << node.endereco[i] << " ";
            }
            out << left << setw(25) << oss.str();
        }
        out << "\n";
    }
    
    out.close();
}
