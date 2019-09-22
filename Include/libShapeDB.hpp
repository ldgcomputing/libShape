//
//  libShapeDB.hpp
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

#ifndef	INCLUDE_LIBSHAPEDB_HPP
#define	INCLUDE_LIBSHAPEDB_HPP

// STL includes
#include <list>
#include <string>
#include <vector>

// Project includes
#include <libShape.hpp>

namespace libShape {

	// A DB exception
	class dbException {

	public:
		dbException(std::string msg);
		virtual ~dbException();
		const std::string excpMsg;
	};

	// The DB field information
	class dbField {

	public:

		// The possible field types
		enum e_field_types {
			FT_TEXT = 'T',
			FT_NUMBER = 'N',
			FT_LOGICAL = 'L',
			FT_INVALID = 'X'
		};
		typedef enum e_field_types E_FIELD_TYPE;

		// Construction from byte buffer
		dbField( const BYTE *pBuffer, const size_t bufSize);

		// Destruction
		virtual ~dbField();

		// Get the name of the field
		const char * getName() const { return fieldName; }

		// Get the field type
		E_FIELD_TYPE getType() const { return eFieldType; }

		// Get the field length
		size_t getLength() const { return fieldLength; }

		// Get the decimal count (if any)
		size_t getDecimalCount() const { return decimalCount; }

	protected:

		// The name of the field
		char fieldName [10 + 1];

		// The field type
		E_FIELD_TYPE eFieldType;

		// The field length
		size_t fieldLength;

		// The decimal count
		size_t decimalCount;

	};
	typedef std::vector<dbField> CNT_FIELDS;
	typedef CNT_FIELDS::const_iterator CITR_FIELDS;
	typedef CNT_FIELDS::iterator ITR_FIELDS;

	class dbTable {

	public:

		// Construction
		dbTable( FILE *dbFile);

		// Destruction
		virtual ~dbTable();

		// Get the table version
		int getVersion() const { return version; }

		// Get the fields
		const CNT_FIELDS & getFields() const { return cntFields; }

		// Get the number of records
		size_t getRecordCount() const { return numRecords; }

		// Get the last update
		const BYTE * getLastUpdate() const { return lastUpdate; }

		// Get the size of each record
		size_t getRecordSize() const { return recSize; }

		// Get the raw bytes for a record
		const BYTE * getRecordBytes(const size_t recNum);

	protected:

		// The DB file
		FILE *fileDB;

		// The DB version
		int version;

		// The number of records
		size_t numRecords;

		// The list of fields
		CNT_FIELDS cntFields;

		// The last update
		BYTE lastUpdate[3];

		// The header size
		size_t hdrSize;

		// The record size
		size_t recSize;

		// The record buffer
		BYTE *pRecordBuffer;

	};

};

#endif

