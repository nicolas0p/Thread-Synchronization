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

    // Atomic operations
    bool tsl(volatile bool & lock) { return CPU::tsl(lock); }
    int finc(volatile int & number) { return CPU::finc(number); }
    int fdec(volatile int & number) { return CPU::fdec(number); }

    // Thread operations
    void begin_atomic() { Thread::lock(); }
    void end_atomic() { Thread::unlock(); }

    void sleep() { Thread::yield(); } // implicit unlock()
    /*
	 *@param time time the thread is supposed to sleep for in SECONDS
	*/
	void sleep(const double& time)
	{
		Alarm(time*1e6, &(this->wakeup()),1); //is that correct?
	}
    void wakeup() { end_atomic(); }
    void wakeup_all() { end_atomic(); }
};

__END_SYS

#endif

