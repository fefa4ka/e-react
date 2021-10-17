#include "component.h"

#ifdef REACT_PROFILER
    #include "hash.h"
    #include <execinfo.h>
unsigned int scope_index[128];
Component *  scope_buffer[128];

struct HAL_calls *current_scope = NULL;
struct HAL_calls  calls         = {0};

struct hash_table scope = {
    .index = scope_index,
    .data  = (void **)&scope_buffer,
    .size  = 128,
    .used  = 0,
};
extern clock_t  cpu_total = 0;
static uint64_t steps     = 0;
uint64_t        step()
{
    steps += 1;
    if (steps == 0) {
        return 1;
    }
    return steps;
}
bool stop = false;
void sighandler(int sig) { stop = true; }

unsigned int frame_depth()
{
    const unsigned int max_depth = 200;
    void *             buffer[max_depth];
    return backtrace(buffer, max_depth) - 3;
}


    #define dump_call(stage, call)                                             \
        if(component->calls.stage.call) printf("\t   %lld " #call "\n", component->calls.stage.call)

    #define dump_stage(stage)                                                  \
        if(component->counter.stage) {     \
        printf("\t" #stage "\t%lld\t%lld\t%0.2lf%%\n",                         \
               component->counter.stage, component->cpu.stage,                 \
               ((double)component->cpu.stage / (double)cpu_total) * 100);       \
            dump_call(stage, gpio_in);                                         \
        dump_call(stage, gpio_out);                                            \
        dump_call(stage, gpio_on);                                             \
        dump_call(stage, gpio_off);                                            \
        dump_call(stage, gpio_flip);                                           \
        dump_call(stage, gpio_pullup);                                         \
        dump_call(stage, gpio_get); \
        dump_call(stage, adc_mount); \
        dump_call(stage, adc_selectChannel); \
        dump_call(stage, adc_startConvertion); \
        dump_call(stage, adc_isConvertionReady); \
        dump_call(stage, adc_readConvertion); \
        dump_call(stage, uart_init); \
        dump_call(stage, uart_isDataReceived); \
        dump_call(stage, uart_isTransmitReady); \
        dump_call(stage, uart_transmit); \
        dump_call(stage, uart_receive); \
        dump_call(stage, timer_init); \
        dump_call(stage, timer_get); \
        dump_call(stage, timer_set); \
        dump_call(stage, timer_off); \
        dump_call(stage, timer_usFromTicks); \
        }


bool dump_usage()
{
    enum error r;
    Component *component;
    printf("\n\nCPU usage details:\n\n");
    printf("\toperation\tsteps\tcpu\t%\n\n");
    printf("\t%d components\t%lld\t%lld\t100%%\n\n", scope.used, step() - 1,
           cpu_total);
    for (unsigned int index = 0; index < scope.used; index++) {
        component = (Component *)scope.data[index];
        clock_t component_cpu_total
            = (component->cpu.willMount + component->cpu.nextProps
               + component->cpu.shouldUpdate + component->cpu.willUpdate
               + component->cpu.release + component->cpu.didMount
               + component->cpu.didUpdate + component->cpu.didUnmount);
        double component_cpu_percent
            = ((double)component_cpu_total / (double)cpu_total) * 100;
        printf("%d: ", index);
        printf("%s\n\n", component->name);
        printf("\tCPU Usage\t \t%ld\t%0.2lf%%\n\n", component_cpu_percent,
               component_cpu_total);
        dump_stage(willMount);
        dump_stage(nextProps);
        dump_stage(shouldUpdate);
        dump_stage(willUpdate);
        dump_stage(release);
        dump_stage(didMount);
        dump_stage(didUnmount);
        printf("\n\n");
    }

    return stop;
}


unsigned int hash_component(char *word)
{
    unsigned long hash = 0;
    if (!word) {
        return 0;
    }
    while (*word != '\0') {
        hash += *word++;
    }

    return (hash % MAX_TABLE_SIZE);
}
#endif


bool Stage_Component(Component *instance, void *next_props)
{
    if (instance->stage == REACT_STAGE_RELEASED
        && instance->ShouldUpdate(instance, next_props)) {
        instance->stage = REACT_STAGE_PREPARED;
        instance->WillUpdate(instance, next_props);
    } else if (instance->stage == REACT_STAGE_PREPARED) {
        instance->stage = REACT_STAGE_RELEASED;
        instance->Release(instance);
        instance->DidUpdate(instance);
    } else if (instance->stage == REACT_STAGE_DEFINED) {
#ifdef REACT_PROFILER
        hash_write(&scope, hash_component(instance->name), (void **)instance);
#endif
        instance->WillMount(instance, next_props);
        instance->Release(instance);
        instance->DidMount(instance);
        instance->stage = REACT_STAGE_RELEASED;
    } else if (instance->stage == REACT_STAGE_UNMOUNTED) {
        instance->stage = REACT_STAGE_BLOCKED;
        instance->DidUnmount(instance);
    }

    return true;
}

bool React_Component(Component *instance, void *next_props)
{
    if (instance->ShouldUpdate(instance, next_props)) {
        instance->stage = REACT_STAGE_PREPARED;
        instance->WillUpdate(instance, next_props);
        instance->stage = REACT_STAGE_RELEASED;
        instance->Release(instance);
        instance->DidUpdate(instance);
    }

    if (instance->stage == REACT_STAGE_UNMOUNTED) {
        instance->stage = REACT_STAGE_BLOCKED;
        instance->DidUnmount(instance);
    }

    return true;
}
