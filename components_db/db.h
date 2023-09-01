#ifndef DB_H
#define DB_H

#include <iostream>
#include "pool.h"

using type_data_base = associative_container<std::string, std::pair<type_pool*, memory*>>;
using type_data_base_avl = avl_tree<std::string, std::pair<type_pool_avl*, memory*>, compare_str_keys>;
using type_data_base_rb = red_black_tree<std::string, std::pair<type_pool_avl*, memory*>, compare_str_keys>;
using type_data_base_splay = splay_tree<std::string,std::pair<type_pool_avl*, memory*>, compare_str_keys>;

#endif