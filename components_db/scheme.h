#ifndef SCHEME_H
#define SCHEME_H

#include <iostream>
#include "collection.h"

using type_scheme = associative_container<std::string, std::pair<type_collection*, std::map<std::string, type_order_collection*>>*>;
using type_scheme_avl = avl_tree<std::string, std::pair<type_collection_avl*, std::map<std::string, type_order_collection*>>*, compare_str_keys>;
using type_scheme_rb = red_black_tree<std::string, std::pair<type_collection_rb*, std::map<std::string, type_order_collection*>>*, compare_str_keys>;
using type_scheme_splay = splay_tree<std::string, std::pair<type_collection_splay*, std::map<std::string, type_order_collection*>>*, compare_str_keys>;

#endif


