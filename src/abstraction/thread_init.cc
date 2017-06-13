// EPOS Thread Abstraction Initialization

#include <system/kmalloc.h>
#include <system.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

void Thread::init()
{
    // The installation of the scheduler timer handler must precede the
    // creation of threads, since the constructor can induce a reschedule
    // and this in turn can call timer->reset()
    // Letting reschedule() happen during thread creation is harmless, since
    // MAIN is created first and dispatch won't replace it nor by itself
    // neither by IDLE (which has a lower priority)
    if(preemptive)
        _timer = new (kmalloc(sizeof(Scheduler_Timer))) Scheduler_Timer(QUANTUM, time_slicer);

    // Puts an idling thread on the ready queue.
    // Its execution will never end, therefore doesnt ever need to be replaced.
    // Its priority is lower than any other thread, therefore will only
    // run if no other thread is present.
    new (kmalloc(sizeof(Thread))) Thread(Thread::Configuration(Thread::READY, Thread::IDLE), &Thread::idle);
}

__END_SYS
