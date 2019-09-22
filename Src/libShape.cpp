//
//  libShape.cpp
//  libShape
//
//  Created by Louis Gehrig on 4/14/19.
//  Copyright © 2019 Louis Gehrig. All rights reserved.
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

// Standard includes
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

// Project includes
#include <libShape.hpp>

///////////////////////
// Utility functions //
///////////////////////

namespace libShape {

	int getInteger(const BYTE *pBuffer) {
		int nValue = ((pBuffer[0] & 127) << 24) + (pBuffer[1] << 16) + (pBuffer[2] << 8) + pBuffer[3];
		if(0x0 != (pBuffer[0] & 128)) nValue *= -1;
		return(nValue);
	}

	bool dblEquals( const double left, const double right) {
		bool bEquals = (fabs( left - right) <= SLACK_DOUBLES);
		return(bEquals);
	}

}

