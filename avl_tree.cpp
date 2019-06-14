/* 
AVL Tree
Main source: https://habr.com/ru/post/150732/
Changes: 
1) Added std::list which allows to iterate through the tree
2) Other minor upgrades added in order to pass the task
*/

#include <iterator>
#include <list>

template<class ValueType>
class Set {
public:
    using init_list = std::initializer_list<ValueType>;
    using iterator = typename std::list<ValueType>::const_iterator;

    Set(): root(nullptr), sz(0), items() {}

    template<typename Iterator>
    Set(Iterator first, Iterator last): Set() {
        for (; first != last; ++first) {
            insert(*first);
        }
    }

    Set(const init_list &elems): Set() {
        for (const auto &elem : elems) {
            insert(elem);
        }
    }

    Set(const Set &other): Set() {
        if (this != &other) {
            for (auto &elem : other.items) {
                insert(elem);
            }
        }
    }
    Set& operator = (const Set &other) {
        if (this != &other) {
            clear(root);
            root = nullptr;
            sz = 0;
            items.clear();

            for (auto &elem : other.items) {
                insert(elem);
            }
        }

        return *this;
    }

    ~Set() {
        clear(root);
        root = nullptr;
        sz = 0;
        items.clear();
    }

    size_t size() const {
        return sz;
    }
    bool empty() const {
        return (sz == 0);
    }

    void insert(const ValueType &elem) {
        if (find(elem) == end()) {
            root = put(root, elem);
            sz++;
        }
    }
    void erase(const ValueType &elem) {
        if (find(elem) != end()) {
            root = del(root, elem);
            sz--;
        }

    }

    iterator begin() const {
        return items.cbegin();
    }
    iterator end() const {
        return items.cend();
    }

    iterator find(const ValueType &elem) const {
        return search(root, elem);
    }
    iterator lower_bound(const ValueType &elem) const {
        return lb(root, elem);
    }

private:
    struct Node {
        const ValueType val;
        int h;
        Node* left, *right;
        iterator iter;

        Node(const ValueType &v, iterator &it): val(v), h(1), left(nullptr), right(nullptr), iter(it) {}
    };

    Node *root;
    size_t sz;
    std::list<ValueType> items;

    void clear(Node *node) {
        if (!node) {
            return;
        }

        clear(node->left);
        clear(node->right);
        delete node;
    }

    int height(Node *node) {
        return (node ? node->h : 0);
    }
    int diff(Node *node) {
        return (node ? height(node->right) - height(node->left) : 0);
    }
    void relax(Node *node) {
        node->h = 1 + std::max(height(node->left), height(node->right));
    }

    // Left rotation
    Node* rotate_left(Node *node) {
        Node *center = node->right;
        node->right = center->left;
        center->left = node;
        relax(node);
        relax(center);
        return center;
    }
    // Right rotation
    Node* rotate_right(Node *node) {
        Node *center = node->left;
        node->left = center->right;
        center->right = node;
        relax(node);
        relax(center);
        return center;
    }

    // Main AVL function: fix tree's balance
    Node* balance(Node *node) {
        relax(node);
        int node_balance = diff(node);
        if (node_balance == 2) {
            if (diff(node->right) < 0) {
                // Right-left rotation
                node->right = rotate_right(node->right);
            }
            // Small left rotation
            return rotate_left(node);
        } else if (node_balance == -2) {
            if (diff(node->left) > 0) {
                // Left-right rotation
                node->left = rotate_left(node->left);
            }
            // Small right rotation
            return rotate_right(node);
        }

        return node;
    }

    Node* put(Node *node, const ValueType &elem) {
        if (!node) {
            auto next = lb(root, elem);
            iterator it;
            if (next == end()) {
                items.push_back(elem);
                it = --items.cend();
            } else {
                it = items.insert(next, elem);
            }
            return new Node(elem, it);
        }

        if (elem < node->val) {
            node->left = put(node->left, elem);
        } else {
            node->right = put(node->right, elem);
        }

        return balance(node);
    }

    // Functions that used in deletion in order to swap min and elem
    Node* find_min(Node *node) {
        return (node->left ? find_min(node->left) : node);
    }
    Node* remove_min(Node *node) {
        if (!node->left) {
            return node->right;
        }

        node->left = remove_min(node->left);
        return balance(node);
    }

    Node* del(Node *node, const ValueType &elem) {
        if (!node) {
            return nullptr;
        }

        if (elem < node->val) {
            node->left = del(node->left, elem);
        } else if (node->val < elem) {
            node->right = del(node->right, elem);
        } else {
            Node *left = node->left, *right = node->right;
            items.erase(node->iter);
            delete node;
            if (!right) {
                return left;
            }

            Node *mn = find_min(right);
            mn->right = remove_min(right);
            mn->left = left;
            return balance(mn);
        }

        return balance(node);
    }

    iterator search(Node *node, const ValueType &elem) const {
        if (!node) {
            return end();
        }

        if (elem < node->val) {
            return search(node->left, elem);
        } else if (node->val < elem) {
            return search(node->right, elem);
        } else {
            return node->iter;
        }

    }

    iterator lb(Node *node, const ValueType &elem) const {
        if (!node) {
            return end();
        }

        if (elem < node->val) {
            iterator left = lb(node->left, elem);
            return (left == end() ? node->iter : left);
        } else if (node->val < elem) {
            return lb(node->right, elem);
        } else {
            return node->iter;
        }
    }
};
