// EPOS Alarm Abstraction Implementation

#include <semaphore.h>
#include <alarm.h>
#include <display.h>
#include <thread.h>

__BEGIN_SYS

// Class attributes
Alarm_Timer * Alarm::_timer;
volatile Alarm::Tick Alarm::_elapsed;
Alarm::Queue Alarm::_request;


// Methods
Alarm::Alarm(const Microsecond & time, Handler * handler, int times)
: _ticks(ticks(time)), _handler(handler), _semaphore(0), _times(times), _link(this, _ticks)
{
    lock();

    db<Alarm>(TRC) << "Alarm(t=" << time << ",tk=" << _ticks << ",h=" << reinterpret_cast<void *>(handler)
                   << ",x=" << times << ") => " << this << endl;

    if(_ticks) {
        _request.insert(&_link);
        unlock();
    } else {
        unlock();
        // in this case it shouldn't create a new thread to execute.
        // it will execute the handler in the same thread that called
        // for the setup of the alarm.
        (*handler)();
    }
}

Alarm::Alarm(const Microsecond & time, Semaphore * semaphore, int times)
: _ticks(ticks(time)), _handler(0), _semaphore(semaphore), _times(times), _link(this, _ticks)
{
    lock();

    // db<Alarm>(TRC) << "Alarm(t=" << time << ",tk=" << _ticks << ",h=" << reinterpret_cast<void *>(handler)
    //                << ",x=" << times << ") => " << this << endl;

    if(_ticks) {
        _request.insert(&_link);
        unlock();
    } else {
        unlock();
        semaphore->v();
    }
}


Alarm::~Alarm()
{
    lock();

    db<Alarm>(TRC) << "~Alarm(this=" << this << ")" << endl;

    _request.remove(this);

    unlock();
}


// Class methods
void Alarm::delay(const Microsecond & time)
{
    db<Alarm>(TRC) << "Alarm::delay(time=" << time << ")" << endl;

    Semaphore s(0);
    Alarm(time, &s, 1);
    s.p();
}


void Alarm::handler(const IC::Interrupt_Id & i)
{
    static Tick next_tick;
    static Handler * next_handler;
    static Semaphore * next_semaphore;

    lock();

    _elapsed++;

    if(Traits<Alarm>::visible) {
        Display display;
        int lin, col;
        display.position(&lin, &col);
        display.position(0, 79);
        display.putc(_elapsed);
        display.position(lin, col);
    }

    if(next_tick)
        next_tick--;
    if(!next_tick) {
        if(next_handler) {
            db<Alarm>(TRC) << "Alarm::handler(h=" << reinterpret_cast<void *>(next_handler) << ")" << endl;
            create_handler_thread(next_handler);
        } else if(next_semaphore) {
            // db<Alarm>(TRC) << "Alarm::handler(h=" << reinterpret_cast<void *>(next_handler) << ")" << endl;
            next_semaphore->v();
        }

        if(_request.empty()){
            next_handler = 0;
            next_semaphore = 0;
        } else {
            Queue::Element * e = _request.remove();
            Alarm * alarm = e->object();
            next_tick = alarm->_ticks;
            next_handler = alarm->_handler;
            next_semaphore = alarm->_semaphore;
            if(alarm->_times != -1)
                alarm->_times--;
            if(alarm->_times) {
                e->rank(alarm->_ticks);
                _request.insert(e);
            }
        }
    }

    unlock();
}

void Alarm::create_handler_thread(Handler * handler){
    new (kmalloc(sizeof(Thread))) Thread(Thread::Configuration(Thread::READY, Thread::HIGH), &wrap_handler, handler);
}

__END_SYS
