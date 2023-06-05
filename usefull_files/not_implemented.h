#ifndef SANDBOX_CPP_NOT_IMPLEMENTED_H
#define SANDBOX_CPP_NOT_IMPLEMENTED_H

#include <stdexcept>

class not_implemented final:
    public std::logic_error
{

public:

    explicit not_implemented(
        std::string const &method_name);

};

#endif //SANDBOX_CPP_NOT_IMPLEMENTED_H
