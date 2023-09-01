#include "../logger/logger.h"

class logger_guard
{
public:
    logger* logger_with_guard(const std::string &target, logger::severity level) const;
    virtual ~logger_guard() = default;
    virtual logger *get_logger() const = 0;
};