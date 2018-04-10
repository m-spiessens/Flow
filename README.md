# Flow {#mainpage}

[![Build Status](https://travis-ci.org/CynaraKrewe/Flow.svg?branch=master)](https://travis-ci.org/CynaraKrewe/Flow)
[![Coverage Status](https://coveralls.io/repos/github/CynaraKrewe/Flow/badge.svg)](https://coveralls.io/github/CynaraKrewe/Flow)

[We have conan packages!](https://bintray.com/cynarakrewe/CynaraConan)

Flow is a pipes and filters implementation tailored for (but not exclusive to) microcontrollers. It provides 3 concepts: component, port and connection.

## Concepts

### Component

A component provides application functionality, it is a filter from the pipes and filters design pattern. A component implements the ```Flow::Component``` interface and the resulting ```void run()``` method defines it's complete behavior.

### Port

A component can have in- and/or output port(s). A typical component will receive data from its input port(s), process data and send data to its output port(s). Flow provides ```Flow::InPort<DataType>``` and ```Flow::OutPort<DataType>``` templates to support every possible datatype.

### Connection

Connections are pipes from the pipes and filters design pattern. An output port can be connected to an input port. A connection can behave as a queue, allowing multiple data element to be buffered. One output port can be connected to one input port, one-to-many or many-to-one connections are not supported. One-to-many or many-to-one can achieved by using components that implement split/tee or zip/combine behavior for example. Connections are perfectly safe from race conditions when the connected components run concurrently.

## Examples

### Blinky

Blinky is a minimal example application that performs blinking of a LED. It runs on a microcontroller with cooperative scheduling.

Blinky is composed of 3 components: a timer, toggling and a digital output. The timer will generate a tick every configured period. The toggling will consume the tick and invert an internal boolean value. The boolean value is sent to the digital output, which has a LED attached.

First of all the three components are created (these implement the ```Flow::Component``` interface):

```
// Create the components of the application.
Timer* timer = new Timer(100 /*ms*/);
Toggle* toggle = new Toggle();
DigitalOutput* led = new DigitalOutput(Gpio::Name{ Gpio::Port::N, 1 });

```

The components (actually the ports of the components) are connected so they can interact:

```
// Connect the components of the application.
Flow::Connection* connections[] =
{
	Flow::connect(timer->outTick, toggle->tick),
	Flow::connect(toggle->out, led->inValue)
};
```

The components must be deployed. The timer is deployed in the SysTick context. While the toggle and digital output is deployed in the super loop (main loop).

```
// Define the deployment of the components.
Flow::Component* mainComponents[] =
{
	toggle,
	led
};

Flow::Component* sysTickComponents[] =
{
	timer
};
```
The full example can be found as [FlowBlinky](https://github.com/CynaraKrewe/FlowBlinky).

### Ping pong (C++ threads, OS)

Ping pong is composed of 2 components. One component will ping the other, the other component will react by sending a pong back.

```C++
enum class Ping
{
	PING
};

enum class Pong
{
	PONG
};

class PingPlayer: public Flow::Component
{
public:
	Flow::InPort<Pong> inPong;
	Flow::OutPort<Ping> outPing;

	void run()
	{
		while (!outPing.send(Ping::PING))
			;
		std::cout << "Sending ping!" << std::endl;

		Pong pong;
		while (!inPong.receive(pong))
			;
		std::cout << "Received pong!" << std::endl;
	}
};

class PongPlayer: public Flow::Component
{
public:
	Flow::InPort<Ping> inPing;
	Flow::OutPort<Pong> outPong;

	void run()
	{
		Ping ping;
		while (!inPing.receive(ping))
			;

		while (!outPong.send(Pong::PONG))
			;
	}
};
```

The PongPlayer is deployed in it's own thread:

```C++
static void pongThreadJob(Flow::Component** components,
		unsigned int componentCount, const unsigned int cycles)
{
	unsigned int count = 0;
	while (count < cycles)
	{
		for (unsigned int c = 0; c < componentCount; c++)
		{
			components[c]->run();
		}

		count++;
	}
}
```

The application defines the components and connects them. Next deployment can be defined. Running the application is very straightforward: call ```run()``` of all the components.

```C++
int main(void)
{
	// Create the components of the application.
	PingPlayer* pingPlayer = new PingPlayer();
	PongPlayer* pongPlayer = new PongPlayer();

	// Connect the components of the application.
	Flow::Connection* connections[] =
	{
		Flow::connect(pingPlayer->outPing, pongPlayer->inPing),
		Flow::connect(pongPlayer->outPong, pingPlayer->inPong)
	};

	// Define the deployment of the components.
	Flow::Component* mainComponents[] =
	{
		pingPlayer
	};

	Flow::Component* threadComponents[] =
	{
		pongPlayer
	};

	// Run the application.

	const unsigned int cycles = 10;

	std::thread pongThread(pongThreadJob, threadComponents,
			ArraySizeOf(threadComponents), cycles);

	unsigned int count = 0;
	while (count < cycles)
	{
		for (unsigned int c = 0; c < ArraySizeOf(mainComponents); c++)
		{
			mainComponents[c]->run();
		}

		count++;
	}

	pongThread.join();

	// Disconnect the components of the application.
	for (unsigned int i = 0; i < ArraySizeOf(connections); i++)
	{
		delete connections[i];
	}

	// Clean up application components.
	for (unsigned int i = 0; i < ArraySizeOf(mainComponents); i++)
	{
		delete mainComponents[i];
	}

	for (unsigned int i = 0; i < ArraySizeOf(threadComponents); i++)
	{
		delete threadComponents[i];
	}

	return 0;
}
```

At the end all connections and components are cleaned up.
