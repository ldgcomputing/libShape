//
//  libShapeDB.cpp
//  libShape
//
//  Created by Louis Gehrig on 4/13/19.
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
#include <string.h>
#include <unistd.h>

// STL includes
#include <string>

// Project includes
#include <libShapeDB.hpp>

namespace libShape {

	// Construction of shape exception
	dbException::dbException(std::string msg): excpMsg(msg) {

	}

	// Destruction of shape exception
	dbException::~dbException() {

	}

	// Construction of the db field class
	dbField::dbField( const BYTE *pBuffer, const size_t bufSize) {

		// Validate input
		if( (const BYTE *) 0x0 == pBuffer) {
			throw( new dbException( std::string( "NULL pointer not acceptable for dbField construction")));
		}
		if( bufSize != 32) {
			char errMsg [1000];
			sprintf( errMsg, "Expected record size of 32, but got %lu", bufSize);
			throw( new dbException( std::string( errMsg)));
		}

		// Initialize everything
		memset( fieldName, 0x0, sizeof( fieldName));
		strncpy( fieldName, (const char *) pBuffer, 10);

		// Decode field type
		switch(pBuffer[11]) {

			case 'c' :
			case 'C' :
				eFieldType = FT_TEXT;
				break;
			case 'n' :
			case 'N' :
				eFieldType = FT_NUMBER;
				break;
			case 'l' :
			case 'L' :
				eFieldType = FT_LOGICAL;
				break;
			default :
				eFieldType = FT_INVALID;
				break;

		}

		// Decode the field length and decimal count
		fieldLength = pBuffer[16];
		decimalCount = pBuffer[17];

	}

	// Destruct the db field
	dbField::~dbField() {

	}

	// Construct the DB table
	dbTable::dbTable( FILE *dbFile) : fileDB(dbFile) {

		// Validate input
		if( (FILE *) 0x0 == dbFile) {
			throw( new dbException( std::string( "NULL file not permitted")));
		}

		// Read the version information
		version = fgetc( dbFile);

		// Pull the last update
		memset(lastUpdate, 0x0, sizeof(lastUpdate));
		if( 3 != fread( lastUpdate, sizeof(BYTE), 3, dbFile)) {
			throw( new dbException( std::string( "Not able to read last update time")));
		}

		// Pull the number of records
		BYTE recBuffer[4];
		if( 4 != fread( recBuffer, sizeof(BYTE), 4, dbFile)) {
			throw( new dbException( std::string( "Not able to read record count")));
		}
		numRecords = * ((int *) recBuffer);

		// Get the header size and record size
		hdrSize = fgetc(dbFile) + 256 * fgetc(dbFile);
		recSize = fgetc(dbFile) + 256 * fgetc(dbFile);

		// Allocate the record buffer
		pRecordBuffer = new BYTE[recSize + 16];
		if( (BYTE *) 0x0 == pRecordBuffer) {
			throw( new dbException( std::string( "Not able to allocate record buffer")));
		}

		// Seek to the field descriptions
		if( 0x0 != fseek( dbFile, 32, SEEK_SET)) {
			throw( new dbException( std::string( "Not able to read field descriptors")));
		}

		// Read each field
		BYTE fieldBuffer [32 + 1];
		size_t curPos = 32;
		int nCurField = 1;
		for( ; hdrSize > curPos; curPos += 32, ++ nCurField) {
			if( 32 != fread( fieldBuffer, sizeof(BYTE), 32, dbFile)) {
				char errMsg[1000];
				sprintf( errMsg, "Unable to read field %d", nCurField);
				throw( new dbException( std::string( errMsg)));
			}
			if( fieldBuffer[0] == '\r') break;
			dbField nextField( fieldBuffer, 32);
			cntFields.push_back(nextField);
		}

	}

	// Destruct the db table
	dbTable::~dbTable() {

		// Free buffer
		if( (BYTE *) 0x0 != pRecordBuffer)
			delete [] pRecordBuffer;

	}

	// Get the raw bytes for a record
	const BYTE * dbTable::getRecordBytes(const size_t recNum) {

		// Compute the location and advance
		off_t position = hdrSize + (recNum * (recSize + 0));
		if( 0x0 != fseek( fileDB, position, SEEK_SET)) {
			throw( new dbException( std::string( "Unable to seek to requested record position")));
		}

		// Read the record
		if( recSize != fread(pRecordBuffer, sizeof(BYTE), recSize, fileDB)) {
			throw( new dbException( std::string( "Failure to read record from file")));
		}

		// And done
		return(pRecordBuffer);

	}

};

