/* The MIT License (MIT)
 *
 * Copyright (c) 2019 Cynara Krewe
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
#include <thread>

#include "CppUTest/TestHarness.h"

#include "flow/pool.h"
#include "flow/utility.h"

#include "flow_test/data.h"

using Flow::Pool;

const static unsigned int UNITS = 3;
const static unsigned int POOL_SIZE[UNITS] =
{ 1, 10, 1000 };

TEST_GROUP(Pool_TestBench)
{
	Pool<Data>* unitUnderTest[UNITS];
	Data** temporaryStore[UNITS];

	void setup()
	{
		for (unsigned int i = 0; i < UNITS; i++)
		{
			unitUnderTest[i] = new Pool<Data>(POOL_SIZE[i]);
			temporaryStore[i] = new Data*[POOL_SIZE[i]];
		}
	}

	void teardown()
	{
		for (unsigned int i = 0; i < UNITS; i++)
		{
			delete unitUnderTest[i];
			delete[] temporaryStore[i];
		}
	}
};

TEST(Pool_TestBench, CopyConstructor)
{
	Pool<char> other(1);
	other.take();
	Pool<char> some(2);
	Pool<char> copied(other);
	some = copied;
	CHECK(!other.haveAvailable());
}

TEST(Pool_TestBench, HaveAvailableAfterCreation)
{
	for (unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->haveAvailable());
	}
}

TEST(Pool_TestBench, TakeReleaseItem)
{
	for (unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->haveAvailable());
		Data* response = unitUnderTest[i]->take();
		CHECK(response != nullptr);

		CHECK(unitUnderTest[i]->release(*response));
		CHECK(unitUnderTest[i]->haveAvailable());
	}
}

TEST(Pool_TestBench, NoAvailable)
{
	for (unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->haveAvailable());

		for (unsigned int c = 0; c < POOL_SIZE[i]; c++)
		{
			temporaryStore[i][c] = unitUnderTest[i]->take();
			CHECK(temporaryStore[i][c] != nullptr);
		}

		// Pool should not have any more available.
		Data* response = unitUnderTest[i]->take();
		CHECK(response == nullptr);

		CHECK(!unitUnderTest[i]->haveAvailable());

		for (unsigned int c = 0; c < POOL_SIZE[i]; c++)
		{
			CHECK(unitUnderTest[i]->release(*temporaryStore[i][c]));
		}
	}
}

static Flow::Queue<Data*> recycle(1001);

static void take(Pool<Data>* pool, const unsigned long long count)
{
	unsigned long long c = 0;
	while (c < count)
	{
		Data* response = nullptr;
		while (response == nullptr)
		{
			response = pool->take();
		}

		recycle.enqueue(response);
		c++;
	}
}

static void release(Pool<Data>* pool, const unsigned long long count,
		bool* success)
{
	unsigned long long c = 0;
	while (c < count)
	{
		Data* response;
		if (recycle.dequeue(response))
		{
			*success = *success && pool->release(*response);

			c++;
		}
	}
}

TEST(Pool_TestBench, Threadsafe)
{
	for (unsigned int i = 0; i < UNITS; i++)
	{
		CHECK(unitUnderTest[i]->haveAvailable());

		const unsigned long long numberOfItems = 1000000;
		bool success = true;

		for (unsigned int t = 0; t < POOL_SIZE[i]; t++)
		{
			temporaryStore[i][t] = nullptr;
		}

		std::thread takeThread(take, unitUnderTest[i], numberOfItems);
		std::thread releaseThread(release, unitUnderTest[i], numberOfItems,
				&success);

		takeThread.join();
		releaseThread.join();

		CHECK(success);
		CHECK(recycle.isEmpty());

		CHECK(unitUnderTest[i]->haveAvailable());
	}
}
