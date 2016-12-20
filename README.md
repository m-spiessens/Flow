# Flow

Flow is a part of Cynara to design applications with. Flow is a pipes and filters implementation tailored for microcontrollers. It provides 3 base concepts: component, port and connection.

## Component

A component provides application functionality, it is a filter from the pipes and filters design pattern.

## Port

A component can have in- and/or output port(s). A typical component will receive data from its input port(s), process data and send data to its output port(s).

## Connection

An output port can be connected to an input port. A connection can behave as a queue, allowing multiple data element to be buffered. Or a connection can connect a constant data element to an input port. Connections are pipes from the pipes and filters design pattern.

## Blinky

Blinky is a minimal example application that performs blinking of a LED. Blinky is composed of 3 components: a timer, toggling and a digital output. The timer will generate a tick every configured period. The toggling will consume the tick and invert an internal boolean value. The boolean value is sent to the digital output, which has a LED attached.

First of all the three components are created:

```C++
// Create the components of the application.
Timer* timer = new Timer();
Toggle* periodToggle = new Toggle();
DigitalOutput* led = new DigitalOutput(Gpio::Name{Gpio::Port::F, 0});

```

The components (actually the ports of the components) are connected so they can interact:

```
// Connect the components of the application.
Flow::Connection* connections[] =
{
	// Configure the timer with a fixed period of 250 ms.
	Flow::connect(250u, timer->inPeriod),
	Flow::connect(timer->outTick, periodToggle->tick),
	Flow::connect(periodToggle->out, led->inValue)
};
```

The components must be deployed. The timer is deployed in the SysTick context. While the toggle and digital output is deployed in the super loop (main loop).

```
// Define the deployment of the components.

Flow::Component* sysTickComponents[] =
{
	timer
};

Flow::Component* mainComponents[] =
{
	periodToggle,
	led
};
```
The full example can be found as [blinky in the Cynara repository](https://github.com/CynaraKrewe/Cynara/tree/master/Software/blinky).
