// EPOS Synchronizer Abstractions Common Package

#ifndef __synchronizer_h
#define __synchronizer_h

#include <cpu.h>
#include <thread.h>

__BEGIN_SYS

class Synchronizer_Common
{
protected:
    Synchronizer_Common() {}
    ~Synchronizer_Common() { begin_atomic(); wakeup_all(); }

    // Atomic operations
    bool tsl(volatile bool & lock) { return CPU::tsl(lock); }
    int finc(volatile int & number) { return CPU::finc(number); }
    int fdec(volatile int & number) { return CPU::fdec(number); }

    // Thread operations
    void begin_atomic() { Thread::lock(); }
    void end_atomic() { Thread::unlock(); }

    void sleep()
    {
        begin_atomic();
        _waiting.insert(&Thread::running()->_link);
        end_atomic();

        Thread::running()->suspend();
    }

    void wakeup()
    {
        begin_atomic();
        if(!_waiting.empty()){
            _waiting.remove()->object()->resume();
        }
        end_atomic();
    }

    void wakeup_all()
    {
        begin_atomic();
        while(!_waiting.empty()) {
            _waiting.remove()->object()->resume();
        }
        end_atomic();
    }

private:
    Thread::Queue _waiting; //thread waiting for this lock
};

__END_SYS

#endif
