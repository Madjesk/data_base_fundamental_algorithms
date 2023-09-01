#include "logger_guard.h"
#include "../logger/logger_concrete.h"
#include "../logger/logger.h"
#include "../logger/logger_builder_concrete.h"

logger *logger_guard::logger_with_guard(const std::string &target, logger::severity level) const
{
    logger* logg = get_logger();
    if (logg != nullptr){
        logg->log(target, level);
    }
    else{
        std::cout << "Logger error" << target << std::endl;
    }
}