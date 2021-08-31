# e-react

e-react is a C99 library for building mcu applications.

* **Declarative**: Design simple components for each subsystem, and compose system using this functional components.

**Examples** of:
* [Applications](/app)
* [Components](/component)

## Component Lifecycle
If component used by `apply(...)` or `use(...)` in `loop(...) { ... }` on each loop step preparation or release step of a component will executed.

Whole lifecycle will executed if component used by `react(...)` macros.

```
 0. Mounting step
┌───────────┐   ┌─────────┐    ┌──────────┐
│ willMount ├──►│ release ├───►│ didMount │
└───────────┘   └─────────┘    └──────────┘

 1. Preparation step                       2. Release step
┌────────────────────────────────────┐
│ ┌───────────◄──┐   ┌────────────┐  │     ┌─────────┐     ┌───────────┐
│ │ shouldUpdate ├──►│ willUpdate ├──┼────►│ release ├────►│ didUpdate │
│ └──────────────┘   └────────────┘  │     └─────────┘     └───────────┘
└────────────────────────────────────┘

 3. Unmounting step
                       ┌────────────┐
  stage: UNMOUNTED ───►│ didUnmount ├───► stage: BLOCKED
                       └────────────┘
```
## Event-loop

```
/* Component initialization
Component(component, _({
    .property = value,
    ...
}))

/* Infinity loop */
loop(component, ...) {
    use(component);
    apply(type, component, _({
        .property = value
    }))
}
```

## HAL
Hardware Architecture Layer consist of handlers for:
* GPIO
* ADC
* Timer
* UART
* SPI

HAL available for:
* STM8
* AVR
* x86
* ESP8266 (in progress)
* RPi (in progress)

