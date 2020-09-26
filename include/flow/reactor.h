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

#ifndef FLOW_REACTOR_H_
#define FLOW_REACTOR_H_

#include "flow.h"

/**
 * \brief Flow is a pipes and filters implementation tailored for
 * (but not exclusive to) microcontrollers.
 */
namespace Flow
{

class Reactor
{
public:
	/**
	 * \brief Add a component to the Flow::Reactor for potential running when needed.
	 *
	 * DO NOT call this function manually unless you know what you're doing.
	 * A Flow::Component will automatically be added to the Flow::Reactor on creation.
	 *
	 * \param component The component that will be taken care of.
	 */
	static void add(Component& component);

	/**
	* \brief Let the Flow::Reactor perform second stage initialization of
	* all Flow::Component of the application.
	*
	* \remark Must be called before run().
	*/
	static void start();

	/**
	* \brief Symmetrical deinitialization, see start().
	*/
	static void stop();

	/**
	* \brief Let the Flow::Reactor do its job.
	*
	* Putting this in a while(true) in the main() is a typical scenario on a microcontroller.
	* If the Flow::Reactor does not find any component that need to be run it will call the
	* Flow::Platform::waitForEvent() function.
	*/
	static void run();

	/**
	 * \brief Drop all registered components.
	 */
	static void reset();

private:
	Reactor();

	/**
	* \brief Get the singleton instance.
	*
	* Yes, a singleton is actually useful here.
	*
	* \return The singleton instance.
	*/
	static Reactor& theOne();

	Component* first = nullptr;
	Component* last = nullptr;

	bool running = false;
};

} //namespace Flow

#endif /* FLOW_REACTOR_H_ */
