/*
 The MIT License (MIT)

 Copyright (c) 2017 Cynara Krewe

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

TEST_GROUP(Component_Invert_TestBench)
{
	OutPort<bool> outStimulus;
	Connection* outStimulusConnection;
	Invert<bool>* unitUnderTest;
	Connection* inResponseConnection;
	InPort<bool> inResponse;

	void setup()
	{
		unitUnderTest = new Invert<bool>();

		outStimulusConnection = connect(outStimulus, unitUnderTest->in);
		inResponseConnection = connect(unitUnderTest->out, inResponse);
	}

	void teardown()
	{
		disconnect(outStimulusConnection);
		disconnect(inResponseConnection);

		delete unitUnderTest;
	}
};

TEST(Component_Invert_TestBench, DormantWithoutStimulus)
{
	CHECK(!inResponse.peek());

	unitUnderTest->run();

	CHECK(!inResponse.peek());
}

TEST(Component_Invert_TestBench, FalseIsTrue)
{
	CHECK(outStimulus.send(false));

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	bool response;
	CHECK(inResponse.receive(response));

	bool expected = true;
	CHECK(response == expected);
}

TEST(Component_Invert_TestBench, TrueIsFalse)
{
	CHECK(outStimulus.send(true));

	CHECK(!inResponse.peek());

	unitUnderTest->run();

	bool response;
	CHECK(inResponse.receive(response));

	bool expected = false;
	CHECK(response == expected);
}
