#include "Time.h"


willMount(Time_block) {
    void *ptr;
    props->timer->init(ptr);
}

shouldUpdate(Time_block) {
    return true;  
}

willUpdate(Time_block) { 
    unsigned int tick = props->timer->getTimer();

    state->passed = tick - state->tick;
    if(state->passed < 0) {
        state->passed = 65535 - state->tick + tick;
    }

    state->tick = tick;
}

release(Time_block) {
    unsigned int us_passed = props->timer->usFromTicks(state->passed);

    props->time->microsecond += us_passed; // state->passed;//134;//ms_passed;
    
    if(props->time->microsecond > 1000) {
        unsigned int ms_passed = props->time->microsecond / 1000;

        props->time->millisecond += ms_passed; 

        if(props->time->millisecond > 1000) {
            unsigned int s_passed = props->time->millisecond / 1000;
            props->time->second += s_passed; 
            props->time->millisecond -= s_passed * 1000;

            if(props->time->second >= 60) {
                unsigned int m_passed = props->time->second / 60;
                props->time->minute += m_passed;
                props->time->second -= m_passed * 60;

                if(props->time->minute >= 60) {
                    unsigned int h_passed = props->time->minute / 60;
                    props->time->hour += h_passed;
                    props->time->minute -= h_passed * 60;

                    if(props->time->hour >= 24) {
                        unsigned int d_passed = props->time->hour / 24;
                        props->time->date += d_passed;
                        props->time->hour -= d_passed * 24;

                        // TODO: month, year
                    }
                }
            }
        }

        props->time->microsecond -= ms_passed * 1000;
    }
}

didMount(Time_block) { }
didUnmount(Time_block) { }

didUpdate(Time_block) {
  
}


React_Constructor(Time_block);
