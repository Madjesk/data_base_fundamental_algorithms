#ifndef BINARY_SEARCH_TREE_H
#define BINARY_SEARCH_TREE_H

#include "associative_container.h"
#include "../memory_guard/memory_guard.h"
#include "../logger_guard/logger_guard.h"
#include "logger.h"
#include <stack>
#include <sstream>
#include <iostream>

template <typename T>
std::string to_string(
        T object)
{
    std::stringstream stream;
    stream << object;

    return stream.str();
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class binary_search_tree : public associative_container<tkey, tvalue>, protected memory_guard, protected logger_guard
{

protected:

    struct node
    {
        tkey key;
        tvalue value;
        node *left_subtree;
        node *right_subtree;
        bool is_left_child;
        bool is_right_child;
    };

public:

    class prefix_iterator final
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        node *_curr_node;
        node *_end_node;
        std::stack<node *> _way;

    public:

        explicit prefix_iterator(binary_search_tree<tkey, tvalue, tkey_comparer> *tree, node *curr_node);

        bool operator==(prefix_iterator const &other) const;

        bool operator!=(prefix_iterator const &other) const;

        prefix_iterator& operator++();

        prefix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    };

    class infix_iterator final
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        node *_curr_node;
        node *_end_node;
        std::stack<node *> _way;

    public:

        explicit infix_iterator(binary_search_tree<tkey, tvalue, tkey_comparer> *tree, node *curr_node);

        bool operator==(infix_iterator const &other) const;

        bool operator!=(infix_iterator const &other) const;

        infix_iterator& operator++();

        infix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    };

    class postfix_iterator final
    {

        friend class binary_search_tree<tkey, tvalue, tkey_comparer>;

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;
        node *_curr_node;
        node *_end_node;
        std::stack<node *> _way;

    public:

        explicit postfix_iterator(
                binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
                node *curr_node);

        bool operator==(postfix_iterator const &other) const;

        bool operator!=(postfix_iterator const &other) const;

        postfix_iterator &operator++();

        postfix_iterator operator++(int not_used);

        std::tuple<unsigned int, tkey const&, tvalue&> operator*() const;

    private:

        node *get_node() const;

    };

protected:

    class find_template_method
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    public:

        bool find(
                typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
                node *&subtree_address);

        explicit find_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

    private:

        bool find_inner(
                typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
                node *&subtree_address,
                std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_find_inner(
                typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
                node *&subtree_address,
                std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);
    };

    class insertion_template_method : private memory_guard, private logger_guard
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    private:

        memory *get_memory() const override;

        logger *get_logger() const override;

        virtual size_t get_node_size() const;

    public:

        explicit insertion_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

        void insert(tkey const &key, tvalue value);

    private:

        void insert_inner(
                tkey const &key,
                tvalue value,
                node *&subtree_root_address,
                std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_insert_inner(
                tkey const &key,
                node *&subtree_root_address,
                std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);
    };

    class removing_template_method : private memory_guard, private logger_guard
    {

    private:

        binary_search_tree<tkey, tvalue, tkey_comparer> *_tree;

    private:

        memory *get_memory() const override;

        logger *get_logger() const override;

        virtual void get_info_deleted_node(node *deleted_node, std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);

    public:

        explicit removing_template_method(binary_search_tree<tkey, tvalue, tkey_comparer> *tree);

        tvalue remove(tkey const &key, node *&tree_root_address);

    private:

        virtual tvalue remove_inner(tkey const &key, node *&subtree_root_address, std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);

    protected:

        virtual void after_remove_inner(tkey const &key, node *&subtree_root_address, std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive);

    };

protected:

    tkey_comparer _comparator;
    node *_root;
    memory *_allocator{};
    logger *_logger{};
    find_template_method *_find;
    insertion_template_method *_insertion;
    removing_template_method *_removing;

public:

    explicit binary_search_tree(memory *allocator_binary_search_tree = nullptr, logger *logger_tree = nullptr);

    ~binary_search_tree() override;

protected:

    binary_search_tree(
            find_template_method *find,
            insertion_template_method *insert,
            removing_template_method *remove,
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr,
            node *root = nullptr);

public:

    void insert(tkey const &key, tvalue value) override;

    void update(tkey const &key, tvalue value) override;

    std::vector<tvalue> get_range(tkey const &lower_bound, tkey const &upper_bound) const override;

    bool find(typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value) override;

    tvalue remove(tkey const &key) override;

public:

    prefix_iterator begin_prefix() const noexcept;

    prefix_iterator end_prefix() const noexcept;

    infix_iterator begin_infix() const noexcept;

    infix_iterator end_infix() const noexcept;

    postfix_iterator begin_postfix() const noexcept;

    postfix_iterator end_postfix() const noexcept;

protected:

    virtual void left_rotate(node **subtree_root, node **parent) const;

    virtual void right_rotate(node **subtree_root, node **parent) const;

private:

    memory *get_memory() const final;

    logger *get_logger() const final;
    void delete_tree(node *&subtree_root_address);
private:

    void copy_subtree(node *src);

public:

    binary_search_tree(const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy);

    binary_search_tree& operator=(const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy);

    binary_search_tree(binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept;

    binary_search_tree& operator=(binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept;
};

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::prefix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        node *curr_node)
{
    _tree = tree;

    node *end_node = _tree->_root;

    if (end_node != nullptr)
    {
        while (end_node->right_subtree != nullptr)
        {
            end_node = end_node->right_subtree;
        }
    }

    _end_node = end_node;

    if (curr_node != nullptr)
    {
        _curr_node = curr_node;

        node *node_search = _tree->_root;

        while (node_search != curr_node)
        {
            _way.push(node_search);

            if (_tree->_comparator(curr_node->key, node_search->key) > 0)
            {
                node_search = node_search->right_subtree;
            }
            else
            {
                node_search = node_search->left_subtree;
            }
        }
    }
    else
    {
        _curr_node = nullptr;
        _way = std::stack<node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator==(
        prefix_iterator const &other) const
{
    if (_way == other._way && _curr_node == other._curr_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator!=(
        prefix_iterator const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++()
{
    if (_curr_node == _end_node)
    {
        _curr_node = nullptr;
        _way = std::stack<node*>();

        return *this;
    }

    if (_curr_node->left_subtree != nullptr)
    {
        _way.push(_curr_node);
        _curr_node = _curr_node->left_subtree;

        return *this;
    }

    if (_curr_node->left_subtree == nullptr &&
        _curr_node->right_subtree != nullptr)
    {
        _way.push(_curr_node);
        _curr_node = _curr_node->right_subtree;

        return  *this;
    }

    if (_curr_node->left_subtree == nullptr &&
        _curr_node->right_subtree == nullptr)
    {
        while (!_way.empty() &&
               (_way.top()->right_subtree == _curr_node ||
                _way.top()->right_subtree == nullptr))
        {
            _curr_node = _way.top();
            _way.pop();
        }

        if (!_way.empty())
        {
            _curr_node = _way.top()->right_subtree;
        }

        return *this;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator++(
        int not_used)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _curr_node->key, _curr_node->value);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::infix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        node *curr_node)
{
    _tree = tree;

    node *end_node = _tree->_root;

    if (end_node != nullptr)
    {
        while (end_node->right_subtree != nullptr)
        {
            end_node = end_node->right_subtree;
        }
    }

    _end_node = end_node;

    if (curr_node != nullptr)
    {
        tkey_comparer functor;
        _curr_node = curr_node;

        node *node_search = _tree->_root;

        while (node_search != curr_node)
        {
            _way.push(node_search);

            if (_tree->_comparator(curr_node->key, node_search->key) > 0)
            {
                node_search = node_search->right_subtree;
            }
            else
            {
                node_search = node_search->left_subtree;
            }
        }
    }
    else
    {
        _curr_node = nullptr;
        _way = std::stack<node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator==(
        infix_iterator const &other) const
{
    if (_way == other._way && _curr_node == other._curr_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator!=(
        infix_iterator  const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++()
{
    if (_curr_node == _end_node)
    {
        _curr_node = nullptr;
        _way = std::stack<node*>();

        return *this;
    }

    if (_curr_node->right_subtree != nullptr)
    {
        _way.push(_curr_node);
        _curr_node = _curr_node->right_subtree;

        while (_curr_node->left_subtree != nullptr)
        {
            _way.push(_curr_node);
            _curr_node = _curr_node->left_subtree;
        }
    }
    else
    {
        if (!_way.empty())
        {
            if (_way.top()->left_subtree == _curr_node)
            {
                _curr_node = _way.top();
                _way.pop();
            }
            else
            {
                while (!_way.empty() &&
                       _way.top()->right_subtree == _curr_node)
                {
                    _curr_node = _way.top();
                    _way.pop();
                }

                if (!_way.empty())
                {
                    _curr_node = _way.top();
                    _way.pop();
                }
            }
        }
    }

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator++(
        int not_used)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _curr_node->key, _curr_node->value);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::postfix_iterator(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree,
        node *curr_node)
{
    _tree = tree;
    _end_node = _tree->_root;

    if (curr_node != nullptr)
    {
        tkey_comparer functor;
        _curr_node = curr_node;

        node *node_search = _tree->_root;

        while (node_search != curr_node)
        {
            _way.push(node_search);

            if (_tree->_comparator(curr_node->key, node_search->key) > 0)
            {
                node_search = node_search->right_subtree;
            }
            else
            {
                node_search = node_search->left_subtree;
            }
        }
    }
    else
    {
        _curr_node = nullptr;
        _way = std::stack<node*>();
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator==(
        postfix_iterator const &other) const
{
    if (_way == other._way && _curr_node == other._curr_node)
    {
        return true;
    }

    return false;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator!=(
        postfix_iterator  const &other) const
{
    return !(*this == other);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator &binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++()
{
    if (_curr_node == nullptr)
    {
        return *this;
    }

    if (_way.empty())
    {
        _curr_node = nullptr;
    }
    else
    {
        if (_way.top()->right_subtree == _curr_node)
        {
            _curr_node = _way.top();
            _way.pop();
        }
        else
        {
            if ((_curr_node = _way.top()->right_subtree) != nullptr)
            {
                while (_curr_node->left_subtree != nullptr ||
                       _curr_node->right_subtree != nullptr)
                {
                    _way.push(_curr_node);
                    _curr_node = _curr_node->left_subtree == nullptr ? _curr_node->right_subtree : _curr_node->left_subtree;
                }
            }
            else
            {
                _curr_node = _way.top();
                _way.pop();
            }
        }
    }

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator++(int not_used)
{
    typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator previous_state = *this;
    ++(*this);

    return previous_state;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::tuple<unsigned int, tkey const&, tvalue&> binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::operator*() const
{
    return std::tuple<unsigned int, tkey const&, tvalue&>(_way.size(), _curr_node->key, _curr_node->value);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator::get_node() const
{
    return _curr_node;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method::find(
        typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
        node *&subtree_address)
{
    std::stack<node**> path_to_subtree_root_exclusive;

    return find_inner(target_key_and_result_value, subtree_address, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method::find_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method::find_inner(
        typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
        node *&subtree_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{
    if (subtree_address == nullptr)
    {
        if (!path_to_subtree_root_exclusive.empty())
        {
            node **last_node = path_to_subtree_root_exclusive.top();
            path_to_subtree_root_exclusive.pop();

            after_find_inner(target_key_and_result_value, *last_node, path_to_subtree_root_exclusive);
        }

        return false;
    }

    tkey_comparer functor;
    int res_functor = functor(target_key_and_result_value->_key, subtree_address->key);

    if (res_functor == 0)
    {
        target_key_and_result_value->_value = subtree_address->value;

        after_find_inner(target_key_and_result_value, subtree_address, path_to_subtree_root_exclusive);

        return true;
    }

    if (subtree_address->left_subtree == nullptr && subtree_address->right_subtree == nullptr)
    {
        return false;
    }

    node* next_node;

    if (res_functor > 0) {
        next_node = subtree_address->right_subtree;
    } else {
        next_node = subtree_address->left_subtree;
    }
    path_to_subtree_root_exclusive.push(&subtree_address);

    return find_inner(target_key_and_result_value, next_node, path_to_subtree_root_exclusive);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method::after_find_inner(
        typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
        node *&subtree_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::get_memory() const
{
    return _tree->_allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::get_logger() const
{
    return _tree->_logger;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
size_t binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::get_node_size() const
{
    return sizeof(node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insertion_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insert(
        const tkey &key,
        tvalue value)
{
    std::stack<binary_search_tree::node**> path_to_subtree_root_exclusive;
    insert_inner(key, std::move(value), _tree->_root, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::insert_inner(
        const tkey &key,
        tvalue value,
        binary_search_tree::node *&subtree_root_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{
    if (subtree_root_address == nullptr)
    {
        subtree_root_address = reinterpret_cast<node*>(guard_allocate(get_node_size()));
        new (subtree_root_address) node{key, std::move(value), nullptr, nullptr};

        if (!path_to_subtree_root_exclusive.empty())
        {
            if (_tree->_comparator(key, (*path_to_subtree_root_exclusive.top())->key) > 0)
            {
                (*path_to_subtree_root_exclusive.top())->right_subtree = subtree_root_address;
            }
            else
            {
                (*path_to_subtree_root_exclusive.top())->left_subtree = subtree_root_address;
            }
        }

        after_insert_inner(key, subtree_root_address, path_to_subtree_root_exclusive);

        return;
    }
    else
    {
        if (_tree->_comparator(key, subtree_root_address->key) == 0)
        {
            subtree_root_address->value = value;
            return;
        }
    }

    binary_search_tree::node* node_next;

    if (_tree->_comparator(key, subtree_root_address->key) > 0) {
        node_next = subtree_root_address->right_subtree;
    } else {
        node_next = subtree_root_address->left_subtree;
    }

    path_to_subtree_root_exclusive.push(&subtree_root_address);
    insert_inner(key, std::move(value), node_next, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method::after_insert_inner(
        tkey const &key,
        node *&subtree_root_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::get_memory() const
{
    return _tree->_allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::get_logger() const
{
    return _tree->_logger;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::get_info_deleted_node(
        node *deleted_node,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::removing_template_method(
        binary_search_tree<tkey, tvalue, tkey_comparer> *tree) :
        _tree(tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::remove(
        tkey const &key,
        node *&tree_root_address)
{
    std::stack<binary_search_tree::node**> path_to_subtree_root_exclusive;

    return remove_inner(key, tree_root_address, path_to_subtree_root_exclusive);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::remove_inner(
        tkey const &key,
        node *&subtree_root_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{
    if (subtree_root_address == nullptr)
    {
        throw std::invalid_argument("Bad argument to delete");
    }

    if (_tree->_comparator(key, subtree_root_address->key) == 0)
    {
        tvalue removing_value = subtree_root_address->value;

        if (subtree_root_address->left_subtree == nullptr &&
            subtree_root_address->right_subtree == nullptr)
        {
            get_info_deleted_node(subtree_root_address, path_to_subtree_root_exclusive);

            if (!path_to_subtree_root_exclusive.empty())
            {
                if ((*path_to_subtree_root_exclusive.top())->right_subtree == subtree_root_address)
                {
                    (*path_to_subtree_root_exclusive.top())->right_subtree = nullptr;
                }
                else
                {
                    (*path_to_subtree_root_exclusive.top())->left_subtree = nullptr;
                }
            }

            subtree_root_address->~node();
            guard_deallocate(subtree_root_address);
            subtree_root_address = nullptr;

            if (!path_to_subtree_root_exclusive.empty())
            {
                node **temp = path_to_subtree_root_exclusive.top();
                path_to_subtree_root_exclusive.pop();
                after_remove_inner(key, *temp, path_to_subtree_root_exclusive);
            }
            else
            {
                after_remove_inner(key, _tree->_root, path_to_subtree_root_exclusive);
            }
        }
        else
        {
            if ((subtree_root_address->left_subtree == nullptr &&
                 subtree_root_address->right_subtree != nullptr) ||
                (subtree_root_address->left_subtree != nullptr &&
                 subtree_root_address->right_subtree == nullptr))
            {
                get_info_deleted_node(subtree_root_address, path_to_subtree_root_exclusive);
                node *node_replacement = subtree_root_address->left_subtree == nullptr ? subtree_root_address->right_subtree : subtree_root_address->left_subtree;

                if (!path_to_subtree_root_exclusive.empty())
                {
                    if ((*path_to_subtree_root_exclusive.top())->right_subtree == subtree_root_address)
                    {
                        (*path_to_subtree_root_exclusive.top())->right_subtree = node_replacement;
                    }
                    else
                    {
                        (*path_to_subtree_root_exclusive.top())->left_subtree = node_replacement;
                    }
                }

                subtree_root_address->~node();
                guard_deallocate(subtree_root_address);
                subtree_root_address = node_replacement;

                if (!path_to_subtree_root_exclusive.empty())
                {
                    node **temp = path_to_subtree_root_exclusive.top();
                    path_to_subtree_root_exclusive.pop();
                    after_remove_inner(key, *temp, path_to_subtree_root_exclusive);
                }
                else
                {
                    after_remove_inner(key, _tree->_root, path_to_subtree_root_exclusive);
                }
            }
            else
            {
                if (subtree_root_address->left_subtree != nullptr &&
                    subtree_root_address->right_subtree != nullptr) {

                    node *max_node = subtree_root_address->left_subtree;

                    while (max_node->right_subtree != nullptr)
                    {
                        max_node = max_node->right_subtree;
                    }

                    tkey key_current = max_node->key;
                    tvalue value_current = max_node->value;

                    _tree->remove(key_current);

                    subtree_root_address->key = std::move(key_current);
                    subtree_root_address->value = std::move(value_current);
                }
            }
        }

        return removing_value;
    }

    binary_search_tree::node *next_node;

    if (_tree->_comparator(key, subtree_root_address->key) > 0) {
        next_node = subtree_root_address->right_subtree;
    } else {
        next_node = subtree_root_address->left_subtree;
    }


    path_to_subtree_root_exclusive.push(&subtree_root_address);
    tvalue removing_value = remove_inner(key, next_node, path_to_subtree_root_exclusive);

    return removing_value;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method::after_remove_inner(
        tkey const &key,
        node *&subtree_root_address,
        std::stack<binary_search_tree::node**> &path_to_subtree_root_exclusive)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
memory *binary_search_tree<tkey, tvalue, tkey_comparer>::get_memory() const
{
    return _allocator;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
logger *binary_search_tree<tkey, tvalue, tkey_comparer>::get_logger() const
{
    return _logger;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::insert(
        tkey const &key,
        tvalue value)
{
    return _insertion->insert(key, std::move(value));
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::update(
        tkey const &key,
        tvalue value)
{
    return _insertion->insert(key, std::move(value));
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
std::vector<tvalue> binary_search_tree<tkey, tvalue, tkey_comparer>::get_range(
        tkey const &lower_bound,
        tkey const &upper_bound) const
{
    std::vector<tvalue> result;

    for (auto it = begin_infix(); it != end_infix(); ++it)
    {
        if (_comparator(lower_bound, std::get<1>(*it)) <= 0 &&
            _comparator(upper_bound, std::get<1>(*it)) >= 0)
        {
            result.push_back(std::get<2>(*it));
        }
    }

    return result;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
bool binary_search_tree<tkey, tvalue, tkey_comparer>::find(
        typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value)
{
    return _find->find(target_key_and_result_value, _root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
tvalue binary_search_tree<tkey, tvalue, tkey_comparer>::remove(
        tkey const &key)
{
    if (_root == nullptr)
    {
        throw std::invalid_argument("Tree is empty");
    }

    return _removing->remove(key, _root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::left_rotate(
        node **subtree_root,
        node **parent) const
{
    node *right_subtree_address = (*subtree_root)->right_subtree;
    (*subtree_root)->right_subtree = right_subtree_address->left_subtree;
    right_subtree_address->left_subtree = *subtree_root;

    if (parent != nullptr)
    {
        if ((*parent)->left_subtree == *subtree_root)
        {
            (*parent)->left_subtree = right_subtree_address;
        }
        else
        {
            (*parent)->right_subtree = right_subtree_address;
        }
    }

    *subtree_root = right_subtree_address;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::right_rotate(
        node **subtree_root,
        node **parent) const
{
    node *left_subtree_address = (*subtree_root)->left_subtree;
    (*subtree_root)->left_subtree = left_subtree_address->right_subtree;
    left_subtree_address->right_subtree = *subtree_root;

    if (parent != nullptr)
    {
        if ((*parent)->left_subtree == *subtree_root)
        {
            (*parent)->left_subtree = left_subtree_address;
        }
        else
        {
            (*parent)->right_subtree = left_subtree_address;
        }
    }

    *subtree_root = left_subtree_address;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_prefix() const noexcept
{
    return prefix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), _root);
}


template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::prefix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_prefix() const noexcept
{
    return prefix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_infix() const noexcept
{
    node *curr_node = _root;

    if (curr_node != nullptr)
    {
        while (curr_node->left_subtree != nullptr)
        {
            curr_node = curr_node->left_subtree;
        }
    }

    return infix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), curr_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::infix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_infix() const noexcept
{
    return infix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::begin_postfix() const noexcept
{
    node *curr_node = _root;

    if (_root == nullptr)
    {
        return end_postfix();
    }

    if (_root->left_subtree != nullptr)
    {
        while (curr_node->left_subtree != nullptr)
        {
            curr_node = curr_node->left_subtree;
        }
    }
    else
    {
        while ((curr_node->left_subtree == nullptr) &&
               !(curr_node->left_subtree == nullptr && curr_node->right_subtree == nullptr))
        {
            curr_node = curr_node->right_subtree;
        }

        while (curr_node->left_subtree != nullptr)
        {
            curr_node = curr_node->left_subtree;
        }
    }

    return postfix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), curr_node);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
typename binary_search_tree<tkey, tvalue, tkey_comparer>::postfix_iterator binary_search_tree<tkey, tvalue, tkey_comparer>::end_postfix() const noexcept
{
    return postfix_iterator(const_cast<binary_search_tree<tkey, tvalue, tkey_comparer>*>(this), nullptr);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        memory *allocator_binary_search_tree,
        logger *logger_tree) :
        _comparator(),
        _root(nullptr),
        _allocator(allocator_binary_search_tree),
        _logger(logger_tree),
        _find(new find_template_method(this)),
        _insertion(new insertion_template_method(this)),
        _removing(new removing_template_method(this))
{
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        find_template_method *find,
        insertion_template_method *insert,
        removing_template_method *remove,
        memory *allocator_binary_search_tree,
        logger *logger_tree,
        node *root) :
        _comparator(),
        _root(root),
        _find(find),
        _insertion(insert),
        _removing(remove),
        _allocator(allocator_binary_search_tree),
        _logger(logger_tree)
{

}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::~binary_search_tree()
{
    delete_tree(_root);

    delete _find;
    delete _insertion;
    delete _removing;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::delete_tree(
        node *&subtree_root_address)
{
    if (subtree_root_address != nullptr)
    {
        delete_tree(subtree_root_address->left_subtree);
        delete_tree(subtree_root_address->right_subtree);
        subtree_root_address->~node();
        guard_deallocate(subtree_root_address);
        subtree_root_address = nullptr;
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
void binary_search_tree<tkey, tvalue, tkey_comparer>::copy_subtree(
        node *src)
{
    if (src != nullptr)
    {
        tvalue value_copy = src->value;

        _insertion->insert(src->key, std::move(value_copy));
        copy_subtree(src->left_subtree);
        copy_subtree(src->right_subtree);
    }
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy) :
        _comparator(target_copy._comparator),
        _root(nullptr),
        _allocator(target_copy._allocator),
        _logger(target_copy._logger),
        _find(target_copy._find),
        _insertion(target_copy._insertion),
        _removing(target_copy._removing)
{
    copy_subtree(target_copy._root);
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>& binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        const binary_search_tree<tkey, tvalue, tkey_comparer> &target_copy)
{
    _comparator = target_copy._comparator;
    _root = nullptr;
    _allocator = target_copy._allocator;
    _logger = target_copy._logger;
    _find = target_copy._find;
    _insertion = target_copy._insertion;
    _removing = target_copy._removing;

    copy_subtree(target_copy._root);

    return *this;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>::binary_search_tree(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept :
        _root(std::move(target_copy._root)),
        _comparator(std::move(target_copy._comparator)),
        _find(target_copy._find),
        _insertion(target_copy._insertion),
        _removing(target_copy._removing)
{
    target_copy._find = nullptr;
    target_copy._insertion = nullptr;
    target_copy._removing = nullptr;
}

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
binary_search_tree<tkey, tvalue, tkey_comparer>& binary_search_tree<tkey, tvalue, tkey_comparer>::operator=(
        binary_search_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
{
    _root = std::move(target_copy._root);
    _comparator = std::move(target_copy._comparator);
    _find = target_copy._find;
    _insertion = target_copy._insertion;
    _removing = target_copy._removing;

    target_copy._find = nullptr;
    target_copy._insertion = nullptr;
    target_copy._removing = nullptr;

    return *this;
}



#endif