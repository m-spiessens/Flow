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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"

Flow::OutPort<char> Debug::outPort;

static const unsigned int bufferSize = 20;

void Debug::printf(const char *format, ...)
{
	va_list variableArguments;

	va_start(variableArguments, format);

	char* buffer = new char[bufferSize];

	vsnprintf(buffer, bufferSize, format, variableArguments);

	for(unsigned int i = 0; (i < strlen(buffer)) && (i < bufferSize); i++)
	{
		outPort.send(buffer[i]);
	}

	delete buffer;

	va_end(variableArguments);
}
