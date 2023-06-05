#ifndef SANDBOX_CPP_TYPENAME_HOLDER_H
#define SANDBOX_CPP_TYPENAME_HOLDER_H

#include <string>

class typename_holder
{

public:

    virtual ~typename_holder() noexcept = default;

protected:

    virtual std::string get_typename() const noexcept = 0;

};

#endif //SANDBOX_CPP_TYPENAME_HOLDER_H
