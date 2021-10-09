#include "Timer.h"

#ifdef _Timer_date
void Timer_date(unsigned long seconds, struct calendar *tm)
{
    unsigned int minutes, hours, days, year, month;
    unsigned int day_of_week;

    /* calculate minutes */
    minutes = seconds / 60;
    seconds -= minutes * 60;
    /* calculate hours */
    hours = minutes / 60;
    minutes -= hours * 60;
    /* calculate days */
    days = hours / 24;
    hours -= days * 24;

    /* Unix time starts in 1970 on a Thursday */
    year        = 1970;
    day_of_week = 4;

    while (1) {
        bool leap_year
            = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        unsigned int days_in_year = leap_year ? 366 : 365;
        if (days >= days_in_year) {
            day_of_week += leap_year ? 2 : 1;
            days -= days_in_year;
            if (day_of_week >= 7)
                day_of_week -= 7;
            ++year;
        } else {
            tm->yday = days;
            day_of_week += days;
            day_of_week %= 7;

            /* calculate the month and day */
            static const short days_in_month[12]
                = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            for (month = 0; month < 12; ++month) {
                short dim = days_in_month[month];

                /* add a day to feburary if this is a leap year */
                if (month == 1 && leap_year)
                    ++dim;

                if (days >= dim)
                    days -= dim;
                else
                    break;
            }
            break;
        }
    }

    tm->sec  = seconds;
    tm->min  = minutes;
    tm->hour = hours;
    tm->mday = days + 1;
    tm->mon  = month;
    tm->year = year;
    tm->wday = day_of_week;
}
#endif


///
/// \brief Configure timer handler
///
willMount(Timer)
{
    void *ptr = 0;
    props->timer->init(ptr);
}

///
/// \brief Timer should update everytime
///
shouldUpdate(Timer) { return true; }

///
/// \brief Calculate total passed timer ticks
///
willUpdate(Timer)
{
    unsigned int tick = props->timer->get();

    state->passed = tick - state->tick;
    if (state->passed < 0) {
        /* Handle timer overflow */
        state->passed = 65535 - state->tick + tick;
    }

    state->tick = tick;
}

///
/// \brief Update us, ms and s timestamps
///
release(Timer)
{
    unsigned int us_passed = props->timer->usFromTicks(state->passed);
    state->time.step_us    = us_passed;
    state->time.us += us_passed;
    state->us += us_passed;

    if (state->us > 1000) {
        unsigned int ms_passed = state->us / 1000;
        state->time.ms += ms_passed;
        state->ms += ms_passed;
        state->us -= ms_passed * 1000;

        if (state->ms > 1000) {
            unsigned int s_passed = state->ms / 1000;
            state->time.timestamp += s_passed;
            state->ms -= s_passed * 1000;

            if (props->onSecond)
                props->onSecond(self);
        }
    }
}

didMount(Timer)
{
#ifdef _Timer_date
    if (props->calendar)
        Timer_date(state->time.timestamp, props->calendar);
#endif
}

didUpdate(Timer) {}
