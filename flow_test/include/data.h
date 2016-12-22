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

#ifndef DATA_H_
#define DATA_H_

#include <stdint.h>

class Data
{
public:
	Data()
	:	integer(0),
	 	boolean(false)
	{}
	Data(uint64_t i, bool b)
	:	integer(i),
		boolean(b)
	{}
	uint64_t integer;
	bool boolean;
	bool operator==(const Data& other) const
	{
		bool equal = true;
		equal = equal && (this->integer == other.integer);
		equal = equal && (this->boolean == other.boolean);
		return equal;
	}
	bool operator!=(const Data& other) const
	{
		return !(*this == other);
	}
	bool operator<(const Data& other) const
	{
		return this->integer < other.integer;
	}
	bool operator>=(const Data& other) const
	{
		return !(*this < other);
	}
};

#endif // DATA_H_
