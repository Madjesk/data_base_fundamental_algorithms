#ifndef AVL_TREE_H
#define AVL_TREE_H
#include "../binary_search_tree.h"
#include <iostream>

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class avl_tree : public binary_search_tree<tkey, tvalue, tkey_comparer>
{
protected:
    struct avl_node : binary_search_tree<tkey, tvalue, tkey_comparer>::node
    {
        size_t node_height = 0;
    };

protected:

    class avl_insertion_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
    private:

        size_t get_node_size() const override
        {
            return sizeof(avl_node);
        }
        avl_tree<tkey, tvalue, tkey_comparer> *_tree;

    protected:

        void after_insert_inner(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *curr_node = subtree_root_address;

            while (!path_to_subtree_root_exclusive.empty())
            {
                _tree->balance(curr_node, path_to_subtree_root_exclusive);
                curr_node = *path_to_subtree_root_exclusive.top();
                path_to_subtree_root_exclusive.pop();
            }
        }

    public:

        explicit avl_insertion_template_method(
                avl_tree<tkey, tvalue, tkey_comparer> *tree) :
                _tree(tree),
                binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(tree)
        {

        }

    };

    class avl_removing_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {

    private:

        avl_tree<tkey, tvalue, tkey_comparer> *_tree;

    protected:

        void after_remove_inner(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *current_node = subtree_root_address;

            while (!path_to_subtree_root_exclusive.empty())
            {
                _tree->balance(current_node, path_to_subtree_root_exclusive);
                current_node = *path_to_subtree_root_exclusive.top();
                path_to_subtree_root_exclusive.pop();
            }
        }

    public:

        explicit avl_removing_template_method(avl_tree<tkey, tvalue, tkey_comparer> *tree) :
                _tree(tree),
                binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method(tree)
        {

        }

    };

public:

    explicit avl_tree(
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr) :
            binary_search_tree<tkey, tvalue, tkey_comparer>(
                    new typename binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method(this),
                    new avl_insertion_template_method(this),
                    new avl_removing_template_method(this),
                    allocator_binary_search_tree,
                    logger_tree,
                    nullptr)
    {

    }

    ~avl_tree() final = default;

private:

    int get_height(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *node)
    {
        if (node == nullptr) {
            return 0;
        } else {
            return reinterpret_cast<avl_node*>(node)->node_height;
        }
    }

    void update_height(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **subtree_address)
    {
        if (subtree_address != nullptr && *subtree_address != nullptr)
        {
            int height_left = 0;
            int height_right = 0;

            if (*subtree_address != nullptr) {
                height_left = get_height((*subtree_address)->left_subtree);
            }

            if (*subtree_address != nullptr) {
                height_right = get_height((*subtree_address)->right_subtree);
            }
            reinterpret_cast<avl_node*>(*subtree_address)->node_height = std::max(height_left, height_right) + 1;
        }
    }

    int get_balance_factor(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *subtree_address) {
        return get_height(subtree_address->left_subtree) - get_height(subtree_address->right_subtree);
    }



public:

    avl_tree(const avl_tree<tkey, tvalue, tkey_comparer> &target_copy) :
            binary_search_tree<tkey, tvalue, tkey_comparer>(
                    this->_find,
                    this->_insertion,
                    this->_removing,
                    this->_allocator,
                    this->_logger,
                    this->_root)
    {

    }

    avl_tree& operator=(const avl_tree<tkey, tvalue, tkey_comparer> &target_copy)
    {
        *this = target_copy;
        return *this;
    }

    avl_tree(avl_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
    {
        *this = std::move(target_copy);
    }

    avl_tree& operator=(avl_tree<tkey, tvalue, tkey_comparer> &&target_copy) noexcept
    {
        *this = std::move(target_copy);
        return *this;
    }

private:

    void balance(
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
            std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive)
    {
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **parent = nullptr;

        if (!path_to_subtree_root_exclusive.empty())
        {
            parent = path_to_subtree_root_exclusive.top();
            path_to_subtree_root_exclusive.pop();
        }

        update_height(&subtree_root_address);
        int bal_factor = get_balance_factor(subtree_root_address);

        if (bal_factor == 2)
        {
            if (get_balance_factor(subtree_root_address->left_subtree) < 0)
            {
                this->left_rotate(&(subtree_root_address->left_subtree), &subtree_root_address);
                update_height(&(subtree_root_address->left_subtree->left_subtree));
                update_height(&(subtree_root_address->left_subtree->right_subtree));
                update_height(&(subtree_root_address->left_subtree));
            }

            this->right_rotate(&subtree_root_address, parent);
            update_height(&(subtree_root_address->right_subtree));
            update_height(&(subtree_root_address->left_subtree));
            update_height(&subtree_root_address);
        }

        if (bal_factor == -2)
        {
            if (get_balance_factor(subtree_root_address->right_subtree) > 0)
            {
                this->right_rotate(&(subtree_root_address->right_subtree), &subtree_root_address);
                update_height(&(subtree_root_address->right_subtree->right_subtree));
                update_height(&(subtree_root_address->right_subtree->left_subtree));
                update_height(&(subtree_root_address->right_subtree));
            }

            this->left_rotate(&subtree_root_address, parent);
            update_height(&(subtree_root_address->right_subtree));
            update_height(&(subtree_root_address->left_subtree));
            update_height(&subtree_root_address);
        }

        if (parent != nullptr)
        {
            path_to_subtree_root_exclusive.push(parent);
        }
    }

};

#endif