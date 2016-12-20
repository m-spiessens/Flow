/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

#ifndef FLOW_COMPONENTS_H_
#define FLOW_COMPONENTS_H_

#include "flow.h"

template<typename Type>
class Invert
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out;
	void run() final override
	{
		Type b;
		if(in.receive(b))
		{
			out.send(!b);
		}
	}
};

template<typename From, typename To>
class Convert
:	public Flow::Component
{
public:
	Flow::InPort<From> inFrom;
	Flow::OutPort<To> outTo;
	void run() final override
	{
		From from;
		if(inFrom.receive(from))
		{
			outTo.send(static_cast<To>(from));
		}
	}
};

template<typename Type>
class Counter
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<unsigned int> out;
	Counter(unsigned int range)
	:	range(range)
	{}
	void run() final override
	{
		Type b;
		bool more = false;
		while(in.receive(b))
		{
			counter++;
			if(counter == range)
			{
				counter = 0;
			}
			more = true;
		}
		if(more)
		{
			out.send(counter);
		}
	}
private:
	unsigned int counter = 0;
	const unsigned int range;
};

template<typename Type>
class UpDownCounter
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<unsigned int> out;
	UpDownCounter(unsigned int downLimit, unsigned int upLimit, unsigned int startValue)
	:	counter(startValue),
		upLimit(upLimit),
		downLimit(downLimit)
	{}
	void run() final override
	{
		Type b;
		bool more = false;
		while(in.receive(b))
		{
			if(up)
			{
				counter++;
			}
			else
			{
				counter--;
			}

			if(counter == upLimit)
			{
				up = false;
			}
			else if(counter == downLimit)
			{
				up = true;
			}

			more = true;
		}

		if(more)
		{
			out.send(counter);
		}
	}
private:
	unsigned int counter;
	const unsigned int upLimit;
	const unsigned int downLimit;
	bool up = true;
};

template<typename Type, unsigned int outputs>
class Split
:	public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out[outputs];
	void run() final override
	{
		Type b;
		if(in.receive(b))
		{
			for(unsigned int i = 0; i < outputs; i++)
			{
				out[i].send(b);
			}
		}
	}
};

template<typename Type, unsigned int inputs>
class Combine
:	public Flow::Component
{
public:
	Flow::InPort<Type> in[inputs];
	Flow::OutPort<Type> out;
	void run() override
	{
		for(unsigned int i = 0; i < inputs; i++)
		{
			Type b;
			while(in[i].receive(b))
			{
				out.send(b);
			}
		}
	}
};

enum Tick
{
	TICK
};

class Timer
:	public Flow::Component
{
public:
	Flow::InPort<unsigned int> inPeriod;
	Flow::OutPort<Tick> outTick;

	void run();

private:
	unsigned int period = 0;
	unsigned int nextPeriod = 0;
	unsigned int sysTicks = 0;
};

class Toggle
:	public Flow::Component
{
public:
	Flow::InPort<Tick> tick;
	Flow::OutPort<bool> out;

	void run();

private:
	bool toggle = false;
};

#endif /* FLOW_COMPONENTS_H_ */
