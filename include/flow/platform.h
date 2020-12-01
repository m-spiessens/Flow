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

#ifndef FLOW_PLATFORM_H_
#define FLOW_PLATFORM_H_

#include <stdint.h>
#include <signal.h>

/**
 * \brief Flow is a pipes and filters implementation tailored for
 * (but not exclusive to) microcontrollers.
 */
namespace Flow
{

class Platform
{
public:
	/**
	 * \brief Platform specific configuration that might be needed for the implementation of waitForEvent().
	 */
	static void configure();

	/**
	 * \brief When Flow::Reactor does not find any components that need to be run()
	 * this function will be executed.
	 *
	 * On a ARM Cortex M3 or M4 executing a WFI or WFE assembler instruction might be useful.
	 */
	static void waitForEvent();

	/**
	 * \brief Atomically increment a value.
	 *
	 * When using the GNU C Compiler (gcc) the __atomic_fetch_add intrinsic can be used.
	 * On a bare metal system without LDREX/STREX instructions the base priority mask
	 * could be used to prevent interrupts during the read-modify-write.
	 *
	 * \param value The value to be incremented.
	 * \param increment The step of the increment.
	 */
	static void atomic_fetch_add(volatile sig_atomic_t* value, uint_fast8_t increment);
};

} //namespace Flow

#endif /* FLOW_PLATFORM_H_ */
