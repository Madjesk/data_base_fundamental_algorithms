#ifndef COLLECTION_H
#define COLLECTION_H

#include <iostream>
#include "../rb_tree/rb_tree.h"
#include "../avl/avl_tree.h"
#include "../splay/splay_tree.h"
#include "../meeting_data/meeting_data.h"
#include "../my_comparators/my_comparators.h"
#include <map>
#include <memory>

using type_value = std::pair<meeting_data*, std::map<long long, std::vector<std::pair<std::string, std::string>>>>;
using type_order_collection = avl_tree<std::string, type_value*, compare_str_keys>;

using type_collection = associative_container<size_t, type_value*>;
using type_collection_avl = avl_tree<size_t, type_value*, compare_data_keys>;
using type_collection_rb = red_black_tree<size_t, type_value*, compare_data_keys>;
using type_collection_splay = splay_tree<size_t, type_value*, compare_data_keys>;
using type_states = std::map<long long, std::vector<std::pair<std::string, std::string>>>;

#endif