#include <iostream>

#include <algorithm>
#include "../parser/parser.h"
#include "../parser_concrete/parser_concrete.h"

int main()
{
    parser *cmd = new parser_concrete();
    std::string command = "";

    while (command != "FINISH")
    {
        std::getline(std::cin, command);

        try
        {
            cmd->accept_request(command);
        }
        catch (const std::exception &ex)
        {
            std::cout << ex.what() << std::endl;
        }
    }

    return 0;
}

