# Flow {#mainpage}

[![Build Status](https://travis-ci.com/CynaraKrewe/Flow.svg?branch=master)](https://travis-ci.com/CynaraKrewe/Flow)
[![Coverage Status](https://coveralls.io/repos/github/CynaraKrewe/Flow/badge.svg?branch=master)](https://coveralls.io/github/CynaraKrewe/Flow?branch=master)

[We have conan packages!](https://bintray.com/cynarakrewe/CynaraConan)

Flow is a pipes and filters implementation tailored for (but not exclusive to) ARM Cortex-M microcontrollers. It provides 3 concepts: component, port and connection.
A minimal cooperative scheduler helps you build low-power, reactive applications with ease.

## Concepts

### Component

A component provides application functionality, it is a filter from the pipes and filters design pattern. A component implements the ```Flow::Component``` interface and the resulting ```void run()``` method defines it's complete behavior.

### Port

A component can have in- and/or output port(s). A typical component will receive data from its input port(s), process data and send data to its output port(s). Flow provides ```Flow::InPort<DataType>``` and ```Flow::OutPort<DataType>``` templates to support every possible datatype.

### Connection

Connections are pipes from the pipes and filters design pattern. An output port can be connected to an input port. A connection can behave as a queue, allowing multiple data element to be buffered. One output port can be connected to one input port, one-to-many or many-to-one connections are not supported. One-to-many or many-to-one can achieved by using components that implement split/tee or zip/combine behavior for example. Connections are perfectly safe from race conditions when the connected components run concurrently.

## Reactive

Systems using microcontrollers are typically reactive systems, they respond to events.
That is were the Flow::Reactor comes in to play.
The idea is to define a reaction to an event by making a pipeline of multiple Flow::Component.
The head (first Flow::Component) of the pipeline will run in interrupt context (an event).
The head sends data to the second component in the pipeline over a (concurrency safe) Flow::Connection.
Which will schedule the second component to be run by the Flow::Reactor when possible.
All components except for the head will be handled by the Flow::Reactor and be executed in main() context, keeping the interrupt service routines short.

After the complete pipeline was run and no other events need to be serviced, the Flow::Reactor will execute the Flow::Platform::waitForEvent() function.
The implementation of this function can be freely defined.
On low power Cortex-M3 or -M4 based system the WFE (wait for event) instruction can be executed.
Depending on the configuration the microcontroller will go into sleep mode untill it is woken up by the next event (interrupt) to which it will respond.

### Conclusion

Given that all pipelines are triggered by events (head component in interrupt service routine), the Flow::Reactor provides a convenient mechanism to build a low power enabled, reactive system.

### Scheduling

The Flow::Reactor is a cooperative scheduler. Except for the head component of a pipeline, which runs in interrupt context, there is no preemption.

The current scheduling strategy is a simple round robin. 

Priority could be introduced by changing the Flow::Reactor::run() implementation. After finding a Flow::Component that had to be run (and running it) the reactor continues in the list. If the reactor would start from the start of the list after running a component, the order in which Flow::Components are created will define their priority for scheduling. This way different data paths could have different priorities. 

A few different strategies might be a future extension.

## Example

### Blinky

Blinky is a minimal example application that performs blinking of a LED. It runs on a [EK-TM4CEXL launchpad](http://www.ti.com/tool/EK-TM4C129EXL) with cooperative scheduling.

Blinky is composed of 3 components: a timer, toggling and a digital output. The timer will generate a tick every configured period. The toggling will consume the tick and invert an internal boolean value. The boolean value is sent to the digital output, which has a LED attached.

First of all the three components are created (these implement the ```Flow::Component``` interface):

```cpp
// Create the components of the application.
Timer timer{100 /*ms*/};
Toggle toggle;
Digital::Output led{Pin::Port::N, 1, Digital::Polarity::Normal};

```

The components (actually the ports of the components) are connected so they can interact:

```cpp
// Connect the components of the application.
Flow::connect(timer.outTick, toggle.tick);
Flow::connect(toggle.out, led.inState);
```

Start the Flow::Reactor and have it schedule the components: 

```cpp
Flow::Reactor::start();

// Run the application.
while(true)
{
	Flow::Reactor::run();
}
```
The full example can be found at [FlowBlinky](https://github.com/CynaraKrewe/FlowBlinky).
A more complex example with PWM, UART, USB and ADC can be found at [FlowTM4C](https://github.com/CynaraKrewe/FlowTM4C).
