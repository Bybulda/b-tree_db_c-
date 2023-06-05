#ifndef MAI_LABS_B_TREE_H
#define MAI_LABS_B_TREE_H

#include "../associative_container/associative_container.h"
#include "../usefull_files/logger_holder.h"
#include "../logger/logger.h"
#include "../logger/logger_builder_concrete.h"
#include "../logger/logger_builder.h"
#include "../memory/memory_concrete.h"
#include "../usefull_files/memory_holder.h"
#include <iostream>
#include <stack>
#include <list>
#include <vector>



template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class b_tree:
        public associative_container<tkey, tvalue>,
        private logger_holder,
        private memory_holder
        {
protected: // struct
            struct node{
                typename associative_container<tkey, tvalue>::key_value_pair** keys_and_values;
                node** subtrees;
                unsigned int size;
            };

public: // rule of five
            explicit b_tree(size_t t, memory* allocator = nullptr, logger* logger = nullptr);
            b_tree(b_tree const& other);
            b_tree(b_tree &&other) noexcept;
            b_tree &operator=(b_tree const& other);
            b_tree &operator=(b_tree && other) noexcept;
            ~b_tree() override;

public: // get-set-methods
            void insert(
                    tkey const &key,
                    tvalue const &&value) override;

            tvalue const &get(
                    tkey const &key) override;

            void remove(
                    tkey const &key) override;

            std::list<tvalue> find_diap(tkey const &start, tkey const &end) override;

            void set(tkey const& key, tvalue const&& value) override;

            void traverse_start();

            void try_iter();

private: // logger-mem getter
            logger* get_logger() const noexcept override;

            memory* get_memory() const noexcept override;

            void traverse(node** root);

private: // allocation and deletion methods
            node** safe_allocation_node(int _t);

            typename associative_container<tkey, tvalue>::key_value_pair** safe_allocation_key_val(int _t);

            typename associative_container<tkey, tvalue>::key_value_pair* safe_alloc_pair(tkey const &key, tvalue const &&value);

            typename associative_container<tkey, tvalue>::key_value_pair* safe_alloc_pair(tkey const &key, tvalue const &value);

            node* safe_initialize(int _t);

            void delete_leaf(node* leaf);

            void delete_node(node* root);

private: // helper_methods
            void split_child(node** root, int index);

            void insert_non_full(node** parent, typename associative_container<tkey, tvalue>::key_value_pair*);

            std::tuple
            <
            bool, size_t, typename b_tree<tkey, tvalue, tkey_comparer>::node**,
            std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **>
            >
            search(tkey const &key) const;

            std::tuple<bool, size_t, node**, std::stack<node**>> find_place(tkey const &key) const;

            void remove_inner(std::stack<node**> way_down, node** curr, int index);

            void remove_leaf(node** curr, int index);

            void merge();

            void borrow_right();

            void borrow_left();

            node* copy_tree(node* copy_from);

            node* copy();


    class infix_iter{
        std::stack<std::pair<node*, int>> way;
        node* root, *cur;
        int curr_index;
    public:
        explicit infix_iter(b_tree::node* start){
            root = start;
            cur = start;
            curr_index = 0;
        }
        void begin();

        std::pair<node*, int> next();
    };

private: // fields
    size_t _t; // min degree
    node* _root;
    memory* _allocator;
    logger* _logger;
    tkey_comparer comparer;

    infix_iter begin_traverse();


    void btree_shift_to_left_child(node *root, int pos, node *y, node *z);
    void btree_shift_to_right_child(node *root, int pos, node *y, node *z);

    typename associative_container<tkey, tvalue>::key_value_pair* btree_search_successor(node *root);

    typename associative_container<tkey, tvalue>::key_value_pair* btree_search_predecessor(node *root);

    void btree_delete_nonone(node *root, tkey const& target);

    node* btree_delete(node* root, tkey const& target);

    void btree_merge_child(node *root, int pos, node *y, node *z);
};

template<typename tkey, typename tvalue, typename tkey_comparer>
typename associative_container<tkey, tvalue>::key_value_pair *
b_tree<tkey, tvalue, tkey_comparer>::safe_alloc_pair(const tkey &key, const tvalue &value) {
    auto mem = reinterpret_cast<typename associative_container<tkey, tvalue>::key_value_pair*>
    (allocate_with_guard(sizeof(typename associative_container<tkey, tvalue>::key_value_pair)));
    new (mem) typename associative_container<tkey, tvalue>::key_value_pair{key, value};
    return mem;
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::set(const tkey &key, const tvalue &&value) {
    if(_root == nullptr){
        return;
    }
    auto res_search = search(key);
    int index = std::get<1>(res_search);
    auto node_ch = std::get<2>(res_search);
    if (index == -1){
        return;
    }
    (*node_ch)->keys_and_values[index]->value = std::move(value);
}


// REGION ITER
template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::infix_iter b_tree<tkey, tvalue, tkey_comparer>::begin_traverse()  {
    return infix_iter(_root);
}
template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::infix_iter::begin() {
    node* curr = root;
    if(curr == nullptr){
        cur = curr;
        curr_index = 0;
        return;
    }
    while (curr->subtrees[0] != nullptr){
        way.push(std::make_pair(curr, 0));
        curr = curr->subtrees[0];
    }
    cur = curr;
    curr_index = 0;
}
template<typename tkey, typename tvalue, typename tkey_comparer>
typename std::pair<typename b_tree<tkey, tvalue, tkey_comparer>::node *, int> b_tree<tkey, tvalue, tkey_comparer>::infix_iter::next() {
    if (cur != nullptr){
        if (cur->subtrees[0] == nullptr) {
            if (curr_index < cur->size) {
                curr_index++;
                return std::make_pair(cur, curr_index - 1);
            }
            if (way.empty()) {
                return std::make_pair(nullptr, -1);
            }
            else{
                auto now = way.top();
                way.pop();
                cur = now.first;
                curr_index = now.second;
                goto in_node;
            }
        } else{
            in_node:
            if(cur->size > curr_index){
                auto res = std::make_pair(cur, curr_index);
                way.push(std::make_pair(cur, curr_index + 1));
                cur = cur->subtrees[curr_index + 1];
                curr_index = 0;
                while(cur->subtrees[0] != nullptr){
                    way.push(std::make_pair(cur, 0));
                    cur = cur->subtrees[0];
                }
                return res;
            }
            else if(cur->size <= curr_index){
                if (way.empty()){
                    return std::make_pair(nullptr, -1);
                }
                auto now = way.top();
                way.pop();
                cur = now.first;
                curr_index = now.second;
                goto in_node;
            }
        }
    }
    return std::make_pair(nullptr, -1);
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::try_iter() {
    auto iter = begin_traverse();
    iter.begin();
    int going = 1;
    int counter = 0;
    while(going){
        auto res = iter.next();
        if (res.second == -1){
            going = 0;
        }
        else{
            counter++;
            std::cout << res.first->keys_and_values[res.second]->value << std::endl;
        }

    }
}
//REGION EMD ITER

// COPY REGION END
template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::node *b_tree<tkey, tvalue, tkey_comparer>::copy() {
    return copy_tree(_root);
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::node* b_tree<tkey, tvalue, tkey_comparer>::copy_tree(b_tree::node *copy_from) {
    if(copy_from != nullptr){
        auto nw_root = safe_initialize(_t);
        for (int i = 0; i < copy_from->size; ++i) {
            auto pair = safe_alloc_pair(copy_from->keys_and_values[i]->key, copy_from->keys_and_values[i]->value);
            nw_root->keys_and_values[i] = pair;
            nw_root->keys_and_values[i]->key = copy_from->keys_and_values[i]->key;
            nw_root->keys_and_values[i]->value = copy_from->keys_and_values[i]->value;
            nw_root->size++;
            nw_root->subtrees[i] = copy_tree(copy_from->subtrees[i]);

        }
        nw_root->subtrees[copy_from->size] = copy_tree(copy_from->subtrees[copy_from->size]);
        return nw_root;
    }
    return nullptr;
}
// COPY REGION END

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::remove_leaf(b_tree::node **curr, int index) {
    deallocate_with_guard((*curr)->keys_and_values[index]);
    (*curr)->size -= 1;
    for(int i = index; i < (*curr)->size; i++){
        (*curr)->keys_and_values[i] = (*curr)->keys_and_values[i + 1];
    }
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::remove_inner(std::stack<node**> way_down,
        b_tree::node **curr, int index) {
    if((*curr)->subtrees[0] == nullptr){
        if ((*curr)->size >=_t){
            remove_leaf(curr, index);
        }
        auto parent = way_down.top();
        int indx = 0;
        for (int i = 0; i <= (*parent)->size; ++i) {
            if ((*parent)->subtrees[i] == *curr){
                indx = i;
                break;
            }
        }
        if(indx > 0){
            auto left_sibling = &((*parent)->subtrees[indx - 1]);
            if((*parent)->subtrees[indx - 1] >= _t){
                auto tmp = (*parent)->keys_and_values[indx];
                deallocate_with_guard((*curr)->keys_and_values[index]);
                (*curr)->keys_and_values[index] = nullptr;
                (*parent)->keys_and_values[indx] = std::move((*left_sibling)->keys_and_values[(*left_sibling)->size - 1]);
                (*left_sibling)->keys_and_values[(*left_sibling)->size - 1] = nullptr;
                for (int i = index; i < (*curr)->size; ++i) {
                    (*curr)->keys_and_values[i] = (*curr)->keys_and_values[i + 1];
                }
                (*left_sibling)->size -= 1;
                for (int i = (*curr)->size; i > -1; --i) {
                    (*curr)->keys_and_values[i + 1] = (*curr)->keys_and_values[i];
                }
                (*curr)->keys_and_values[0] = std::move(tmp);
            }
        }
    }
    else{
        node** cr = &((*curr)->subtrees[index]);
        node** nxt = (&(*curr)->subtrees[index + 1]);
        std::stack<node**> way_pred, way_next;
        while((*cr)->subtrees[0] != nullptr){
            way_pred.push(cr);
            cr = &((*cr)->subtrees[(*cr)->size]);
            way_next.push(nxt);
            nxt = &((*nxt)->subtrees[0]);
        }
        if ((*cr)->size >= _t){
            auto tmp = (*cr)->keys_and_values[(*cr)->size];
            (*cr)->keys_and_values[(*cr)->size] = std::move((*curr)->keys_and_values[index]);
            (*curr)->keys_and_values[index] = std::move(tmp);
            remove_leaf(cr, (*cr)->size);
        }
        else if((*nxt)->size >= _t){
            auto tmp = (*nxt)->keys_and_values[0];
            (*nxt)->keys_and_values[0] = std::move((*curr)->keys_and_values[index]);
            (*curr)->keys_and_values[index] = std::move(tmp);
            remove_leaf(nxt, 0);
        }
    }
}

template<typename tkey, typename tvalue, typename tkey_comparer>
std::list<tvalue> b_tree<tkey, tvalue, tkey_comparer>::find_diap(const tkey &start, const tkey &end) {
    std::list<tvalue> values;
    auto iter = begin_traverse();
    iter.begin();
    while(true){
        auto res = iter.next();
        auto rs_node = res.first;
        int rs_ind = res.second;
        if (rs_ind == -1){
            break;
        }
        if (comparer(start, rs_node->keys_and_values[rs_ind]->key) <= 0 && comparer(end, rs_node->keys_and_values[rs_ind]->key) >= 0)
            values.push_back(rs_node->keys_and_values[rs_ind]->value);
    }
    return values;
}

//TRAVERSE
template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::traverse_start() {
    traverse(&_root);
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::traverse(node** root) {
    if(root != nullptr){
        std::string vals;
        for (int i = 0; i < (*root)->size; ++i) {
            if((*root)->subtrees[0] != nullptr){
                traverse(&((*root)->subtrees[i]));

            }
            vals += std::to_string((*root)->keys_and_values[i]->key) + " ";
        }
        if((*root)->subtrees[0] != nullptr){
            traverse(&((*root)->subtrees[(*root)->size]));

        }
        std::cout << vals << std::endl << " ";
    }
}
//TRAVERSE END

//DELETION
template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::delete_node(b_tree::node *root) {
    if(root != nullptr && root->subtrees[0] == nullptr){
        delete_leaf(root);
    }
    else if (root != nullptr){
        for (int i = 0; i < root->size; ++i) {
            delete_node(root->subtrees[i]);
            root->subtrees[i] = nullptr;
        }
        delete_leaf(root);
    }
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::delete_leaf(b_tree::node *leaf) {
    for (int i = 0; i < leaf->size; ++i) {
        leaf->keys_and_values[i]->~key_value_pair();
        deallocate_with_guard(reinterpret_cast<void*>(leaf->keys_and_values[i]));
        leaf->keys_and_values[i] = nullptr;
    }
    deallocate_with_guard(reinterpret_cast<void*>(leaf->keys_and_values));
    leaf->keys_and_values = nullptr;
    deallocate_with_guard(reinterpret_cast<void*>(leaf->subtrees));
    leaf->subtrees = nullptr;
    leaf->~node();
    deallocate_with_guard(reinterpret_cast<void*>(leaf));
}
//DELETION END

// RULE OF FIVE
template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer>::b_tree(size_t t, memory* allocator, logger* logger) :
_root(nullptr), _allocator(allocator), _logger(logger)
{
    _t = t < 2 ? 2 : t;
    trace_with_guard("B-tree class have been created");
}



template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer>::~b_tree(){
    delete_node(_root);
    warning_with_guard("done");
}

template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer>::b_tree(b_tree const& other){
    comparer = other.comparer;
    _root = nullptr;
    _allocator = other._allocator;
    _logger = other._logger;
    _t = other._t;
    _root = const_cast<b_tree&>(other).copy();
}

template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer>::b_tree(b_tree &&other) noexcept{
    _root = std::move(other._root);
    other._root = nullptr;
    comparer = std::move(other.comparer);
    _t = std::move(other._t);
    _logger = other._logger;
    other._logger = nullptr;
    _allocator = other._allocator;
    other._allocator = nullptr;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer> &b_tree<tkey, tvalue, tkey_comparer>::operator=(b_tree const& other){
    if(this == &other){
        return *this;
    }
    _root = nullptr;
    comparer = other.comparer;
    _logger = other._logger;
    _allocator = other._allocator;
    _t = other._t;
    _root = const_cast<b_tree&>(other).copy();

    return *this;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
b_tree<tkey, tvalue, tkey_comparer> &b_tree<tkey, tvalue, tkey_comparer>::operator=(b_tree && other) noexcept{
    if(this == &other){
        return *this;
    }
    _root = std::move(other._root);
    other._root = nullptr;
    comparer = std::move(other.comparer);
    _t = std::move(other._t);
    _logger = other._logger;
    other._logger = nullptr;
    _allocator = other._allocator;
    other._allocator = nullptr;

    return *this;
}

// RULE OF FIVE END

// LOGGER
template<typename tkey, typename tvalue, typename tkey_comparer>
logger* b_tree<tkey, tvalue, tkey_comparer>::get_logger() const noexcept{
    return _logger;
}
// LOGGER END

// SAVE ALLOC
template<typename tkey, typename tvalue, typename tkey_comparer>
memory* b_tree<tkey, tvalue, tkey_comparer>::get_memory() const noexcept{
    return _allocator;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename associative_container<tkey, tvalue>::key_value_pair *b_tree<tkey, tvalue, tkey_comparer>::safe_alloc_pair(tkey const &key, tvalue const &&value) {
    auto mem = reinterpret_cast<typename associative_container<tkey, tvalue>::key_value_pair*>(allocate_with_guard(sizeof(typename associative_container<tkey, tvalue>::key_value_pair)));
    new (mem) typename associative_container<tkey, tvalue>::key_value_pair{key, std::move(value)};
    return mem;
}


template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::node** b_tree<tkey, tvalue, tkey_comparer>::safe_allocation_node(int _t){
    auto mem = allocate_with_guard(sizeof(typename b_tree<tkey, tvalue, tkey_comparer>::node*)*(2*_t));
    if (mem == nullptr){
        throw std::runtime_error("COLLAPSED");
    }
    auto steady = reinterpret_cast<typename b_tree<tkey, tvalue, tkey_comparer>::node**>(mem);
    for (int i = 0; i < (2*_t); ++i) {
        steady[i] = nullptr;
    }
    return steady;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename associative_container<tkey, tvalue>::key_value_pair **b_tree<tkey, tvalue, tkey_comparer>::safe_allocation_key_val(int _t) {
    auto mem = reinterpret_cast<
            typename associative_container<tkey, tvalue>::key_value_pair**>
    (allocate_with_guard(sizeof(typename associative_container<tkey, tvalue>::key_value_pair*)*(2*_t - 1)));
    for (int i = 0; i < ((2*_t) - 1); ++i) {
        mem[i] = nullptr;
    }
    return mem;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::node* b_tree<tkey, tvalue, tkey_comparer>::safe_initialize(int _t) {
    auto mem = reinterpret_cast<node*>(allocate_with_guard(sizeof(typename b_tree<tkey, tvalue, tkey_comparer>::node)));
    if (mem == nullptr){
        throw std::runtime_error("COLLAPSED");
    }
    auto keys = safe_allocation_key_val(_t);
    auto roots = safe_allocation_node(_t);
    node* root = new (mem) node{keys, roots, 0};
//    root->size = 0;
//    root->keys_and_values = safe_allocation_key_val();
//    root->subtrees = safe_allocation_node();
    return root;
}

// SAVE ALLOC ENDS

//B-tree METHODS
template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::insert_non_full(b_tree::node **parent,
                                                          typename associative_container<tkey, tvalue>::key_value_pair * pair) {
    int n = (*parent)->size - 1;
    if ((*parent)->subtrees[0] == nullptr){
        while (n >= 0 && comparer(pair->key, (*parent)->keys_and_values[n]->key) < 0){
            (*parent)->keys_and_values[n + 1] = (*parent)->keys_and_values[n];
            n -= 1;
        }
        (*parent)->keys_and_values[n + 1] = pair;
        (*parent)->size += 1;
    }
    else{
        while (n >= 0 && comparer(pair->key, (*parent)->keys_and_values[n]->key) < 0){
            n -= 1;
        }
        if ((*parent)->subtrees[n + 1]->size == (2*_t - 1)){
            split_child(parent, n + 1);
            if (comparer(pair->key, (*parent)->keys_and_values[n + 1]->key) > 0){
                n += 1;
            }
        }
        insert_non_full(&(*parent)->subtrees[n + 1], pair);
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::split_child(
        b_tree::node **root,
        int index) {
    warning_with_guard("starting to split with head val ");
    node* z = safe_initialize(_t);
    node* y = (*root)->subtrees[index];
    z->size = _t - 1;
    for (int i = 0; i < (_t - 1); ++i) {
//        _logger->log("splitting child, giving z keys on pos " + std::to_string(i + _t), logger::severity::critical);
        z->keys_and_values[i] = std::move(y->keys_and_values[i + _t]);
        y->keys_and_values[i + _t] = nullptr;
    }
    if (y->subtrees[0] != nullptr){
        for (int i = 0; (i < _t); ++i) {
//            _logger->log("splitting child, giving z trees on pos " + std::to_string(i + _t), logger::severity::critical);
            z->subtrees[i] = std::move(y->subtrees[i + _t]);
            y->subtrees[i + _t] = nullptr;
        }
    }
    y->size = _t - 1;
    for (int i = (*root)->size; i > index; --i) {
//        _logger->log("splitting child, giving parent trees on pos " + std::to_string(i + 1) + " from " + std::to_string(i), logger::severity::critical);
        (*root)->subtrees[i + 1] = std::move((*root)->subtrees[i]);
        (*root)->subtrees[i] = nullptr;
    }
//    _logger->log("splitting child, giving parent z subtree on pos " + std::to_string(index + 1), logger::severity::critical);
    (*root)->subtrees[index + 1] = z;
    for (int i = (*root)->size - 1; i > index; --i) {
//        _logger->log("splitting child, moving parent keys on pos " + std::to_string(i + 1) + " from " + std::to_string(i), logger::severity::critical);
        (*root)->keys_and_values[i+1] = std::move((*root)->keys_and_values[i]);
        (*root)->keys_and_values[i] = nullptr;
    }
//    warning_with_guard("Suka");
//    _logger->log("setting to parent y _t - 1 key" + std::to_string(index) + " from " + std::to_string(_t - 1), logger::severity::critical);
    (*root)->keys_and_values[index] = std::move(y->keys_and_values[_t - 1]);
    y->keys_and_values[_t - 1] = nullptr;
    (*root)->size += 1;
}


template<typename tkey, typename tvalue, typename tkey_comparer>
std::tuple<bool, size_t, typename b_tree<tkey, tvalue, tkey_comparer>::node**, std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **>>
b_tree<tkey, tvalue, tkey_comparer>::find_place(const tkey &key) const{
    std::stack<node**> way;
    node* curr = _root;
    while(curr != nullptr){
        for(int i = 0; i < (2*_t-1); i++){
            if(curr->keys[i] == nullptr){
                return std::make_tuple<bool, size_t, node* , std::stack<node *>>(true, i, curr, way);
            }
            else if(comparer(curr->keys[i], key) > 0){
                way.push(&curr);
                curr = curr->pointers[i];
                break;
            }
        }
        if (curr->pointers[0] == nullptr){
            return std::tuple<bool, size_t, node* , std::stack<node *>>(false, 0, nullptr, way);
        }
    }
}




template<typename tkey, typename tvalue, typename tkey_comparer>
std::tuple<bool, size_t, typename b_tree<tkey, tvalue, tkey_comparer>::node**, std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **>>
b_tree<tkey, tvalue, tkey_comparer>::search(tkey const &key) const{
    auto curr = _root;
    std::stack<typename b_tree<tkey, tvalue, tkey_comparer>::node **> trace;
    trace.push(nullptr);
    while(true){
        int i = 0;
        while(curr->size != i && comparer(curr->keys_and_values[i]->key, key) < 0){
            i++;
        }
        if (curr->size != i && comparer(curr->keys_and_values[i]->key, key) == 0){
            return std::tuple<bool, size_t, node**, std::stack<node **>>(curr->subtrees[0] == nullptr, i, &curr, trace);
        }
        if(curr->subtrees[0] == nullptr){
            return std::tuple<bool, size_t, node**, std::stack<node **>>(true, -1, nullptr, trace);
        }
        else{
            trace.push(&curr);
            curr = curr->subtrees[i];
        }
    }
}

template<typename tkey, typename tvalue, typename tkey_comparer>
tvalue const &b_tree<tkey, tvalue, tkey_comparer>::get(tkey const &key){
//    auto root = _reading->read(key, &_root);
    auto root = search(key);
    int index = std::get<1>(root);
    node** res_node = std::get<2>(root);

    if (res_node == nullptr){
        throw std::runtime_error("NO VALUE LIKE THIS");
    }
    return (*res_node)->keys_and_values[index]->value;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::remove(tkey const &key){
    if(_root == nullptr){
        critical_with_guard("Root have not been initialized!");
        throw std::runtime_error("critical");
    }
    auto removing = search(key);
    int index = std::get<1>(removing);
    if (index == -1){
        throw std::runtime_error("critical");
    }
    auto root = std::get<2>(removing);
    typename associative_container<tkey, tvalue>::key_value_pair* pr = (*root)->keys_and_values[index];
//    remove_inner(root, index);
    _root = btree_delete(_root, key);
//    auto tvalue_gain = std::move(pr->value);
    pr->~key_value_pair();
    deallocate_with_guard(pr);
//    return std::move(tvalue_gain);
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::insert(tkey const &key, tvalue const &&value){
//    _insertion->insert(key, std::move(value));
    b_tree<tkey, tvalue, tkey_comparer>::node* r = this->_root;
    if(_root == nullptr){
        _root = safe_initialize(_t);
        auto pair = safe_alloc_pair(key, std::move(value));
        _root->keys_and_values[0] = std::move(pair);
        _root->size += 1;
    }
    else{
        auto pair = safe_alloc_pair(key, std::move(value));
        if(r->size ==( 2*_t - 1)){
            b_tree<tkey, tvalue, tkey_comparer>::node* root = safe_initialize(_t);
            root->subtrees[0] = r;
            root->size = 0;

            split_child(&root, 0);
            insert_non_full(&root, pair);
            _root = root;
        }
        else{
            insert_non_full(&r, pair);
        }
    }
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::btree_merge_child(node *root, int pos, node *y, node *z)
{

    y->size = 2 * _t - 1;
    for(int i = _t; i < 2 * _t - 1; i++) {
        y->keys_and_values[i] = z->keys_and_values[i-_t];
    }
    y->keys_and_values[_t-1] = root->keys_and_values[pos];// k[pos]下降为y的中间节点


    if(z->subtrees[0] != nullptr) {
        for(int i = _t; i < 2 * _t; i++) {
            y->subtrees[i] = z->subtrees[i-_t];
        }
    }


    for(int j = pos + 1; j < root->size; j++) {
        root->keys_and_values[j-1] = root->keys_and_values[j];
        root->subtrees[j] = root->subtrees[j+1];
    }

    root->size -= 1;
    z->~node();
    deallocate_with_guard(z);
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename b_tree<tkey, tvalue, tkey_comparer>::node* b_tree<tkey, tvalue, tkey_comparer>::btree_delete(node* root, tkey const& target)
{

    if(1 == root->size) {
        node *y = root->subtrees[0];
        node *z = root->subtrees[1];
        if(nullptr != y && nullptr != z &&
           _t - 1 == y->size && _t - 1 == z->size) {
            btree_merge_child(root, 0, y, z);
            root->~node();
            deallocate_with_guard(root);
            btree_delete_nonone(y, target);
            return y;
        } else {
            btree_delete_nonone(root, target);
            return root;
        }
    } else {
        btree_delete_nonone(root, target);
        return root;
    }
}


template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::btree_delete_nonone(node *root, tkey const& target)
{
    if(root->subtrees[0] == nullptr) {
        // 如果在叶子节点，直接删除
        int i = 0;
        while(i < root->size && comparer(target, root->keys_and_values[i]->key) > 0) i++;
        if(comparer(target, root->keys_and_values[i]->key) == 0) { // добавить сравнение
            for(int j = i + 1; j < 2 * _t - 1; j++) {
                root->keys_and_values[j-1] = root->keys_and_values[j];
            }
            root->size -= 1;

//            btree_node_num-=1;

        } else {
            printf("target not found\n");
        }
    } else {
        int i = 0;
        node *y = NULL, *z = NULL;
        while(i < root->size && comparer(target, root->keys_and_values[i]->key) > 0) i++;
        if(i < root->size && comparer(target, root->keys_and_values[i]->key) == 0) {
            // 如果在分支节点找到target
            y = root->subtrees[i];
            z = root->subtrees[i+1];
            if(y->size > _t - 1) {
                // 如果左分支关键字多于M-1，则找到左分支的最右节点prev，替换target
                // 并在左分支中递归删除prev,情况2（a)
                auto pre = btree_search_predecessor(y); // ключ
                root->keys_and_values[i] = pre;
                btree_delete_nonone(y, pre->key);
            } else if(z->size > _t - 1) {
                // 如果右分支关键字多于M-1，则找到右分支的最左节点next，替换target
                // 并在右分支中递归删除next,情况2(b)
                auto next = btree_search_successor(z); // ключ
                root->keys_and_values[i] = next;
                btree_delete_nonone(z, next->key);
            } else {
                // 两个分支节点数都为M-1，则合并至y，并在y中递归删除target,情况2(c)
                btree_merge_child(root, i, y, z);
                btree_delete(y, target);
            }
        } else {
            // 在分支没有找到，肯定在分支的子节点中
            y = root->subtrees[i];
            if(i < root->size) {
                z = root->subtrees[i+1];
            }
            node *p = NULL;
            if(i > 0) {
                p = root->subtrees[i-1];
            }

            if(y->size == _t - 1) {
                if(i > 0 && p->size > _t - 1) {
                    // 左邻接节点关键字个数大于M-1
                    //情况3(a)
                    btree_shift_to_right_child(root, i-1, p, y);
                } else if(i < root->size && z->size > _t - 1) {
                    // 右邻接节点关键字个数大于M-1
                    // 情况3(b)
                    btree_shift_to_left_child(root, i, y, z);
                } else if(i > 0) {
                    // 情况3（c)
                    btree_merge_child(root, i-1, p, y); // note
                    y = p;
                } else {
                    // 情况3(c)
                    btree_merge_child(root, i, y, z);
                }
                btree_delete_nonone(y, target);
            } else {
                btree_delete_nonone(y, target);
            }
        }

    }
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename associative_container<tkey, tvalue>::key_value_pair* b_tree<tkey, tvalue, tkey_comparer>::btree_search_predecessor(node *root)
{
    node *y = root;
    while(y->subtrees[0] != nullptr) {
        y = y->subtrees[y->size];
    }
    return y->keys_and_values[y->size-1];
}

template<typename tkey, typename tvalue, typename tkey_comparer>
typename associative_container<tkey, tvalue>::key_value_pair* b_tree<tkey, tvalue, tkey_comparer>::btree_search_successor(node *root)
{
    node *z = root;
    while(z->subtrees[0] != nullptr) {
        z = z->subtrees[0];
    }
    return z->keys_and_values[0];
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::btree_shift_to_right_child(node *root, int pos,
                                       node *y, node *z)
{
    z->size += 1;
    for(int i = z->size -1; i > 0; i--) {
        z->keys_and_values[i] = z->keys_and_values[i-1];
    }
    z->keys_and_values[0]= root->keys_and_values[pos];
    root->keys_and_values[pos] = y->keys_and_values[y->size-1];

    if(z->subtrees[0] != nullptr) {
        for(int i = z->size; i > 0; i--) {
            z->subtrees[i] = z->subtrees[i-1];
        }
        z->subtrees[0] = y->subtrees[y->size];
    }

    y->size -= 1;
}

template<typename tkey, typename tvalue, typename tkey_comparer>
void b_tree<tkey, tvalue, tkey_comparer>::btree_shift_to_left_child(node *root, int pos,
                                      node *y, node *z)
{
    y->size+= 1;
    y->keys_and_values[y->size-1] = root->keys_and_values[pos];
    root->keys_and_values[pos] = z->keys_and_values[0];

    for(int j = 1; j < z->size; j++) {
        z->keys_and_values[j-1] = z->keys_and_values[j];
    }

    if(z->subtrees[0] != nullptr) {
        y->subtrees[y->size] = z->subtrees[0];
        for(int j = 1; j <= z->size; j++) {
            z->subtrees[j-1] = z->subtrees[j];
        }
    }

    z->size -= 1;
}


#endif //MAI_LABS_B_TREE_H
