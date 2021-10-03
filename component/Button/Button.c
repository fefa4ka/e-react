#include "Button.h"

#if !defined(_Button_poll)
static void Button_isr(void *self)
{
    Button_Component *button   = self;
    Component *       instance = &button->instance;

    if (instance->ShouldUpdate(instance, 0)) {
        instance->WillUpdate(instance, 0);
        instance->Release(instance);
    }

    /* Check bounce */
    if (button->props.bounce_delay_ms) {
        button->props.timer->set(button->props.bounce_delay_ms, Button_isr,
                                button);
    }
}
#endif

/**
 * \brief  Configure pins for input and pullup if needed
 */
willMount(Button)
{
    io_handler *io = props->io;

    /* Setup pin as input */
    io->in(props->pin);
    state->inverse
        = props->type == BTN_PUSH_PULLUP || props->type == BTN_TOGGLE_PULLUP;

    if (state->inverse) {
        io->pullup(props->pin);
    }

#if !defined(_Button_poll)
    /* Use interrupt if available */
    if (props->timer && io->isr && io->isr->is_available(props->pin)) {
        io->isr->mount(props->pin, Button_isr, self);
    }
#endif
}

/**
 * \brief  Check if level doesn't change during bounce_delay_ms
 *         or check if push button released
 */
shouldUpdate(Button)
{
    bool level          = props->io->get(props->pin);
    bool is_level_equal = state->level == level;
    bool state_level    = state->inverse ? !state->level : state->level;
    int  passed         = props->clock->ms - state->tick;

    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->clock->ms;
    }

    /* Second check after bounce_delay_ms */
    if (state->tick && passed >= props->bounce_delay_ms) {
        return true;
    }

    /* Already pressed */
    if (state_level == 1 && is_level_equal && state->pressed) {
        return false;
    }

    /* First high level detected */
    if (state_level == 0 && !is_level_equal) {
        return true;
    }

    /* Another checks after pressed */
    if ((props->type == BTN_PUSH_PULLUP || props->type == BTN_PUSH)
        && state->pressed && (level == 0 || state->inverse)) {
        /* Push button unpressed after release */
        return true;
    }

    state->level = level;
    return false;
}

/**
 * \brief            Prepare button internal states
 */
willUpdate(Button)
{
    /* Actual state reading */
    state->level     = props->io->get(props->pin);
    bool state_level = state->inverse ? !state->level : state->level;

    /* Set initial tick to start delay counting */
    if (!state->tick && state_level) {
        state->tick = props->clock->ms;
    }
}

/**
 * \brief            Button pressed state logic
 */
release(Button)
{
    int  passed      = props->clock->ms - state->tick;
    bool pressed     = false;
    bool state_level = state->inverse ? !state->level : state->level;


    /* Tick overflow */
    if (passed < 0) {
        passed = 65535 - state->tick + props->clock->ms;
    }

    /* Cache button state for toggling */
    if (props->type == BTN_TOGGLE_PULLUP || props->type == BTN_TOGGLE) {
        pressed = state->pressed;
    }

    /* Change state if bounce filtered */
    if (state->tick && passed > props->bounce_delay_ms) {
        if (state_level) {
            if (props->type == BTN_TOGGLE_PULLUP || props->type == BTN_TOGGLE) {
                pressed = !state->pressed;
            } else {
                pressed = true;
            }
        }

        state->tick = 0;
    }

    /* Callback triggering */
    if (state->pressed != pressed) {
        if (pressed) {
            if (props->onPress)
                props->onPress(self);
        } else {
            if (props->onRelease)
                props->onRelease(self);
        }

        if (props->onToggle)
            props->onToggle(self);

        state->pressed = pressed;
    }
}

didMount(Button) {}
didUpdate(Button) {}
didUnmount(Button)
{
#if !defined(_Button_poll)
    /* Use interrupt if available */
    if (props->timer && props->io->isr && props->io->isr->is_available(props->pin)) {
        props->io->isr->umount(props->pin);
    }
#endif
}
