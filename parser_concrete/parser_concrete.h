#ifndef PARSER_CONCRETE_H
#define PARSER_CONCRETE_H

#include "../parser/parser.h"
#include <functional>
#include <fstream>
#include "../allocator_list/allocator_list.h"
#include "../allocator_descriptor/allocator_descriptor.h"
#include "../allocator_buddies/allocator_buddies.h"
#include <string>

void read_id_meeting(size_t &id_user,std::istream &stream);
void read_data_meeting(meeting_data *&target,std::istream &stream);
void read_id_meeting_from_console(size_t &id_user);
void read_data_meeting_from_console(meeting_data *&target);
void output_meeting_data(meeting_data *&target);
void read_arguments(const std::string &str, const std::string &name_argument, const std::string &sep, std::string &value_argument);
std::string get_current_datetime();
meeting_data *get_state(const std::string &datetime, type_value *states);
bool is_ves(int year);
long long get_time(const std::string &time);

class parser_run: public parser
{
public:
    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
    {
        if (!params.empty() && params[0] == "READ")
        {
            command *execute_run = new run_command(this);
            try
            {
                execute_run->execute(params, nullptr, nullptr, nullptr, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_run;
                throw std::invalid_argument(ex.what());
            }

            delete execute_run;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class run_command: public command
    {
    private:
        parser *_parser;
    public:
        explicit run_command(parser *parser_init) :
                _parser(parser_init)
        {

        }

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            std::fstream stream_file(params[1]);
            std::string line;

            if (!stream_file.is_open())
            {
                throw std::invalid_argument("Can't open file");
            }

            while (std::getline(stream_file, line))
            {
                std::vector<std::string> params_file_command = split_by_spaces(delete_spaces(line));

                try
                {
                    _parser->parser_request(params_file_command, stream_file, false);
                }
                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }
            }

            stream_file.close();
        }
    };

    ~parser_run() override = default;
};

class parser_add_pool: public parser
{

public:

    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
    {
        if (!params.empty() && params[0] == "CREATE_POOL")
        {
            type_data_base *data_base = parser::get_instance();
            if (params.size() != 2)
            {
                throw std::invalid_argument("Can't add pool!");
            }

            if (data_base->in(params[1]))
            {
                throw std::invalid_argument("Wrong name!");
            }
            command *execute_add_pool = new add_pool_command;
            try
            {
                execute_add_pool->execute(params, nullptr, nullptr, nullptr, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_add_pool;
                throw std::invalid_argument(ex.what());
            }

            delete execute_add_pool;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class add_pool_command: public command {

    public:
        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool *, memory *> *pool,
                std::pair<type_scheme *, memory *> *scheme,
                std::pair<type_collection *, std::map<std::string, type_order_collection *>> *collection,
                std::istream &stream,
                bool console) override {
            type_data_base *data_base = parser::get_instance();

            if (console) {
                int number_allocator = 0;

                std::cout << "What kind of allocator do you want?" << std::endl;
                std::cout << "1. Allocator list" << std::endl;
                std::cout << "2. Allocator descriptor" << std::endl;
                std::cout << "3. Allocator buddies" << std::endl;
                std::cout << "Enter the number: ";
                std::cin >> number_allocator;

                switch (number_allocator) {
                    case 1:
                        data_base->insert(params[1], std::make_pair(parser::allocate_pool(),new allocator_list(66666)));
                        break;
                    case 2:
                        data_base->insert(params[1], std::make_pair(parser::allocate_pool(),new allocator_descriptor(66666)));
                        break;
                    case 3:
                        data_base->insert(params[1],std::make_pair(parser::allocate_pool(), new allocator_buddies(16)));
                        break;
                    default:
                        throw std::invalid_argument("Wrong allocator");
                }
            } else {
                std::string allocator_params;

                if (stream.eof()) {
                    throw std::out_of_range("Can't read allocator name");
                }
                std::getline(stream, allocator_params);
                std::string name_allocator;
                try {
                    read_arguments(allocator_params, "ALLOCATOR", ":", name_allocator);
                }
                catch (const std::exception &ex) {
                    throw std::invalid_argument(ex.what());
                }

                if (name_allocator == "allocator_list") {
                    data_base->insert(params[1],std::make_pair(parser::allocate_pool(), new allocator_list(666666)));
                } else if (name_allocator == "allocator_descriptor") {
                    data_base->insert(params[1], std::make_pair(parser::allocate_pool(),new allocator_descriptor(66666)));
                } else if (name_allocator == "allocator_buddies") {
                    data_base->insert(params[1], std::make_pair(parser::allocate_pool(),new allocator_buddies(16)));
                } else {
                    throw std::invalid_argument("Wrong allocator");
                }
            }
        }
    };

    ~parser_add_pool() override = default;
};

class parser_remove_pool: public parser
{

public:
    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
    {
        if (!params.empty() && params[0] == "DELETE_POOL")
        {
            type_data_base *data_base = parser::get_instance();
            if (params.size() != 2)
            {
                throw std::invalid_argument("Can't remove pool");
            }
            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find that pool");
            }
            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);
            command *execute_remove_pool = new remove_pool_command();
            try
            {
                execute_remove_pool->execute(params, &pool, nullptr, nullptr, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_remove_pool;
                throw std::invalid_argument(ex.what());
            }

            delete execute_remove_pool;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class remove_pool_command: public command
    {
    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            auto end_pool = reinterpret_cast<type_pool_avl*>(pool->first)->end_prefix();
            auto begin_pool = reinterpret_cast<type_pool_avl*>(pool->first)->begin_prefix();

            for (auto it_pool = begin_pool; it_pool != end_pool; ++it_pool)
            {
                auto end_scheme = std::get<2>(*it_pool).first->end_prefix();
                auto begin_scheme = std::get<2>(*it_pool).first->begin_prefix();

                for (auto it_scheme = begin_scheme; it_scheme != end_scheme; ++it_scheme)
                {
                    auto end_collection = std::get<2>(*it_scheme)->first->end_prefix();
                    auto begin_collection = std::get<2>(*it_scheme)->first->begin_prefix();

                    for (auto it_collection = begin_collection; it_collection != end_collection; ++it_collection)
                    {
                        delete std::get<2>(*it_collection)->first;
                        delete std::get<2>(*it_collection);
                    }

                    delete std::get<2>(*it_scheme)->first;
                    delete std::get<2>(*it_scheme)->second["TYPE"];
                    delete std::get<2>(*it_scheme)->second["FORMAT"];
                    delete std::get<2>(*it_scheme)->second["DESCRIPTION"];
                    delete std::get<2>(*it_scheme)->second["LINK"];
                    delete std::get<2>(*it_scheme)->second["SURNAME"];
                    delete std::get<2>(*it_scheme)->second["NAME"];
                    delete std::get<2>(*it_scheme)->second["PATRONYMIC"];
                    delete std::get<2>(*it_scheme)->second["DATE"];
                    delete std::get<2>(*it_scheme)->second["START_TIME"];
                    delete std::get<2>(*it_scheme)->second["DURATION"];
                    delete std::get<2>(*it_scheme)->second["MEMBERS"];
                    delete std::get<2>(*it_scheme);
                    std::get<2>(*it_pool).first->remove(std::get<1>(*it_scheme));
                }

                pool->first->remove(std::get<1>(*it_pool));
                delete std::get<2>(*it_pool).first;
            }

            delete pool->first;
            delete pool->second;
            parser::get_instance()->remove(params[1]);

            pool->first = nullptr;
            pool->second = nullptr;
        }
    };

    ~parser_remove_pool() override = default;
};

class parser_add_scheme: public parser
{

public:
    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
    {
        if (!params.empty() && params[0] == "CREATE_SCHEME")
        {
            type_data_base *data_base = parser::get_instance();
            if (params.size() != 3)
            {
                throw std::invalid_argument("Can't add to scheme");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find that pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (pool.first->in(params[2]))
            {
                throw std::invalid_argument("Wrong scheme name");
            }

            command *execute_add_scheme = new add_scheme_command;

            try
            {
                execute_add_scheme->execute(params, &pool, nullptr, nullptr, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_add_scheme;
                throw std::invalid_argument(ex.what());
            }

            delete execute_add_scheme;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class add_scheme_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            (*pool).first->insert(params[2], std::make_pair(parser::allocate_scheme(), (*pool).second));
        }
    };

    ~parser_add_scheme() override = default;
};

class parser_remove_scheme: public parser
{

public:

    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
            {
                if (!params.empty() && params[0] == "DELETE_SCHEME")
                {
                    type_data_base *data_base = parser::get_instance();

                    if (params.size() != 3)
                    {
                        throw std::invalid_argument("Can't remove scheme");
                    }

                    if (!data_base->in(params[1]))
                    {
                        throw std::invalid_argument("Can't find pool");
                    }

                    std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

                    if (!pool.first->in(params[2]))
                    {
                        throw std::invalid_argument("Can't find scheme");
                    }

                    std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

                    command *execute_remove_scheme = new remove_scheme_command;

                    try
                    {
                        execute_remove_scheme->execute(params, &pool, &scheme, nullptr, stream, console);
                    }
                    catch (const std::exception &ex)
                    {
                        delete execute_remove_scheme;
                        throw std::invalid_argument(ex.what());
                    }

                    delete execute_remove_scheme;
                }
                else
                {
                    if (_next_parser != nullptr)
                    {
                        _next_parser->parser_request(params, stream, console);
                    }
                }
            }

    class remove_scheme_command: public command
    {
    public:
        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            auto end_scheme = reinterpret_cast<type_scheme_avl*>(scheme->first)->end_prefix();
            auto begin_scheme = reinterpret_cast<type_scheme_avl*>(scheme->first)->begin_prefix();

            for (auto it_scheme = begin_scheme; it_scheme != end_scheme; ++it_scheme)
            {
                auto end_collection = std::get<2>(*it_scheme)->first->end_prefix();
                auto begin_collection = std::get<2>(*it_scheme)->first->begin_prefix();

                for (auto it_collection = begin_collection; it_collection != end_collection; ++it_collection)
                {
                    delete std::get<2>(*it_collection)->first;
                    delete std::get<2>(*it_collection);
                }

                delete std::get<2>(*it_scheme)->first;
                delete std::get<2>(*it_scheme)->second["TYPE"];
                delete std::get<2>(*it_scheme)->second["FORMAT"];
                delete std::get<2>(*it_scheme)->second["DESCRIPTION"];
                delete std::get<2>(*it_scheme)->second["LINK"];
                delete std::get<2>(*it_scheme)->second["SURNAME"];
                delete std::get<2>(*it_scheme)->second["NAME"];
                delete std::get<2>(*it_scheme)->second["PATRONYMIC"];
                delete std::get<2>(*it_scheme)->second["DATE"];
                delete std::get<2>(*it_scheme)->second["START_TIME"];
                delete std::get<2>(*it_scheme)->second["DURATION"];
                delete std::get<2>(*it_scheme)->second["MEMBERS"];
                delete std::get<2>(*it_scheme);
                scheme->first->remove(std::get<1>(*it_scheme));
            }

            delete scheme->first;
            pool->first->remove(params[2]);

            scheme->first = nullptr;
        }
    };

    ~parser_remove_scheme() override = default;
};

class parser_add_collection: public parser
{

public:

    void parser_request(const std::vector<std::string> &params, std::istream &stream, bool console) override
    {
        if (!params.empty() && params[0] == "CREATE_COLLECTION")
        {

            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't add collection");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Wrong collection name");
            }

            command *execute_add_collection = new add_collection_command;

            try
            {
                execute_add_collection->execute(params, &pool, &scheme, nullptr, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_add_collection;
                throw std::invalid_argument(ex.what());
            }

            delete execute_add_collection;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class add_collection_command: public command
    {
    public:
        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            type_data_base *data_base = parser::get_instance();

            std::map<std::string, type_order_collection*> order_collections;
            order_collections.insert({"TYPE", new type_order_collection()});
            order_collections.insert({"FORMAT", new type_order_collection()});
            order_collections.insert({"DESCRIPTION", new type_order_collection()});
            order_collections.insert({"LINK", new type_order_collection()});
            order_collections.insert({"SURNAME", new type_order_collection()});
            order_collections.insert({"NAME", new type_order_collection()});
            order_collections.insert({"PATRONYMIC", new type_order_collection()});
            order_collections.insert({"DATE", new type_order_collection()});
            order_collections.insert({"START_TIME", new type_order_collection()});
            order_collections.insert({"DURATION", new type_order_collection()});
            order_collections.insert({"MEMBERS", new type_order_collection()});

            if (console)
            {
                int number_tree = 0;

                std::cout << "Choose an type of tree for collection:" << std::endl;
                std::cout << "1. Red-black tree" << std::endl;
                std::cout << "2. Splay tree" << std::endl;
                std::cout << "3. AVL tree" << std::endl;
                std::cout << "Put the number: ";
                std::cin >> number_tree;

                try
                {
                    scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(parser::allocate_collection(pool->second, static_cast<parser::TREE>(number_tree - 1)), order_collections)));
                }
                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }
            }
            else
            {
                std::string tree_params;

                if (stream.eof())
                {
                    throw std::out_of_range("Can't read type tree");
                }

                std::getline(stream, tree_params);
                std::string name_type;

                try
                {
                    read_arguments(tree_params, "TREE", ":", name_type);
                }
                catch (const std::exception &ex)
                {
                    throw std::invalid_argument(ex.what());
                }

                if (name_type == "RED_BLACK_TREE")
                {
                    scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(parser::allocate_collection(pool->second, parser::TREE::RED_BLACK_TREE), order_collections)));
                }
                else if (name_type == "AVL_TREE")
                {
                    scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(parser::allocate_collection(pool->second, parser::TREE::AVL_TREE), order_collections)));
                }
                else if (name_type == "SPLAY_TREE")
                {
                    scheme->first->insert(params[3], new std::pair<type_collection*, std::map<std::string, type_order_collection*>>(std::make_pair(parser::allocate_collection(pool->second, parser::TREE::SPLAY_TREE), order_collections)));
                }
                else
                {
                    throw std::invalid_argument("WRONG name tree");
                }
            }
        }
    };

    ~parser_add_collection() override = default;
};

class parser_remove_collection: public parser
{

public:

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "DELETE_COLLECTION")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't remove collection");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't remove pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_remove_collection = new remove_collection_command;

            try
            {
                execute_remove_collection->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_remove_collection;
                throw std::invalid_argument(ex.what());
            }

            delete execute_remove_collection;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class remove_collection_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            auto end_collection = reinterpret_cast<type_collection_avl*>(scheme->first->get(params[3])->first)->end_prefix();
            auto begin_collection = reinterpret_cast<type_collection_avl*>((*scheme).first->get(params[3])->first)->begin_prefix();

            for (auto it_collection = begin_collection; it_collection != end_collection; ++it_collection)
            {
                delete std::get<2>(*it_collection)->first;
                delete std::get<2>(*it_collection);
            }

            delete collection->first;

            delete collection->second["TYPE"];
            delete collection->second["FORMAT"];
            delete collection->second["DESCRIPTION"];
            delete collection->second["LINK"];
            delete collection->second["SURNAME"];
            delete collection->second["PATRONYMIC"];
            delete collection->second["DATE"];
            delete collection->second["START_TIME"];
            delete collection->second["DURATION"];
            delete collection->second["MEMBERS"];
            delete collection;

            scheme->first->remove(params[3]);

            collection = nullptr;
        }
    };

    ~parser_remove_collection() override = default;
};

class parser_add: public parser
{

public:

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "INSERT_DATA")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't insert data");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_add = new add_command;

            try
            {
                execute_add->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_add;
                throw std::invalid_argument(ex.what());
            }

            delete execute_add;
        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class add_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            size_t id_user = 0;
            meeting_data *to_insert = nullptr;

            try
            {
                if (console)
                {

                    read_id_meeting_from_console(id_user);
                    read_data_meeting_from_console(to_insert);

                    if (collection->first->in(id_user))
                    {
                        throw std::invalid_argument("Repeat meeting!");
                    }
                }
                else
                {
                    read_id_meeting(id_user, stream);
                    read_data_meeting(to_insert, stream);

                    if (collection->first->in(id_user))
                    {
                        throw std::invalid_argument("Repeat meeting!");
                    }
                }
            }
            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            auto *to_insert_value = new type_value(std::make_pair(to_insert, type_states()));

            collection->first->insert(id_user, to_insert_value);
            collection->second["TYPE"]->insert(to_insert->get_type()->get_value(), to_insert_value);
            collection->second["FORMAT"]->insert(to_insert->get_format()->get_value(), to_insert_value);
            collection->second["DESCRIPTION"]->insert(to_insert->get_description()->get_value(), to_insert_value);
            collection->second["LINK"]->insert(to_insert->get_link()->get_value(), to_insert_value);
            collection->second["SURNAME"]->insert(to_insert->get_surname()->get_value(), to_insert_value);
            collection->second["NAME"]->insert(to_insert->get_name()->get_value(), to_insert_value);
            collection->second["PATRONYMIC"]->insert(to_insert->get_patronymic()->get_value(), to_insert_value);
            collection->second["DATE"]->insert(to_insert->get_date()->get_value(), to_insert_value);
            collection->second["START_TIME"]->insert(to_insert->get_start_time()->get_value(), to_insert_value);
            collection->second["DURATION"]->insert(to_insert->get_duration()->get_value(), to_insert_value);
            collection->second["MEMBERS"]->insert(to_insert->get_members()->get_value(), to_insert_value);
        }
    };

    ~parser_add() override = default;
};



meeting_data *get_state(
        const std::string &datetime,
        type_value *states)
{

    auto *temp = new meeting_data();

    temp->set_type(states->first->get_type()->get_value());
    temp->set_format(states->first->get_format()->get_value());
    temp->set_description(states->first->get_description()->get_value());
    temp->set_link(states->first->get_link()->get_value());
    temp->set_surname(states->first->get_surname()->get_value());
    temp->set_name(states->first->get_name()->get_value());
    temp->set_patronymic(states->first->get_patronymic()->get_value());
    temp->set_date(states->first->get_date()->get_value());
    temp->set_start_time(states->first->get_start_time()->get_value());
    temp->set_duration(states->first->get_duration()->get_value());
    temp->set_members(states->first->get_members()->get_value());

    if (datetime.empty())
    {
        if (!states->second.empty())
        {
            for (const auto &item: states->second)
            {
                for (const auto &item_states: states->second[item.first])
                {
                    if (item_states.first == "TYPE")
                    {
                        temp->set_type(item_states.second);
                    }
                    else if (item_states.first == "FORMAT")
                    {
                        temp->set_format(item_states.second);
                    }
                    else if (item_states.first == "DESCRIPTION")
                    {
                        temp->set_description(item_states.second);
                    }
                    else if (item_states.first == "LINK")
                    {
                        temp->set_link(item_states.second);
                    }
                    else if (item_states.first == "SURNAME")
                    {
                        temp->set_surname(item_states.second);
                    }
                    else if (item_states.first == "NAME")
                    {
                        temp->set_name(item_states.second);
                    }
                    else if (item_states.first == "PATRONYMIC")
                    {
                        temp->set_patronymic(item_states.second);
                    }
                    else if (item_states.first == "DATE")
                    {
                        temp->set_date(item_states.second);
                    }
                    else if (item_states.first == "START_TIME")
                    {
                        temp->set_start_time(item_states.second);
                    }
                    else if (item_states.first == "DURATION")
                    {
                        temp->set_duration(item_states.second);
                    }
                    else if (item_states.first == "MEMBERS")
                    {
                        temp->set_members(item_states.second);
                    }

                }
            }
        }

        return temp;
    }

    long long time = get_time(datetime);

    if (!states->second.empty())
    {
        for (const auto &item: states->second)
        {
            if (time > item.first)
            {
                for (const auto &item_states: states->second[item.first])
                {
                    if (item_states.first == "TYPE")
                    {
                        temp->set_type(item_states.second);
                    }
                    else if (item_states.first == "FORMAT")
                    {
                        temp->set_format(item_states.second);
                    }
                    else if (item_states.first == "DESCRIPTION")
                    {
                        temp->set_description(item_states.second);
                    }
                    else if (item_states.first == "LINK")
                    {
                        temp->set_link(item_states.second);
                    }
                    else if (item_states.first == "SURNAME")
                    {
                        temp->set_surname(item_states.second);
                    }
                    else if (item_states.first == "NAME")
                    {
                        temp->set_name(item_states.second);
                    }
                    else if (item_states.first == "PATRONYMIC")
                    {
                        temp->set_patronymic(item_states.second);
                    }
                    else if (item_states.first == "DATE")
                    {
                        temp->set_date(item_states.second);
                    }
                    else if (item_states.first == "START_TIME")
                    {
                        temp->set_start_time(item_states.second);
                    }
                    else if (item_states.first == "DURATION")
                    {
                        temp->set_duration(item_states.second);
                    }
                    else if (item_states.first == "MEMBERS")
                    {
                        temp->set_members(item_states.second);
                    }

                }
            }
        }
    }

    return temp;
}

bool check_type(const std::string &type) {
    if (type == "daily" || type == "results" || type== "interview" || type == "corporate") {
        return true;
    } else {
        return false;
    }
}

bool check_format(const std::string &format)
{
    if (format == "face-to-face" || format == "remote")
    {
        return true;
    } else {
        return false;
    }
}

bool check_date(const std::string &date)
{
    //дописать
    return true;
}


class parser_get: public parser
{
public:
    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "GET_DATA")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't get data");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_get = new get_command;

            try
            {
                execute_get->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_get;
                throw std::invalid_argument(ex.what());
            }

            delete execute_get;

        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class get_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            size_t id_user = 0;

            type_value *get_target = nullptr;
            std::string names_field[11] = {
                    "TYPE",
                    "FORMAT",
                    "DESCRIPTION",
                    "LINK",
                    "SURNAME",
                    "NAME",
                    "PATRONYMIC",
                    "DATE",
                    "START_TIME",
                    "DURATION",
                    "MEMBERS",
            };



            try
            {
                if (console)
                {
                    int number_field = 0;
                    std::string field_value;

                    std::cout << "Choose field to be searched: " << std::endl;
                    std::cout << "1. Id" << std::endl;
                    std::cout << "2. Type" << std::endl;
                    std::cout << "3. Format" << std::endl;
                    std::cout << "4. Description" << std::endl;
                    std::cout << "5. Link" << std::endl;
                    std::cout << "6. Surname" << std::endl;
                    std::cout << "7. Name" << std::endl;
                    std::cout << "8. Patronymic" << std::endl;
                    std::cout << "9. Date" << std::endl;
                    std::cout << "10. Start time" << std::endl;
                    std::cout << "11. Duration" << std::endl;
                    std::cout << "12. Members" << std::endl;
                    std::cout << "Number field: ";
                    std::cin >> number_field;

                    if (number_field == 1)
                    {

                        read_id_meeting_from_console(id_user);

                        if (collection->first->in(id_user))
                        {
                            get_target = collection->first->get(id_user);
                        }
                        else
                        {
                            throw std::invalid_argument("Can't find meeting");
                        }
                    }
                    else if (number_field >= 2 && number_field <= 12)
                    {
                        std::cout << "PUT " + names_field[number_field - 2] + ": ";
                        std::cin >> field_value;

                        if (collection->second[names_field[number_field - 2]]->in(field_value))
                        {
                            get_target = collection->second[names_field[number_field - 2]]->get(field_value);
                        }
                        else
                        {
                            throw std::invalid_argument("Can't find meeting");
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Wrong number");
                    }
                }
                else
                {
                    std::string name_field;
                    std::string value_field;
                    std::string line;

                    if (stream.eof())
                    {
                        throw std::invalid_argument("End of file");
                    }

                    std::getline(stream, name_field);
                    name_field = parser::delete_spaces(name_field);

                    if (name_field == "ID")
                    {
                        try
                        {
                            read_id_meeting(id_user, stream);
                        }
                        catch (const std::exception &ex)
                        {
                            throw std::invalid_argument(ex.what());
                        }

                        get_target = collection->first->get(id_user);
                    }

                    else if (name_field == "TYPE" ||
                             name_field == "FORMAT" ||
                             name_field == "LINK" ||
                             name_field == "SURNAME" ||
                             name_field == "NAME" ||
                             name_field == "PATRONYMIC" ||
                             name_field == "DATE" ||
                             name_field == "START_TIME" ||
                             name_field == "DURATION" ||
                             name_field == "MEMBERS")
                    {
                        if (stream.eof())
                        {
                            throw std::invalid_argument("End of file");
                        }

                        std::getline(stream, line);

                        try
                        {
                            read_arguments(line, name_field, ":", value_field);
                        }
                        catch (const std::exception &ex)
                        {
                            throw std::invalid_argument(ex.what());
                        }

                        if (collection->second[name_field]->in(value_field))
                        {
                            get_target = collection->second[name_field]->get(value_field);
                        }
                        else
                        {
                            throw std::invalid_argument("Can't find meeting");
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Wrong name field");
                    }
                }
            }
            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }



            if (console)
            {
                meeting_data *temp = nullptr;
                std::vector<std::pair<std::string, std::string>> last_item;
                std::string datetime;
                long long _datetime = 0;
                int choice = 0;

                std::cout << "At what point in time do you want to get the data?" << std::endl;
                std::cout << "1. At right now" << std::endl;
                std::cout << "2. At a given point in time" << std::endl;
                std::cout << "Enter the number: ";
                std::cin >> choice;

                switch (choice)
                {
                    case 1:
                        temp = get_state("", get_target);
                        std::cout << std::endl << "FOUND MEETING:" << std::endl << std::endl;
                        output_meeting_data(temp);
                        std::cout << std::endl;
                        delete temp;

                        break;
                    case 2:
                        std::cout << "Put the datetime in format %d/%m/%y-%h:%m:%s: ";
                        std::cin >> datetime;

                        _datetime = get_time(datetime);
                        datetime = parser::delete_spaces(datetime);

                        if (check_date(datetime))
                        {
                            temp = get_state(datetime, get_target);
                            std::cout << std::endl << "FOUND MEETING AT A GIVEN POINT IN TIME" + datetime + ":" << std::endl << std::endl;
                            output_meeting_data(temp);
                            std::cout << std::endl;
                            delete temp;
                        }
                        else
                        {
                            throw std::invalid_argument("Wrong datetime");
                        }

                        break;
                    default:
                        throw std::invalid_argument("can't get item");
                }
            }
            else
            {
                std::string state;

                if (stream.eof())
                {
                    throw std::invalid_argument("File end");
                }

                std::getline(stream, state);
                state = parser::delete_spaces(state);

                if (state == "CURRENT")
                {
                    meeting_data *temp = get_state("", get_target);
                    std::cout << std::endl << "FOUND MEETING:" << std::endl << std::endl;
                    output_meeting_data(temp);
                    std::cout << std::endl;
                    delete temp;
                }
                else if (check_date(state))
                {
                    meeting_data *temp = get_state(state, get_target);
                    std::cout << std::endl << "FOUND MEETING AT A GIVEN POINT IN TIME " + state + ":" << std::endl << std::endl;
                    output_meeting_data(temp);
                    std::cout << std::endl;
                    delete temp;
                }
                else
                {
                    throw std::invalid_argument("Wrong state");
                }
            }
        }
    };

    ~parser_get() override = default;
};

class parser_get_range: public parser
{

public:

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "GET_RANGE")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't find collection");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_get_range = new get_range_command;

            try
            {
                execute_get_range->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_get_range;
                throw std::invalid_argument(ex.what());
            }

            delete execute_get_range;

        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class get_range_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            size_t id_user_lower_bound = 0;
            size_t id_delivery_lower_bound = 0;
            size_t id_user_upper_bound = 0;
            size_t id_delivery_upper_bound = 0;
            std::vector<type_value*> res;
            std::string names_field[11] = {
                    "TYPE",
                    "FORMAT",
                    "DESCRIPTION",
                    "LINK",
                    "SURNAME",
                    "NAME",
                    "PATRONYMIC",
                    "DATE",
                    "START_TIME",
                    "DURATION",
                    "MEMBERS",
            };

            try
            {
                if (console)
                {
                    int number_field = 0;
                    std::string lower_bound;
                    std::string upper_bound;

                    std::cout << "Choose field to be searched: " << std::endl;
                    std::cout << "1. Id" << std::endl;
                    std::cout << "2. Type" << std::endl;
                    std::cout << "3. Format" << std::endl;
                    std::cout << "4. Description" << std::endl;
                    std::cout << "5. Link" << std::endl;
                    std::cout << "6. Surname" << std::endl;
                    std::cout << "7. Name" << std::endl;
                    std::cout << "8. Patronymic" << std::endl;
                    std::cout << "9. Date" << std::endl;
                    std::cout << "10. Start time" << std::endl;
                    std::cout << "11. Duration" << std::endl;
                    std::cout << "12. Members" << std::endl;
                    std::cout << "Number field: ";
                    std::cin >> number_field;

                    try
                    {
                        if (number_field == 1)
                        {
                            std::cout << "PUT LOWER BOUND: " << std::endl;
                            read_id_meeting_from_console(id_user_lower_bound);
                            std::cout << "PUT UPPER BOUND: " << std::endl;
                            read_id_meeting_from_console(id_user_upper_bound);

                            res = collection->first->get_range(id_user_lower_bound,id_user_upper_bound);
                        }
                        else if (number_field >= 2 && number_field <= 12)
                        {
                            std::cout << "PUT LOWER BOUND: " << std::endl;
                            std::cin >> lower_bound;
                            std::cout << "PUT UPPER BOUND: " << std::endl;
                            std::cin >> upper_bound;

                            res = collection->second[names_field[number_field - 2]]->get_range(lower_bound, upper_bound);
                        }
                        else
                        {
                            throw std::invalid_argument("Wrong number_field");
                        }
                    }
                    catch (const std::exception &ex)
                    {
                        throw std::invalid_argument(ex.what());
                    }
                }
                else
                {
                    std::string name_field;
                    std::string lower_bound;
                    std::string upper_bound;
                    std::string line_lower_bound;
                    std::string line_upper_bound;

                    if (stream.eof())
                    {
                        throw std::invalid_argument("End of file");
                    }

                    std::getline(stream, name_field);
                    name_field = parser::delete_spaces(name_field);

                    if (name_field == "ID")
                    {
                        try
                        {
                            read_id_meeting(id_user_lower_bound, stream);
                            read_id_meeting(id_user_upper_bound, stream);
                        }
                        catch (const std::exception &ex)
                        {
                            throw std::invalid_argument(ex.what());
                        }

                        res = collection->first->get_range(id_user_lower_bound,
                                                           id_user_upper_bound);

                    }
                    else if (name_field == "TYPE" ||
                             name_field == "FORMAT" ||
                             name_field == "LINK" ||
                             name_field == "SURNAME" ||
                             name_field == "NAME" ||
                             name_field == "PATRONYMIC" ||
                             name_field == "DATE" ||
                             name_field == "START_TIME" ||
                             name_field == "DURATION" ||
                             name_field == "MEMBERS")
                    {
                        if (stream.eof())
                        {
                            throw std::invalid_argument("End of file");
                        }

                        std::getline(stream, line_lower_bound);

                        if (stream.eof())
                        {
                            throw std::invalid_argument("End of file");
                        }

                        std::getline(stream, line_upper_bound);

                        try
                        {
                            read_arguments(line_lower_bound, name_field, ":", lower_bound);
                            read_arguments(line_upper_bound, name_field, ":", upper_bound);
                        }
                        catch (const std::exception &ex)
                        {
                            throw std::invalid_argument(ex.what());
                        }

                        res = collection->second[name_field]->get_range(lower_bound, upper_bound);
                    }
                    else
                    {
                        throw std::invalid_argument("Wrong name field");
                    }
                }
            }
            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            meeting_data *temp = nullptr;

            if (res.empty())
            {
                std::cout << "NO MATCHES" << std::endl;
            }
            else
            {
                if (console)
                {
                    std::vector<std::pair<std::string, std::string>> last_item;
                    std::string datetime;
                    long long _datetime = 0;
                    int choice = 0;

                    std::cout << "At what point in time do you want to receive data?" << std::endl;
                    std::cout << "1. Current point in time" << std::endl;
                    std::cout << "2. Set point in time" << std::endl;
                    std::cout << "Put the number: ";
                    std::cin >> choice;

                    switch (choice)
                    {
                        case 1:
                            for (auto &item: res)
                            {
                                temp = get_state("", item);
                                output_meeting_data(temp);
                                std::cout << std::endl;
                                delete temp;
                            }

                            break;
                        case 2:
                            std::cout << "Put the datetime in format %d/%m/%y-%h:%m:%s: ";
                            std::cin >> datetime;

                            _datetime = get_time(datetime);
                            datetime = parser::delete_spaces(datetime);

                            if (check_date(datetime))
                            {
                                std::cout << std::endl << "FOUND MEETINGS:" << std::endl << std::endl;
                                for (auto &item: res)
                                {
                                    temp = get_state(datetime, item);
                                    output_meeting_data(temp);
                                    std::cout << std::endl;
                                    delete temp;
                                }
                            }
                            else
                            {
                                throw std::invalid_argument("Wrong format datetime");
                            }

                            break;
                        default:
                            throw std::invalid_argument("Can't get item");
                    }
                }
                else
                {
                    std::string state;

                    if (stream.eof())
                    {
                        throw std::invalid_argument("File end");
                    }

                    std::getline(stream, state);
                    state = parser::delete_spaces(state);

                    if (state == "CURRENT")
                    {
                        std::cout << std::endl << "FOUND MEETINGS:" << std::endl << std::endl;
                        for (auto &item: res)
                        {
                            temp = get_state("", item);
                            output_meeting_data(temp);
                            std::cout << std::endl;
                            delete temp;
                        }
                    }
                    else if (check_date(state))
                    {
                        for (auto &item: res)
                        {
                            std::cout << std::endl << "FOUND MEETINGS:" << std::endl << std::endl;
                            temp = get_state(state, item);
                            output_meeting_data(temp);
                            std::cout << std::endl;
                            delete temp;
                        }
                    }
                    else
                    {
                        throw std::invalid_argument("Can't get state");
                    }
                }
            }
        }
    };

    ~parser_get_range() override = default;
};

class parser_update: public parser
{

public:

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "UPDATE_OBJECT")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("can't update");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*> scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_update = new update_command;

            try
            {
                execute_update->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_update;
                throw std::invalid_argument(ex.what());
            }

            delete execute_update;

        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class update_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            size_t id_user = 0;
            meeting_data to_insert;

            try
            {
                if (console)
                {
                    read_id_meeting_from_console(id_user);
                }
                else
                {
                    read_id_meeting(id_user, stream);
                }
            }
            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            if (collection->first->in(id_user))
            {
                type_value *update_item = collection->first->get(id_user);
                long long current_time = get_time(get_current_datetime());

                if (console)
                {
                    int number_field = -1;

                    while (number_field != 0)
                    {
                        std::string new_value;

                        std::cout << "Choose number to update some field or cancel" << std::endl;
                        std::cout << "0. Cancel" << std::endl;
                        std::cout << "1. Type" << std::endl;
                        std::cout << "2. Format" << std::endl;
                        std::cout << "3. Description" << std::endl;
                        std::cout << "4. Link" << std::endl;
                        std::cout << "5. Surname" << std::endl;
                        std::cout << "6. Name" << std::endl;
                        std::cout << "7. Patronymic" << std::endl;
                        std::cout << "8. Date" << std::endl;
                        std::cout << "9. Start time" << std::endl;
                        std::cout << "10. Duration" << std::endl;
                        std::cout << "11. Members" << std::endl;
                        std::cout << "Put the number: ";
                        std::cin >> number_field;


                        switch (number_field)
                        {
                            case 0:
                                break;
                            case 1:
                                std::cout << "type: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("TYPE", new_value);
                                break;
                            case 2:
                                std::cout << "format: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("FORMAT", new_value);
                                break;
                            case 3:
                                std::cout << "Description: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("DESCRIPTION", new_value);
                                break;
                            case 4:
                                std::cout << "Link: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("LINK", new_value);
                                break;
                            case 5:
                                std::cout << "Surname: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("SURNAME", new_value);
                                break;
                            case 6:
                                std::cout << "Name: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("NAME", new_value);
                                break;
                            case 7:
                                std::cout << "Patronymic: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("PATRONYMIC", new_value);
                                break;
                            case 8:
                                std::cout << "Date: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("DATE", new_value);
                                break;
                            case 9:
                                std::cout << "Start time: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("START_TIME", new_value);
                                break;
                            case 10:
                                std::cout << "Duration: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("DURATION", new_value);
                                break;
                            case 11:
                                std::cout << "Members: ";
                                std::cin >> new_value;
                                update_item->second[current_time].emplace_back("MEMBERS", new_value);
                                break;
                            default:
                                throw std::invalid_argument("Wrong number to update");
                        }
                    }
                }
                else
                {
                    std::string line;

                    while (line != "FINISH_UPDATE")
                    {
                        if (stream.eof())
                        {
                            throw std::invalid_argument("Can't update");
                        }

                        std::getline(stream, line);

                        if (parser::delete_spaces(line) != "FINISH_UPDATE")
                        {
                            if (line.find(':') == std::string::npos)
                            {
                                throw std::invalid_argument("Can't update");
                            }

                            std::string name_argument = parser::delete_spaces(line.substr(0, line.find(':')));
                            std::string value_argument = parser::delete_spaces(line.substr(line.find(':') + 1));


                            if (name_argument == "TYPE")
                            {
                                update_item->second[current_time].emplace_back("type", value_argument);
                            }
                            else if (name_argument == "FORMAT")
                            {
                                update_item->second[current_time].emplace_back("format", value_argument);
                            }
                            else if (name_argument == "DESCRIPTION")
                            {
                                update_item->second[current_time].emplace_back("description", value_argument);
                            }
                            else if (name_argument == "LINK")
                            {
                                update_item->second[current_time].emplace_back("link", value_argument);
                            }
                            else if (name_argument == "SURNAME")
                            {
                                update_item->second[current_time].emplace_back("surname", value_argument);
                            }
                            else if (name_argument == "NAME")
                            {
                                update_item->second[current_time].emplace_back("name", value_argument);
                            }
                            else if (name_argument == "PATRONYMIC")
                            {
                                update_item->second[current_time].emplace_back("patronymic", value_argument);
                            }
                            else if (name_argument == "DATE")
                            {
                                update_item->second[current_time].emplace_back("date", value_argument);
                            }
                            else if (name_argument == "START_TIME")
                            {
                                update_item->second[current_time].emplace_back("start_time", value_argument);
                            }
                            else if (name_argument == "DURATION")
                            {
                                update_item->second[current_time].emplace_back("duration", value_argument);
                            }
                            else if (name_argument == "MEMBERS")
                            {
                                update_item->second[current_time].emplace_back("members", value_argument);
                            }
                            else
                            {
                                throw std::invalid_argument("Can't update");
                            }
                        }
                    }
                }
            }
            else
            {
                throw std::invalid_argument("can't find");
            }
        }
    };

    ~parser_update() override = default;
};

class parser_remove: public parser
{

public:

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        if (!params.empty() && params[0] == "DELETE_OBJECT")
        {
            type_data_base *data_base = parser::get_instance();

            if (params.size() != 4)
            {
                throw std::invalid_argument("Can't delete");
            }

            if (!data_base->in(params[1]))
            {
                throw std::invalid_argument("Can't find pool");
            }

            std::pair<type_pool*, memory*> pool = data_base->get(params[1]);

            if (!pool.first->in(params[2]))
            {
                throw std::invalid_argument("Can't find scheme");
            }

            std::pair<type_scheme*, memory*>scheme = pool.first->get(params[2]);

            if (!scheme.first->in(params[3]))
            {
                throw std::invalid_argument("Can't find collection");
            }

            std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection = scheme.first->get(params[3]);

            command *execute_remove = new remove_command;

            try
            {
                execute_remove->execute(params, &pool, &scheme, collection, stream, console);
            }
            catch (const std::exception &ex)
            {
                delete execute_remove;
                throw std::invalid_argument(ex.what());
            }

            delete execute_remove;

        }
        else
        {
            if (_next_parser != nullptr)
            {
                _next_parser->parser_request(params, stream, console);
            }
        }
    }

    class remove_command: public command
    {

    public:

        void execute(
                const std::vector<std::string> &params,
                std::pair<type_pool*, memory*> *pool,
                std::pair<type_scheme*, memory*> *scheme,
                std::pair<type_collection*, std::map<std::string, type_order_collection*>> *collection,
                std::istream &stream,
                bool console) override
        {
            size_t id_user = 0;

            try
            {
                if (console)
                {
                    read_id_meeting_from_console(id_user);
                }
                else
                {
                    read_id_meeting(id_user, stream);
                }
            }
            catch (const std::exception &ex)
            {
                throw std::invalid_argument(ex.what());
            }

            if (collection->first->in(id_user))
            {
                type_value *deleted_value = collection->first->get(id_user);


                collection->second["TYPE"]->remove(deleted_value->first->get_type()->get_value());
                collection->second["FORMAT"]->remove(deleted_value->first->get_format()->get_value());
                collection->second["DESCRIPTION"]->remove(deleted_value->first->get_description()->get_value());
                collection->second["LINK"]->remove(deleted_value->first->get_link()->get_value());
                collection->second["SURNAME"]->remove(deleted_value->first->get_surname()->get_value());
                collection->second["NAME"]->remove(deleted_value->first->get_name()->get_value());
                collection->second["PATRONYMIC"]->remove(deleted_value->first->get_patronymic()->get_value());
                collection->second["DATE"]->remove(deleted_value->first->get_date()->get_value());
                collection->second["START_TIME"]->remove(deleted_value->first->get_start_time()->get_value());
                collection->second["DURATION"]->remove(deleted_value->first->get_duration()->get_value());
                collection->second["MEMBERS"]->remove(deleted_value->first->get_members()->get_value());
                collection->first->remove(id_user);
            }
            else
            {
                throw std::invalid_argument("can't remove");
            }
        }
    };

    ~parser_remove() override = default;
};

class parser_concrete: public parser
{

public:

    parser_concrete()
    {
        parser *_run = new parser_run;
        parser *_add_pool = new parser_add_pool;
        parser *_remove_pool = new parser_remove_pool;
        parser *_add_scheme = new parser_add_scheme;
        parser *_remove_scheme = new parser_remove_scheme;
        parser *_add_collection = new parser_add_collection;
        parser *_remove_collection = new parser_remove_collection;
        parser *_add = new parser_add;
        parser *_get = new parser_get;
        parser *_get_range = new parser_get_range;
        parser *_update = new parser_update;
        parser *_remove = new parser_remove;

        set_next(_run);
        _run->set_next(_add_pool);
        _add_pool->set_next(_remove_pool);
        _remove_pool->set_next(_add_scheme);
        _add_scheme->set_next(_remove_scheme);
        _remove_scheme->set_next(_add_collection);
        _add_collection->set_next(_remove_collection);
        _remove_collection->set_next(_add);
        _add->set_next(_get);
        _get->set_next(_get_range);
        _get_range->set_next(_update);
        _update->set_next(_remove);
        _remove->set_next(nullptr);
        parser::get_logger()->log("Create chain", logger::severity::information);
    }

    void parser_request(
            const std::vector<std::string> &params,
            std::istream &stream,
            bool console) override
    {
        try
        {
            _next_parser->parser_request(params, stream, console);
        }
        catch (const std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    ~parser_concrete() override
    {
        auto *data_base = reinterpret_cast<type_data_base_avl*>(parser::get_instance());
        auto data_base_end = data_base->end_prefix();

        for (auto it_base = data_base->begin_prefix(); it_base != data_base_end; ++it_base)
        {
            auto end_pool= std::get<2>(*it_base).first->end_prefix();
            auto begin_pool = std::get<2>(*it_base).first->begin_prefix();

            for (auto it_pool = begin_pool; it_pool != end_pool; ++it_pool)
            {
                auto end_scheme = std::get<2>(*it_pool).first->end_prefix();
                auto begin_scheme = std::get<2>(*it_pool).first->begin_prefix();

                for (auto it_scheme = begin_scheme; it_scheme != end_scheme; ++it_scheme)
                {
                    auto end_collection = std::get<2>(*it_scheme)->first->end_prefix();
                    auto begin_collection = std::get<2>(*it_scheme)->first->begin_prefix();

                    for (auto it_collection = begin_collection; it_collection != end_collection; ++it_collection)
                    {
                        delete std::get<2>(*it_collection)->first;
                        delete std::get<2>(*it_collection);
                    }

                    delete std::get<2>(*it_scheme)->first;
                    delete std::get<2>(*it_scheme)->second["TYPE"];
                    delete std::get<2>(*it_scheme)->second["FORMAT"];
                    delete std::get<2>(*it_scheme)->second["DESCRIPTION"];
                    delete std::get<2>(*it_scheme)->second["LINK"];
                    delete std::get<2>(*it_scheme)->second["SURNAME"];
                    delete std::get<2>(*it_scheme)->second["NAME"];
                    delete std::get<2>(*it_scheme)->second["PATRONYMIC"];
                    delete std::get<2>(*it_scheme)->second["DATE"];
                    delete std::get<2>(*it_scheme)->second["START_TIME"];
                    delete std::get<2>(*it_scheme)->second["DURATION"];
                    delete std::get<2>(*it_scheme)->second["MEMBERS"];
                    delete std::get<2>(*it_scheme);
                    std::get<2>(*it_pool).first->remove(std::get<1>(*it_scheme));
                }

                delete std::get<2>(*it_pool).first;
                std::get<2>(*it_base).first->remove(std::get<1>(*it_pool));
            }

            delete std::get<2>(*it_base).second;
            delete std::get<2>(*it_base).first;
            data_base->remove(std::get<1>(*it_base));
        }

        delete parser::get_instance();
    }
};

bool check_id(const std::string &id)
{
    if(id.empty())
    {
        return false;
    }


    for(char letter: id)
    {
        if(!std::isdigit(letter))
        {
            return false;
        }
    }

    if(stoi(id) >= 0 )
    {
        return true;
    }

    return false;
}

void read_id_meeting(size_t &id_user, std::istream &stream)
{
    std::string name_arguments[1] = {"ID"};

    int num_read_fields = 0;
    std::string fields[1];
    std::string line;

    for (int i = 0; i < 1 && !stream.eof(); i++)
    {
        std::getline(stream, line);

        try
        {
            read_arguments(line, name_arguments[i], ":", fields[num_read_fields++]);
        }
        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    if (num_read_fields != 1)
    {
        throw std::out_of_range("Don't enough data..");
    }

    if(check_id(fields[0]))
    {
        id_user = std::stoi(fields[0]);
    }
    else
    {
        throw std::out_of_range("Wrong id");
    }
}

void read_data_meeting(
        meeting_data *&target,
        std::istream &stream)
{
    target = new meeting_data;

    std::string name_arguments[11] = {
            "TYPE",
            "FORMAT",
            "DESCRIPTION",
            "LINK",
            "SURNAME",
            "NAME",
            "PATRONYMIC",
            "DATE",
            "START_TIME",
            "DURATION",
            "MEMBERS"
    };

    int num_read_fields = 0;
    std::string fields[11];
    std::string line;

    for (int i = 0; i < 11 && !stream.eof(); i++)
    {
        std::getline(stream, line);

        try
        {
            read_arguments(line, name_arguments[i], ":", fields[num_read_fields++]);
        }
        catch (const std::exception &ex)
        {
            throw std::invalid_argument(ex.what());
        }
    }

    if (num_read_fields != 11)
    {
        throw std::out_of_range("Don't enough data..");
    }

    if (!check_type(fields[0]))
    {
        throw std::invalid_argument("You should write \"daily\" or \"results\" or \"interview\" or \"corporate\"");
    }

    target->set_type(fields[0]);

    if (!check_format(fields[1]))
    {
        throw std::invalid_argument("You should write \"remote\" or \"face-to-face\"");
    }
    target->set_format(fields[1]);

    target->set_description(fields[2]);
    target->set_link(fields[3]);
    target->set_surname(fields[4]);
    target->set_name(fields[5]);
    target->set_patronymic(fields[6]);
    target->set_date(fields[7]);
    target->set_start_time(fields[8]);
    target->set_duration(fields[9]);
    target->set_members(fields[9]);
}



void read_id_meeting_from_console(size_t &id_user)
{
    std::string id_user_string;
    std::cout << "ID: ";
    std::cin >> id_user_string;
    if(check_id(id_user_string))
    {
        id_user = std::stoi(id_user_string);
    }
    else
    {
        throw std::invalid_argument("Wrong id!");
    }
}

void read_data_meeting_from_console(
        meeting_data *&target)
{
    std::string data;
    target = new meeting_data;

    std::cout << "TYPE: ";
    std::cin >> data;

    if (!check_type(data))
    {
        throw std::invalid_argument("You should write \"daily\" or \"results\" or \"interview\" or \"corporate\"");
    }

    target->set_type(data);


    std::cout << "FORMAT: ";
    std::cin >> data;

    if (!check_format(data))
    {
        throw std::invalid_argument("You should write \"remote\" or \"face-to-face\"");
    }
    target->set_format(data);


    std::cout << "DESCRIPTION: ";
    std::cin >> data;
    target->set_description(data);

    std::cout << "LINK: ";
    std::cin >> data;
    target->set_link(data);


    std::cout << "SURNAME: ";
    std::cin >> data;
    target->set_surname(data);


    std::cout << "NAME: ";
    std::cin >> data;
    target->set_name(data);


    std::cout << "PATRONYMIC: ";
    std::cin >> data;
    target->set_patronymic(data);


    std::cout << "DATE: ";
    std::cin >> data;
    target->set_date(data);

    std::cout << "Start time: ";
    std::cin >> data;
    target->set_start_time(data);

    std::cout << "Duration: ";
    std::cin >> data;
    target->set_duration(data);

    std::cout << "Members: ";
    std::cin >> data;
    target->set_members(data);


}

void output_meeting_data(
        meeting_data *&target)
{
    std::cout << "TYPE: " << target->get_type()->get_value() << std::endl;
    std::cout << "FORMAT: " << target->get_format()->get_value() << std::endl;
    std::cout << "DESCRIPTION: " << target->get_description()->get_value() << std::endl;
    std::cout << "LINK: " << target->get_link()->get_value() << std::endl;
    std::cout << "SURNAME: " << target->get_surname()->get_value() << std::endl;
    std::cout << "NAME: " << target->get_name()->get_value() << std::endl;
    std::cout << "PATRONYMIC: " << target->get_patronymic()->get_value() << std::endl;
    std::cout << "DATE: " << target->get_date()->get_value() << std::endl;
    std::cout << "START_TIME: " << target->get_start_time()->get_value() << std::endl;
    std::cout << "DURATION: " << target->get_duration()->get_value() << std::endl;
    std::cout << "MEMBERS: " << target->get_members()->get_value() << std::endl;
}

void read_arguments(
        const std::string &str,
        const std::string &name_argument,
        const std::string &sep,
        std::string &value_argument)
{
    if (str.find(sep) == std::string::npos)
    {
        throw std::invalid_argument("can't read arguments");
    }

    std::string _name_argument = parser::delete_spaces(str.substr(0, str.find(sep)));

    if (_name_argument != name_argument)
    {
        throw std::invalid_argument("Wrong name argument");
    }

    value_argument = parser::delete_spaces(str.substr(str.find(sep) + 1));
}

std::string get_current_datetime()
{
    std::time_t seconds = std::time(nullptr);
    std::tm* now = std::localtime(&seconds);

    char buffer[BUFSIZ];
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %X", now);

    return buffer;
}

bool is_ves(
        int year)
{
    if (year % 4 == 0)
    {
        if (year % 100 == 0)
        {
            if (year % 400 == 0)
            {
                return true;
            }
        }
        else
        {
            return true;
        }
    }

    return false;
}

long long get_time(
        const std::string &time)
{
    int day_in_months[12] = {
            31,
            28,
            31,
            30,
            31,
            30,
            31,
            31,
            30,
            31,
            30,
            31
    };

    long long full_seconds = 0;
    long long full_days = 0;

    int day = std::stoi(time.substr(0, 2));
    int month = std::stoi(time.substr(3, 2));
    int year = std::stoi(time.substr(6, 4));
    int hours = std::stoi(time.substr(11, 2));
    int minutes = std::stoi(time.substr(14, 2));
    int seconds = std::stoi(time.substr(17, 2));

    full_days += day;
    if (month == 2) {
        if (is_ves(year)) {
            full_days += day_in_months[month - 1] + 1;
        } else {
            full_days += day_in_months[month - 1];
        }
    } else {
        full_days += day_in_months[month - 1];
    }


    for (int i = 0; i <= year; i++)
    {
        if (is_ves(i))
        {
            full_days += 366;
        }
        else
        {
            full_days += 365;
        }
    }

    full_seconds += seconds;
    full_seconds += minutes * 60;
    full_seconds += hours * 60 * 60;
    full_seconds += full_days * 24 * 60 * 60;

    return full_seconds;
}

#endif