#ifndef SPLAY_TREE_H
#define SPLAY_TREE_H

#include "../binary_search_tree.h"

template<
        typename tkey,
        typename tvalue,
        typename tkey_comparer>
class splay_tree : public binary_search_tree<tkey, tvalue, tkey_comparer>
{
private:
    class splay_find_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method
    {
    private:
        splay_tree<tkey, tvalue, tkey_comparer> *_tree;
    protected:
        void after_find_inner(
                typename associative_container<tkey, tvalue>::key_value_pair *target_key_and_result_value,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {
            _tree->splay(subtree_address, path_to_subtree_root_exclusive);
        }
    public:
        explicit splay_find_template_method(
                splay_tree<tkey, tvalue, tkey_comparer> *tree) :
                _tree(tree),
                binary_search_tree<tkey, tvalue, tkey_comparer>::find_template_method(tree)
        {

        }
    };

private:

    class splay_insertion_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method
    {
    private:

        size_t get_node_size() const override
        {
            return sizeof(typename binary_search_tree<tkey, tvalue, tkey_comparer>::node);
        }

        splay_tree<tkey, tvalue, tkey_comparer> *_tree;

    protected:
        void after_insert_inner(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {
            _tree->splay(subtree_root_address, path_to_subtree_root_exclusive);
        }

    public:
        explicit splay_insertion_template_method(splay_tree<tkey, tvalue, tkey_comparer> *tree) :
                _tree(tree),
                binary_search_tree<tkey, tvalue, tkey_comparer>::insertion_template_method(tree)
        {

        }

    };

private:
    class splay_removing_template_method : public binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method
    {
    private:
        splay_tree<tkey, tvalue, tkey_comparer> *_tree;

        tvalue remove_inner(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {
            if (subtree_root_address == nullptr)
            {
                throw std::invalid_argument("Invalid key to remove");
            }

            if (_tree->_comparator(key, subtree_root_address->key) == 0)
            {
                tvalue removing_value = subtree_root_address->value;

                _tree->splay(subtree_root_address, path_to_subtree_root_exclusive);

                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *left_subtree = subtree_root_address->left_subtree;
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *right_subtree = subtree_root_address->right_subtree;

                subtree_root_address->~node();
                _tree->guard_deallocate(subtree_root_address);
                subtree_root_address = nullptr;

                _tree->_root = _tree->merge(left_subtree, right_subtree);

                return removing_value;
            }

            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *next_node = _tree->_comparator(key, subtree_root_address->key) > 0 ? subtree_root_address->right_subtree : subtree_root_address->left_subtree;
            path_to_subtree_root_exclusive.push(&subtree_root_address);
            tvalue removing_value = remove_inner(key, next_node, path_to_subtree_root_exclusive);

            return removing_value;
        }

    protected:
        void after_remove_inner(
                tkey const &key,
                typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_root_address,
                std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive) override
        {

        }

    public:
        explicit splay_removing_template_method(
                splay_tree<tkey, tvalue, tkey_comparer> *tree) :
                _tree(tree),
                binary_search_tree<tkey, tvalue, tkey_comparer>::removing_template_method(tree)
        {

        }

    };

public:
    explicit splay_tree(
            memory *allocator_binary_search_tree = nullptr,
            logger *logger_tree = nullptr) :
            binary_search_tree<tkey, tvalue, tkey_comparer>(
                    new splay_find_template_method(this),
                    new splay_insertion_template_method(this),
                    new splay_removing_template_method(this),
                    allocator_binary_search_tree,
                    logger_tree,
                    nullptr)
    {

    }

    ~splay_tree() final = default;

private:
    void splay(
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&subtree_address,
            std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> &path_to_subtree_root_exclusive)
    {
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **parent = nullptr;
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **grand_parent = nullptr;
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **great_grand_parent = nullptr;
        typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *current_node = subtree_address;

        while (!path_to_subtree_root_exclusive.empty())
        {
            parent = path_to_subtree_root_exclusive.top();
            path_to_subtree_root_exclusive.pop();

            if (!path_to_subtree_root_exclusive.empty())
            {
                grand_parent = path_to_subtree_root_exclusive.top();
                path_to_subtree_root_exclusive.pop();

                if (!path_to_subtree_root_exclusive.empty())
                {
                    great_grand_parent = path_to_subtree_root_exclusive.top();
                    path_to_subtree_root_exclusive.pop();
                }
            }

            if ((*parent)->left_subtree == current_node)
            {
                if (grand_parent == nullptr)
                {
                    this->right_rotate(parent, grand_parent);
                    current_node = *parent;
                }
                else
                {
                    if ((*grand_parent)->right_subtree == *parent)
                    {
                        this->right_rotate(parent, grand_parent);
                        this->left_rotate(grand_parent, great_grand_parent);
                        current_node = *grand_parent;
                    }
                    else
                    {
                        this->right_rotate(grand_parent, great_grand_parent);
                        this->right_rotate(grand_parent, great_grand_parent);
                        current_node = *grand_parent;
                    }
                }
            }
            else
            {
                if (grand_parent == nullptr)
                {
                    this->left_rotate(parent, grand_parent);
                    current_node = *parent;
                }
                else
                {
                    if ((*grand_parent)->left_subtree == *parent)
                    {
                        this->left_rotate(parent, grand_parent);
                        this->right_rotate(grand_parent, great_grand_parent);
                        current_node = *grand_parent;
                    }
                    else
                    {
                        this->left_rotate(grand_parent, great_grand_parent);
                        this->left_rotate(grand_parent, great_grand_parent);
                        current_node = *grand_parent;
                    }
                }
            }

            if (great_grand_parent != nullptr)
            {
                path_to_subtree_root_exclusive.push(great_grand_parent);
            }

            parent = nullptr;
            grand_parent = nullptr;
            great_grand_parent = nullptr;
        }
    }

    typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *merge(
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&tree_first,
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *&tree_second)
    {
        if (tree_first == nullptr && tree_second != nullptr)
        {
            return tree_second;
        }

        if (tree_first != nullptr && tree_second == nullptr)
        {
            return tree_first;
        }

        if (tree_first != nullptr && tree_second != nullptr)
        {
            std::stack<typename binary_search_tree<tkey, tvalue, tkey_comparer>::node**> path;
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node *max_node = tree_first;
            typename binary_search_tree<tkey, tvalue, tkey_comparer>::node **parent = &tree_first;

            while (max_node->right_subtree != nullptr)
            {
                path.push(parent);
                parent = &(max_node->right_subtree);
                max_node = max_node->right_subtree;
            }

            splay(max_node, path);

            tree_first->right_subtree = tree_second;

            return tree_first;
        }

        return nullptr;
    }
};

#endif