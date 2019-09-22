//
//  libShape.hpp
//  libShape
//
//  Created by Louis Gehrig on 4/13/19.
//  Copyright © 2019 Louis Gehrig. All rights reserved.
//

//
// This header file does declare a few useful utility functions
// of use, but it's primary point is to be the single necessary
// include for the rest of the libShape library.
//

/***

	MIT License

	Copyright (c) 2019 Louis Gehrig

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.

***/

#ifndef libShape_hpp
#define libShape_hpp

namespace libShape {

	// Helpful typedefs
	typedef unsigned char BYTE;

	// The permitted delta on doubles
	const double SLACK_DOUBLES = 0.00000000000001;

	// Utility function - convert big-endian signed 32 bits to int
	int getInteger(const BYTE *pBuffer);

	// Utility function - see if a double is "close-enough"
	bool dblEquals( const double left, const double right);

};

#include <libShapeDB.hpp>
#include <libShapeFile.hpp>

#endif /* libShape_h */
