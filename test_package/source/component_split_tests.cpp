/* The MIT License (MIT)
 *
 * Copyright (c) 2020 Cynara Krewe
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

#include "components.h"
#include "reactor.h"

#include "data.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

TEST_GROUP(Component_Split_TestBench)
{
	constexpr static unsigned int SPLIT_COUNT = 5;

	OutPort<char> outStimulus;
	Connection* outStimulusConnection;
	Split<char, SPLIT_COUNT>* unitUnderTest;
	Connection* inResponseConnection[SPLIT_COUNT];
	InPort<char>* inResponse[SPLIT_COUNT];

	void setup()
	{
		unitUnderTest = new Split<char, SPLIT_COUNT>();

		outStimulusConnection = connect(outStimulus, unitUnderTest->in);

		for (unsigned int i = 0; i < SPLIT_COUNT; i++)
		{
			inResponse[i] = new InPort<char>{ nullptr };
			inResponseConnection[i] = connect(unitUnderTest->out[i],
					inResponse[i]);
		}
	}

	void teardown()
	{
		delete outStimulusConnection;

		for (unsigned int i = 0; i < SPLIT_COUNT; i++)
		{
			delete inResponseConnection[i];
			delete inResponse[i];
		}

		delete unitUnderTest;

		Flow::Reactor::reset();
	}
};

TEST(Component_Split_TestBench, DormantWithoutStimulus)
{
	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(!inResponse[i]->peek());
	}

	unitUnderTest->run();

	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(!inResponse[i]->peek());
	}
}

TEST(Component_Split_TestBench, Split)
{
	CHECK(outStimulus.send(1));

	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(!inResponse[i]->peek());
	}

	unitUnderTest->run();

	char response = 0;
	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(inResponse[i]->receive(response));

		char expected = 1;
		CHECK(response == expected);
	}

	unitUnderTest->run();

	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(!inResponse[i]->receive(response));
	}

	CHECK(outStimulus.send(123));

	unitUnderTest->run();

	for (unsigned int i = 0; i < SPLIT_COUNT; i++)
	{
		CHECK(inResponse[i]->receive(response));

		char expected = 123;
		CHECK(response == expected);
	}
}
