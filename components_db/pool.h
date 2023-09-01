#ifndef POOL_H
#define POOL_H

#include <iostream>
#include "scheme.h"

using type_pool = associative_container<std::string, std::pair<type_scheme*, memory*>>;
using type_pool_avl = avl_tree<std::string, std::pair<type_scheme_avl*, memory*>, compare_str_keys>;
using type_pool_rb = red_black_tree<std::string, std::pair<type_scheme_avl*, memory*>, compare_str_keys>;
using type_pool_splay = splay_tree<std::string, std::pair<type_scheme_avl*, memory*>, compare_str_keys>;

#endif