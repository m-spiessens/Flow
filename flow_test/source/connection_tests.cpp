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
#include <thread>

#include "CppUTest/TestHarness.h"

#include "flow/flow.h"

#include "flow_test/data.h"

using Flow::ConnectionFIFO;
using Flow::OutPort;
using Flow::InPort;

const static unsigned int UNITS = 3;
const static unsigned int CONNECTION_FIFO_SIZE[UNITS] = {1, 10, 255};

TEST_GROUP(ConnectionFIFO_TestBench)
{
	ConnectionFIFO<Data>* unitUnderTest[UNITS];
	OutPort<Data> sender[UNITS];
	InPort<Data> receiver[UNITS];

	void setup()
	{
		for(unsigned int i = 0; i < UNITS; i++)
		{
			unitUnderTest[i] = new Flow::ConnectionFIFO<Data>(sender[i], receiver[i], CONNECTION_FIFO_SIZE[i]);
		}
	}

	void teardown()
	{
		for(unsigned int i = 0; i < UNITS; i++)
		{
			delete unitUnderTest[i];
		}
	}
};

TEST(ConnectionFIFO_TestBench, IsEmptyAfterCreation)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(!unitUnderTest[i]->peek());
		Data response;
		CHECK(!unitUnderTest[i]->receive(response));
	}
}

TEST(ConnectionFIFO_TestBench, SendReceiveItem)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(!unitUnderTest[i]->peek());
		Data stimulus = Data(123, true);
		CHECK(unitUnderTest[i]->send(stimulus));
		CHECK(unitUnderTest[i]->peek());
		Data response;
		CHECK(unitUnderTest[i]->receive(response));
		CHECK(stimulus == response);
		CHECK(!unitUnderTest[i]->peek());
		CHECK(!unitUnderTest[i]->receive(response));
	}
}

TEST(ConnectionFIFO_TestBench, FullConnection)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		// Connection should be empty.
		CHECK(!unitUnderTest[i]->peek());

		for(unsigned int c = 0; c < (CONNECTION_FIFO_SIZE[i] - 1); c++)
		{
			Data stimulus = Data(c, true);
			// Connection should accept another item.
			CHECK(unitUnderTest[i]->send(stimulus));

			// Connection should not be empty.
			CHECK(unitUnderTest[i]->peek());
		}

		Data lastStimulus = Data(CONNECTION_FIFO_SIZE[i], false);
		// Connection should accept another item.
		CHECK(unitUnderTest[i]->send(lastStimulus));

		// Connection should not be empty.
		CHECK(unitUnderTest[i]->peek());

		// Connection shouldn't accept any more items.
		CHECK(!unitUnderTest[i]->send(lastStimulus));

		Data response;

		for(unsigned int c = 0; c < (CONNECTION_FIFO_SIZE[i] - 1); c++)
		{
			// Should get another item from the Connection.
			CHECK(unitUnderTest[i]->receive(response));

			// Item should be the expected.
			Data expectedResponse = Data(c, true);
			CHECK(response == expectedResponse);

			// Connection should not be empty.
			CHECK(unitUnderTest[i]->peek());
		}

		// Should get another item from the Connection.
		CHECK(unitUnderTest[i]->receive(response));

		// Item should be the expected.
		CHECK(lastStimulus == response);

		// Connection should be empty.
		CHECK(!unitUnderTest[i]->peek());

		// Shouldn't get another item from the Connection.
		CHECK(!unitUnderTest[i]->receive(response));
	}
}

static void producer(ConnectionFIFO<Data>* _unitUnderTest, const unsigned long long count)
{
	for(unsigned long long c = 0; c <= count; c++)
	{
		while(!_unitUnderTest->send(Data(c, ((c % 2) == 0))));
	}
}

static void consumer(ConnectionFIFO<Data>* _unitUnderTest, const unsigned long long count, bool* success)
{
	unsigned long long c = 0;

	while(c <= count)
	{
		Data response;
		if(_unitUnderTest->receive(response))
		{
			Data expected = Data(c, ((c % 2) == 0));
			*success = *success && (response == expected);
			c++;
		}
	}
}

TEST(ConnectionFIFO_TestBench, Threadsafe)
{
	for(unsigned int i = 0; i < UNITS; i++)
	{
		// Connection should be empty.
		CHECK(!unitUnderTest[i]->peek());

		const unsigned long long count = 1000000;
		bool success = true;

		std::thread producerThread(producer, unitUnderTest[i], count);
		std::thread consumerThread(consumer, unitUnderTest[i], count, &success);

		producerThread.join();
		consumerThread.join();

		CHECK(success);

		// Connection should be empty.
		CHECK(!unitUnderTest[i]->peek());
	}
}

using Flow::ConnectionConstant;

TEST_GROUP(ConnectionConstant_TestBench)
{
	ConnectionConstant<Data>* unitUnderTest;
	InPort<Data> receiver;

	void setup()
	{
		unitUnderTest = new Flow::ConnectionConstant<Data>(Data{123, true}, receiver);
	}

	void teardown()
	{
		delete unitUnderTest;
	}
};

TEST(ConnectionConstant_TestBench, Constant)
{
	for(volatile unsigned int i = 0; i < 1000; i++)
	{
		// Should always have a value.
		CHECK(unitUnderTest->peek());

		Data response;
		CHECK(unitUnderTest->receive(response));
		Data expected{123, true};
		CHECK(response == expected);
	}
}
