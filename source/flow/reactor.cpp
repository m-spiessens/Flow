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

#include "flow/platform.h"
#include "flow/reactor.h"

Flow::Reactor& Flow::Reactor::theOne()
{
	static Reactor me;
	return me;
}

void Flow::Reactor::add(Component& component)
{
	if(first == nullptr && last == nullptr)
	{
		first = &component;
		last = &component;
	}
	else
	{
		last->next = &component;
		last = &component;
	}
}

void Flow::Reactor::run()
{
	bool ranSomething = false;
	Component* current = first;
	while(current != nullptr)
	{
		if(current->tryRun())
		{
			ranSomething = true;
		}

		current = current->next;
	}

	if(!ranSomething)
	{
		Platform::waitForEvent();
	}
}

Flow::Reactor::Reactor()
{
}
