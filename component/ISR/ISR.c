#include "ISR.h"

/* 1. Interrupt is happen, ISR subsystem component flag was set by ISR_fired
 * handler
 *
 *
 *
 *
 *
 *
 *
 */
void ISR_fired(void *args)
{
    /*
    ISR_Component *ISR           = trigger;
    unsigned char  ISR_system_id = *(unsigned char *)args;
    bit_set(ISR->state.fired, ISR_system_id);
    */
}

willMount(ISR) {}

shouldUpdate(ISR)
{
    /* Check is all irs vectors enabled / disabled
     * by checking XOR of all interrupts = state->operation_hash
     * */
    return true;
}

willUpdate(ISR)
{
    /* Mount all state->is_processed == 0
     * mount release() to isr vector
     * */
    //  props->hw->isr->mount()
}

release(ISR)
{
    /* Fire all state->is_fired */
    /*
      for(unsigned int index = 0; index <= isr_vectors.used; index++) {
        union isr_vector_index vector_index;
        vector_index.index = isr_vectors.index[index];
        if((vector_index.pin.port ^ vector) == 0) {
            struct callback *isr_handler = isr_vectors.data[index];
            isr_handler->method(0, isr_handler->argument);
        }
    }
    */
}

didMount(ISR) {}

didUpdate(ISR) {}
