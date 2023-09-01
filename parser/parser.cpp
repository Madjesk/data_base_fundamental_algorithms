#include "parser.h"

type_data_base *parser::_data_base = allocate_data_base(parser::TREE::AVL_TREE);
logger *parser::_logger = nullptr;

type_data_base *parser::allocate_data_base(TREE tree)
{
    type_data_base* data_base = nullptr;

    if (tree == TREE::RED_BLACK_TREE) {
        data_base = reinterpret_cast<type_data_base*>(new type_data_base_rb);
    } else if (tree == TREE::AVL_TREE) {
        data_base = reinterpret_cast<type_data_base*>(new type_data_base_avl);
    } else if (tree == TREE::SPLAY_TREE) {
        data_base = reinterpret_cast<type_data_base*>(new type_data_base_splay);
    } else {
        throw std::invalid_argument("Wrong number tree");
    }

    return data_base;
}

type_pool* parser::allocate_pool(TREE tree)
{
    type_pool* pool = nullptr;

    if (tree == TREE::RED_BLACK_TREE) {
        pool = reinterpret_cast<type_pool*>(new type_pool_rb);
    } else if (tree == TREE::AVL_TREE) {
        pool = reinterpret_cast<type_pool*>(new type_pool_avl);
    } else if (tree == TREE::SPLAY_TREE) {
        pool = reinterpret_cast<type_pool*>(new type_pool_splay);
    } else {
        throw std::invalid_argument("Wrong number of type tree");
    }

    return pool;
}


type_scheme* parser::allocate_scheme(TREE tree)
{
    type_scheme* scheme = nullptr;

    if (tree == TREE::RED_BLACK_TREE) {
        scheme = reinterpret_cast<type_scheme*>(new type_scheme_rb);
    } else if (tree == TREE::AVL_TREE) {
        scheme = reinterpret_cast<type_scheme*>(new type_scheme_avl);
    } else if (tree == TREE::SPLAY_TREE) {
        scheme = reinterpret_cast<type_scheme*>(new type_scheme_splay);
    } else {
        throw std::invalid_argument("ERROR: Invalid number of type tree");
    }

    return scheme;
}

type_collection* parser::allocate_collection(memory* memory_init, TREE tree)
{
    type_collection* collection = nullptr;

    if (tree == TREE::RED_BLACK_TREE) {
        collection = reinterpret_cast<type_collection*>(new type_collection_rb(memory_init));
    } else if (tree == TREE::AVL_TREE) {
        collection = reinterpret_cast<type_collection*>(new type_collection_avl(memory_init));
    } else if (tree == TREE::SPLAY_TREE) {
        collection = reinterpret_cast<type_collection*>(new type_collection_splay(memory_init));
    } else {
        throw std::invalid_argument("Wrong number of tree");
    }

    return collection;
}


parser::parser() : _next_parser(nullptr)
{
    json_builder *builder = new json_builder_concrete();
    _logger = builder->build("json/config.json");
}

parser::~parser()
{
    delete _next_parser;
}

void parser::set_next(
        parser *next_parser)
{
    _next_parser = next_parser;
}

void parser::accept_request(
        const std::string &request)
{
    std::vector<std::string> params = split_by_spaces(delete_spaces(request));

    try
    {
        parser_request(params, std::cin, true);
    }
    catch (const std::exception &ex)
    {
        throw ex;
    }
}

type_data_base *parser::get_instance()
{
    return _data_base;
}

logger *parser::get_logger()
{
    return _logger;
}

std::string parser::delete_spaces(
        const std::string &str)
{
    std::string copy_str = str;

    size_t str_begin = copy_str.find_first_not_of(' ');
    size_t str_end = copy_str.find_last_not_of(' ');
    copy_str.erase(str_end + 1, copy_str.size() - str_end);
    copy_str.erase(0, str_begin);

    return copy_str;
}

std::vector<std::string> parser::split_by_spaces(
        const std::string &str)
{
    std::vector<std::string> result;
    std::stringstream stream(str);
    std::string word;

    while (stream >> word)
    {
        result.push_back(word);
    }

    return result;
}
