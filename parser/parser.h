#ifndef PARSER_H
#define PARSER_H

#include "../components_db/collection.h"
#include "../components_db/scheme.h"
#include "../components_db/pool.h"
#include "../components_db/db.h"
#include "../json_builder/json_builder.h"
#include "../json_builder_concrete/json_builder_concrete.h"

class parser
{
public:
    enum class TREE
    {
        RED_BLACK_TREE,
        SPLAY_TREE,
        AVL_TREE
    };

public:
    class command
    {
    public:
        virtual void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme *, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) = 0;
        virtual ~command() = default;
    };

private:
    static type_data_base *_data_base;
    static logger *_logger;

protected:
    parser *_next_parser;

public:

    static std::string delete_spaces(const std::string &str);

    static std::vector<std::string> split_by_spaces(const std::string &str);

    parser();

    void set_next(parser *next_parser);

    void accept_request(const std::string &request);

    virtual void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) = 0;

    static type_data_base *get_instance();

    static logger *get_logger();

    static type_data_base *allocate_data_base(TREE tree = TREE::AVL_TREE);

    static type_pool *allocate_pool(TREE tree = TREE::AVL_TREE);

    static type_scheme *allocate_scheme(TREE tree = TREE::AVL_TREE);

    static type_collection *allocate_collection(memory *memory_init, TREE tree = TREE::AVL_TREE);

    virtual ~parser();
};

#endif