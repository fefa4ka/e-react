#include "Calendar.h"

willMount(Time) {
    void *ptr;
    props->timer->init(ptr);
}

shouldUpdate(Time) {
    return true;  
}

willUpdate(Time) { 
    unsigned int tick = props->timer->get();

    state->passed = tick - state->tick;
    if(state->passed < 0) {
        state->passed = 65535 - state->tick + tick;
    }

    state->tick = tick;
}

release(Time) {
    unsigned int us_passed = props->timer->usFromTicks(state->passed);
    props->time->step_us = us_passed;
    props->time->microsecond += us_passed; // state->passed;//134;//ms_passed;
    props->time->time_us += us_passed;

    if(props->time->microsecond > 1000) {
        unsigned int ms_passed = props->time->microsecond / 1000;
        props->time->time_ms += ms_passed;
        props->time->millisecond += ms_passed; 
        props->time->microsecond -= ms_passed * 1000;

        if(props->time->millisecond > 1000) {
            unsigned int s_passed = props->time->millisecond / 1000;
            props->time->second += s_passed; 
            props->time->millisecond -= s_passed * 1000;
            if(props->onSecond) props->onSecond(self);

            if(props->time->second >= 60) {
                unsigned int m_passed = props->time->second / 60;
                props->time->minute += m_passed;
                props->time->second -= m_passed * 60;
                if(props->onMinute) props->onMinute(self);

                if(props->time->minute >= 60) {
                    unsigned int h_passed = props->time->minute / 60;
                    props->time->hour += h_passed;
                    props->time->minute -= h_passed * 60;
                    if(props->onHour) props->onHour(self);

                    if(props->time->hour >= 24) {
                        unsigned int d_passed = props->time->hour / 24;
                        props->time->date += d_passed;
                        props->time->hour -= d_passed * 24;

                        if(props->onDay) props->onDay(self);

                        // TODO: month, year
                    }
                }
            }
        }

    }
}

didMount(Time) { }
didUnmount(Time) { }

didUpdate(Time) {
  
}


React_Constructor(Time)
