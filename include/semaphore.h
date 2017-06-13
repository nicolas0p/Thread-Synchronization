// EPOS Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <synchronizer.h>
#include <utility/handler.h>

__BEGIN_SYS

class Semaphore: protected Synchronizer_Common
{
public:
    Semaphore(int v = 1);
    ~Semaphore();

    void p();
    void v();

private:
    volatile int _value;

public:
    class V_Handler: public Handler {
        Semaphore & semaphore;
    public:
        V_Handler(Semaphore & _semaphore) : semaphore(_semaphore) {}

        void operator()() { semaphore.v(); }
    }

    // class P_Handler
};


__END_SYS

#endif
