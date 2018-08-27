/* The MIT License (MIT)
 *
 * Copyright (c) 2018 Cynara Krewe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software, hardware and associated documentation files (the "Solution"), to deal
 * in the Solution without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Solution, and to permit persons to whom the Solution is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Solution.
 *
 * THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 * SOLUTION.
 */

#include <stdint.h>

#include "CppUTest/TestHarness.h"

#include "flow/components.h"
#include "flow/reactor.h"

#include "flow_test/data.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_Counter_TestBench)
{
	OutPort<char> outStimulus;
	Connection* outStimulusConnection;
	Counter<char>* unitUnderTest;
	Connection* inResponseConnection;
	InPort<unsigned int> inResponse{&dummyComponent};

	void setup()
	{
		unitUnderTest = new Counter<char>(10);

		outStimulusConnection = connect(outStimulus, unitUnderTest->in, 5);
		inResponseConnection = connect(unitUnderTest->out, inResponse);
	}

	void teardown()
	{
		disconnect(outStimulusConnection);
		disconnect(inResponseConnection);

		delete unitUnderTest;

		Flow::Reactor::reset();
	}
};

TEST(Component_Counter_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());
}

TEST(Component_Counter_TestBench, Counting)
{
	CHECK(outStimulus.send(0));

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	unsigned int response = 0;
	CHECK(inResponse.receive(response));

	unsigned int expected = 1;
	CHECK(response == expected);

	unitUnderTest->run();

	CHECK(!inResponse.receive(response));

	CHECK(outStimulus.send(123));

	unitUnderTest->run();

	CHECK(inResponse.receive(response));

	expected = 2;
	CHECK(response == expected);

	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));

	unitUnderTest->run();

	CHECK(inResponse.receive(response));

	expected = 7;
	CHECK(response == expected);

	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));
	CHECK(outStimulus.send(123));

	unitUnderTest->run();

	CHECK(inResponse.receive(response));

	expected = 2;
	CHECK(response == expected);
}
