#include "btree.h"

// Construtor do nó: reserva espaço para até (2*order+1) chaves (temporariamente) e (2*order+2) filhos
BTreeNode::BTreeNode(int _order, bool _leaf) {
    order = _order;
    leaf = _leaf;
    n = 0;
    keys.reserve(2 * order + 1);
    children.reserve(2 * order + 2);
}

// Pesquisa recursiva: retorna o nó onde a chave foi encontrada ou nullptr
BTreeNode* BTreeNode::search(int key) {
    int i = 0;
    while(i < n && key > keys[i].key)
        i++;
    if(i < n && keys[i].key == key)
        return this;
    if(leaf)
        return nullptr;
    return children[i]->search(key);
}

// Inserção em nó que não esteja “overfull”
void BTreeNode::insertNonFull(const Record &record) {
    int i = n - 1;
    if(leaf) {
        // Insere a chave na posição correta (usando vector::insert para facilitar o shift)
        while(i >= 0 && keys[i].key > record.key)
            i--;
        keys.insert(keys.begin() + i + 1, record);
        n++;
        // Se após a inserção o nó ficou com 2*order+1 chaves, ele deverá ser dividido pelo pai.
        // (A divisão será realizada no retorno para o nível superior.)
    } else {
        while(i >= 0 && keys[i].key > record.key)
            i--;
        i++;
        // Insere recursivamente no filho i
        children[i]->insertNonFull(record);
        // Se o filho ficou com 2*order+1 chaves, divida-o agora.
        if(children[i]->n == 2 * order + 1)
            splitChild(i, children[i]);
    }
}

// Divide o filho y (que possui 2*order+1 chaves) na posição i deste nó.
void BTreeNode::splitChild(int i, BTreeNode *y) {
    // Cria um novo nó z que receberá as últimas 'order' chaves de y.
    BTreeNode *z = new BTreeNode(y->order, y->leaf);
    z->n = order;
    // Copia as chaves de índice (order+1) até (2*order) de y para z.
    for (int j = 0; j < order; j++) {
        z->keys.push_back(y->keys[j + order + 1]);
    }
    // Se y não for folha, copia também os ponteiros dos filhos correspondentes.
    if (!y->leaf) {
        for (int j = 0; j <= order; j++) {
            z->children.push_back(y->children[j + order + 1]);
        }
        y->children.resize(order + 1);
    }
    // A chave mediana, na posição 'order', será promovida.
    Record median = y->keys[order];
    // Reduz o número de chaves de y para 'order' (descartando a mediana e os que foram para z).
    y->keys.resize(order);
    y->n = order;

    // Insere o novo filho z na posição i+1 deste nó
    children.insert(children.begin() + i + 1, z);
    // Insere a mediana no vetor de chaves deste nó, na posição i.
    keys.insert(keys.begin() + i, median);
    n++;
}

int BTreeNode::findKey(int key) {
    int idx = 0;
    while(idx < n && keys[idx].key < key)
        ++idx;
    return idx;
}

void BTreeNode::remove(int key) {
    int idx = findKey(key);
    if(idx < n && keys[idx].key == key) {
        if(leaf)
            removeFromLeaf(idx);
        else
            removeFromNonLeaf(idx);
        std::cout << "A chave " << key << " foi removida da árvore\n";
    } else {
        if(leaf) {
            std::cout << "A chave " << key << " não está presente na árvore.\n";
            return;
        }
        bool flag = ( (idx == n) ? true : false );
        if(children[idx]->n < order)
            fill(idx);
        if(flag && idx > n)
            children[idx-1]->remove(key);
        else
            children[idx]->remove(key);
    }
}

void BTreeNode::removeFromLeaf(int idx) {
    keys.erase(keys.begin() + idx);
    n--;
}

void BTreeNode::removeFromNonLeaf(int idx) {
    int k = keys[idx].key;
    if(children[idx]->n >= order) {
        Record pred = getPred(idx);
        keys[idx] = pred;
        children[idx]->remove(pred.key);
    } else if(children[idx+1]->n >= order) {
        Record succ = getSucc(idx);
        keys[idx] = succ;
        children[idx+1]->remove(succ.key);
    } else {
        merge(idx);
        children[idx]->remove(k);
    }
}

Record BTreeNode::getPred(int idx) {
    BTreeNode *cur = children[idx];
    while(!cur->leaf)
        cur = cur->children[cur->n];
    return cur->keys[cur->n - 1];
}

Record BTreeNode::getSucc(int idx) {
    BTreeNode *cur = children[idx+1];
    while(!cur->leaf)
        cur = cur->children[0];
    return cur->keys[0];
}

void BTreeNode::fill(int idx) {
    if(idx != 0 && children[idx-1]->n >= order)
        borrowFromPrev(idx);
    else if(idx != n && children[idx+1]->n >= order)
        borrowFromNext(idx);
    else {
        if(idx != n)
            merge(idx);
        else
            merge(idx-1);
    }
}

void BTreeNode::borrowFromPrev(int idx) {
    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx-1];
    
    child->keys.insert(child->keys.begin(), keys[idx-1]);
    if(!child->leaf)
        child->children.insert(child->children.begin(), sibling->children[sibling->n]);
    keys[idx-1] = sibling->keys[sibling->n - 1];
    sibling->keys.pop_back();
    if(!sibling->leaf)
        sibling->children.pop_back();
    child->n++;
    sibling->n--;
}

void BTreeNode::borrowFromNext(int idx) {
    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx+1];
    
    child->keys.push_back(keys[idx]);
    if(!child->leaf)
        child->children.push_back(sibling->children[0]);
    keys[idx] = sibling->keys[0];
    sibling->keys.erase(sibling->keys.begin());
    if(!sibling->leaf)
        sibling->children.erase(sibling->children.begin());
    child->n++;
    sibling->n--;
}

void BTreeNode::merge(int idx) {
    BTreeNode *child = children[idx];
    BTreeNode *sibling = children[idx+1];
    
    child->keys.push_back(keys[idx]);
    for (int i = 0; i < sibling->n; i++) {
        child->keys.push_back(sibling->keys[i]);
    }
    if(!child->leaf) {
        for (int i = 0; i <= sibling->n; i++) {
            child->children.push_back(sibling->children[i]);
        }
    }
    keys.erase(keys.begin() + idx);
    children.erase(children.begin() + idx + 1);
    child->n += sibling->n + 1;
    n--;
    delete sibling;
}

void BTreeNode::print(int level) {
    std::cout << std::string(level * 4, ' ') << "Nível " << level << ": ";
    for(int i = 0; i < n; i++)
        std::cout << keys[i].key << " ";
    std::cout << "\n";
    if(!leaf) {
        for(int i = 0; i <= n; i++)
            if(children[i])
                children[i]->print(level + 1);
    }
}

BTreeNode::~BTreeNode() {
    for(auto child : children) {
        if(child)
            delete child;
    }
}

void BTree::insert(const Record &record) {
    // Se a árvore estiver vazia, cria a raiz como folha
    if(root == nullptr) {
        root = new BTreeNode(order, true);
        root->keys.push_back(record);
        root->n = 1;
    } else {
        // Insere recursivamente
        root->insertNonFull(record);
        // Se a raiz ficar overfull (2*order+1 chaves), deve ser dividida
        if(root->n == 2 * order + 1) {
            BTreeNode *s = new BTreeNode(order, false);
            s->children.push_back(root);
            s->n = 0;
            s->splitChild(0, root);
            root = s;
        }
    }
}

void BTree::remove(int key) {
    if(!root) {
        std::cout << "A árvore está vazia.\n";
        return;
    }
    root->remove(key);
    if(root->n == 0) {
        BTreeNode *tmp = root;
        if(root->leaf)
            root = nullptr;
        else
            root = root->children[0];
        delete tmp;
    }
}

BTree::~BTree() {
    if(root)
        delete root;
}
