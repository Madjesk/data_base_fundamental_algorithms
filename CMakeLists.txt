cmake_minimum_required(VERSION 3.24)
project(KP)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-w)

include_directories(.)
include_directories(components_db)
include_directories(avl)
include_directories(allocator_list)
include_directories(allocator_descriptor)
include_directories(allocator_buddies)
include_directories(check_data)
include_directories(my_comparators)
include_directories(meeting_data)
include_directories(flyweight)
include_directories(handler_commands_concrete)
include_directories(logger)
include_directories(logger_guard)
include_directories(memory)
include_directories(memory_guard)
include_directories(parser_commands)
include_directories(rb_tree)
include_directories(splay)
include_directories(parser)
include_directories(parser_concrete)
include_directories(json)

add_executable(KP
        components_db/collection.h
        components_db/scheme.h
        components_db/pool.h
        components_db/db.h
        avl/avl_tree.h
        my_comparators/my_comparators.h
        meeting_data/meeting_data.cpp
        meeting_data/meeting_data.h
        flyweight/flyweight_for_string.cpp
        flyweight/flyweight_for_string.h
        logger/date.h
        logger/logger.cpp
        logger/logger.h
        logger/logger_builder.cpp
        logger/logger_builder.h
        logger/logger_builder_concrete.cpp
        logger/logger_builder_concrete.h
        logger/logger_concrete.cpp
        logger/logger_concrete.h
        logger_guard/logger_guard.cpp
        logger_guard/logger_guard.h
        memory/memory.cpp
        memory/memory.h
        allocator_list/allocator_list.cpp
        allocator_list/allocator_list.h
        memory_guard/memory_guard.cpp
        memory_guard/memory_guard.h
        rb_tree/rb_tree.h
        splay/splay_tree.h
        associative_container.h
        binary_search_tree.h
        main.cpp parser/parser.cpp parser/parser.h parser_concrete/parser_concrete.h json_builder/json_builder.cpp json_builder_concrete/json_builder_concrete.cpp allocator_descriptor/allocator_descriptor.cpp allocator_descriptor/allocator_descriptor.h allocator_buddies/allocator_buddies.cpp allocator_buddies/allocator_buddies.h)
