// EPOS Handler Utility Declarations

#ifndef __handler_h
#define __handler_h

#include <system/config.h>

__BEGIN_UTIL

//typedef void (Handler)();
class Handler
{
    void (*func)();
public:
    Handler() {}
    Handler() : func(func) {}

    virtual void operator()(){ (*func)() };
};

__END_UTIL

#endif
