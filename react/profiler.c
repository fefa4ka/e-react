#include "hash.h"
#include "profiler.h"
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
    if (stop) {
        dump_usage();
    } else {
        steps += 1;
        if (steps == 0) {
            return 1;
        }
    }
    return !stop;
}
bool stop = false;
void sighandler(int sig) { stop = true; }

unsigned int frame_depth()
{
    const unsigned int max_depth = 200;
    void *             buffer[max_depth];
    return backtrace(buffer, max_depth) - 3;
}


#define dump_call(stage, call)                                                 \
    if ((component)->calls.stage.call)                                           \
    printf("   " #call " \t \t%lld\n", component->calls.stage.call)

#define dump_stage(stage)                                                      \
    if (component->counter.stage) {                                            \
        printf(" " #stage " \t \t%lld\t%lld\t%0.2lf%%\n",                      \
               component->counter.stage, component->cpu.stage,                 \
               ((double)component->cpu.stage / (double)cpu_total) * 100);      \
        dump_call(stage, gpio_in);                                             \
        dump_call(stage, gpio_out);                                            \
        dump_call(stage, gpio_on);                                             \
        dump_call(stage, gpio_off);                                            \
        dump_call(stage, gpio_flip);                                           \
        dump_call(stage, gpio_pullup);                                         \
        dump_call(stage, gpio_get);                                            \
        dump_call(stage, adc_mount);                                           \
        dump_call(stage, adc_selectChannel);                                   \
        dump_call(stage, adc_startConvertion);                                 \
        dump_call(stage, adc_isConvertionReady);                               \
        dump_call(stage, adc_readConvertion);                                  \
        dump_call(stage, uart_init);                                           \
        dump_call(stage, uart_isDataReceived);                                 \
        dump_call(stage, uart_isTransmitReady);                                \
        dump_call(stage, uart_transmit);                                       \
        dump_call(stage, uart_receive);                                        \
        dump_call(stage, timer_init);                                          \
        dump_call(stage, timer_get);                                           \
        dump_call(stage, timer_set);                                           \
        dump_call(stage, timer_off);                                           \
        dump_call(stage, timer_usFromTicks);                                   \
    }


bool dump_usage()
{
    enum error r;
    Component *component;

    printf("\n\nCPU usage details:\n\n");
    printf("operation\t\tsteps\tcpu\t%\n\n");
    printf("%d components\t\t%lld\t%lld\t100%%\n\n", scope.used, steps,
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
        printf("%s\n", component->name);
        dump_stage(willMount);
        dump_stage(nextProps);
        dump_stage(shouldUpdate);
        dump_stage(willUpdate);
        dump_stage(release);
        dump_stage(didMount);
        dump_stage(didUnmount);
        printf("\t\t\t\t%lld\t%0.2lf%%\n", component_cpu_total,
               component_cpu_percent);
        printf("\n\n");
    }

    vcd_clean();

    return stop;
}

const char *int_to_binary_str(int x, int N_bits)
{
    static char b[512];
    char *      p = b;
    b[0]          = '\0';

    for (int i = (N_bits - 1); i >= 0; i--) {
        *p++ = (x & (1 << i)) ? '1' : '0';
        if (!(i % 4))
            *p++ = ' ';
    }
    return b;
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