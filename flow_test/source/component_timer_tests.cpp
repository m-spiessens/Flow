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

#include <stdint.h>

#include "CppUTest/TestHarness.h"

#include "flow/components.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_Timer_TestBench)
{
	OutPort<unsigned int> outStimulus;
	Connection* outStimulusConnection;
	Timer* unitUnderTest;
	Connection* inResponseConnection;
	InPort<Tick> inResponse;

	void setup()
	{
		unitUnderTest = new Timer();

		outStimulusConnection = connect(outStimulus, unitUnderTest->inPeriod);
		inResponseConnection = connect(unitUnderTest->outTick, inResponse);
	}

	void teardown()
	{
		delete outStimulusConnection;
		delete inResponseConnection;

		delete unitUnderTest;
	}
};

TEST(Component_Timer_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());
}

TEST(Component_Timer_TestBench, TickPeriod100)
{
	CHECK(!inResponse.peek());

	unsigned int period = 100;

	CHECK(outStimulus.send(period));

	for(unsigned int i = 0; i < period - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	Tick tick;
	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < period - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < period - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);
}

TEST(Component_Timer_TestBench, TickPeriodChange)
{
	CHECK(!inResponse.peek());

	unsigned int firstPeriod = 100;

	CHECK(outStimulus.send(firstPeriod));

	for(unsigned int i = 0; i < firstPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	Tick tick;
	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	unsigned int secondPeriod = 50;

	CHECK(outStimulus.send(secondPeriod));

	for(unsigned int i = 0; i < firstPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < secondPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < secondPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	unsigned int thirdPeriod = 500;

	CHECK(outStimulus.send(thirdPeriod));

	for(unsigned int i = 0; i < secondPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < thirdPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);

	for(unsigned int i = 0; i < thirdPeriod - 1; i++)
	{
		unitUnderTest->run();
	}

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(inResponse.receive(tick));
	CHECK(tick == TICK);
}
