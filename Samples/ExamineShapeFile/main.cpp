//
//  main.cpp
//  
//
//  Created by Louis Gehrig on 11/30/19.
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include <string.h>

// Project includes
#include <libShapeFile.hpp>

// Some useful debug macros
#define LP		fprintf( stderr, "Now at %s:%d\n", __FILE__, __LINE__);
#define LPd(x)	fprintf( stderr, "Now at %s:%d - %s = %d\n", __FILE__, __LINE__, #x, (x));
#define LPs(x)	fprintf( stderr, "Now at %s:%d - %s = '%s'\n", __FILE__, __LINE__, #x, (x));

//
// Global variables
//
// g_argError
//		"true" if one or more program args are invalid
//
// g_showArgs
//		"true" if the program arguments should be output
//
// g_strDBFile
//		A pointer to the database filename to open
//
// g_strShapeFile
//		A pointer to the shape filename to open
//

static bool g_argError = false;
static bool g_showArgs = false;
static char *g_strDBFile = (char *) 0x0;
static char *g_strShapeFile = (char *) 0x0;

//
// Show program arguments
//

void showArgs( const char *progName) {

	printf( "\nProgram usage: %s < shapeFile > < dbFile > [ -? | -help | --help ]\n", progName);
	printf( "\n");
	printf( "\tshapeFile       The full path to the shape file to open\n");
	printf( "\tdbFile          The full path to the database file to open\n");
	printf( "\n\n");

}

//
// Decode the program arguments
//
// This function assumes the program arguments are in the
// following order:
//
//		1. The shape file
//		2. The database file
//
// Decoded information is stored in global variables
//

void decodeProgramArgs( int argc, char **argv) {

	// Loop over the program arguments
	for( int i = 1 ; i < argc ; ++ i) {

		// Begins with "-"?
		if( argv [i][0] == '-') {

			// Show args?
			if(
			   (0x0 == strcmp( argv [i] + 1, "?")) ||
			   (0x0 == strcmp( argv [i] + 1, "h")) ||
			   (0x0 == strcmp( argv [i] + 1, "help")) ||
			   (0x0 == strcmp( argv [i] + 1, "-help"))
			   ) {
				g_showArgs = true;
			}

			// And an error ...
			else {
				fprintf( stderr, "Unknown switch: %s\n", argv [i]);
				g_argError = true;
			}

		}

		// Not a switch
		else {

			// Empty shape file?
			if( (char *) 0x0 == g_strShapeFile) {
				g_strShapeFile = argv [i];
			}

			// Empty db file?
			else if( (char *) 0x0 == g_strDBFile) {
				g_strDBFile = argv [i];
			}

			// Bad argument
			else {
				fprintf( stderr, "Unknown argument: %s\n", argv [i]);
				g_argError = true;
			}

		}

	} // endfor loop over arguments

	// Have db file?
	if( (char *) 0x0 == g_strDBFile) {
		fprintf( stderr, "Error - missing database file argument!\n");
		g_argError = true;
	}
	if( (char *) 0x0 == g_strShapeFile) {
		fprintf( stderr, "Error - missing shape file argument!\n");
		g_argError = true;
	}

}

//////////
// MAIN //
//////////

int main( int argc, char **argv) {

	// Program vars
	FILE *fDBFile = (FILE *) 0x0;
	FILE *fShapeFile = (FILE *) 0x0;
	int nRetCode = EXIT_FAILURE;

	// Decode the program arguments
	decodeProgramArgs( argc, argv);
	if( g_showArgs || g_argError) {
		showArgs( argv[0]);
		exit( g_argError ? EXIT_FAILURE : EXIT_SUCCESS);
	}

	// Wrap it all
	try {

		// Open the shape file
		fShapeFile = fopen( g_strShapeFile, "rb");
		if( (FILE *) 0x0 == fShapeFile)
			throw( "Failed to open shapefile");

		// Open the DB file
		fDBFile = fopen( g_strDBFile, "rb");
		if( (FILE *) 0x0 == fDBFile)
			throw( "Failed to open database file");

		// Allocate the shape reader
		libShape::Reader shpReader( fShapeFile);
		const libShape::S_SHAPE_HEADER shpHeader = shpReader.getShapeHeader();
		const libShape::CNT_SHAPES shpShapes = shpReader.getShapes();

		// Determine the shape type
		char shapeTypeName [100 + 1];
		memset( shapeTypeName, 0x0, sizeof( shapeTypeName));
		switch( shpHeader.shapeType) {

			case libShape::SHAPE_NULL :
				strncpy( shapeTypeName, "NULL", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POINT :
				strncpy( shapeTypeName, "POINT", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYLINE :
				strncpy( shapeTypeName, "POLYLINE", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYGON :
				strncpy( shapeTypeName, "POLYGON", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_MULTIPOINT :
				strncpy( shapeTypeName, "MULTIPOINT", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POINTZ :
				strncpy( shapeTypeName, "POINT Z", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYLINE_Z :
				strncpy( shapeTypeName, "POLYLINE Z", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYGON_Z :
				strncpy( shapeTypeName, "POLYGON Z", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_MULTIPOINT_Z :
				strncpy( shapeTypeName, "MULTIPOINT Z", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POINT_M :
				strncpy( shapeTypeName, "POINT M", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYLINE_M :
				strncpy( shapeTypeName, "POLYLINE M", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_POLYGON_M :
				strncpy( shapeTypeName, "POLYGON M", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_MULTIPOINT_M :
				strncpy( shapeTypeName, "MULTIPOINT M", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_MULTIPATH :
				strncpy( shapeTypeName, "MULTIPATH", sizeof( shapeTypeName) - 1);
				break;
			case libShape::SHAPE_INVALID :
				strncpy( shapeTypeName, "INVALID", sizeof( shapeTypeName) - 1);
				break;
			default :
				strncpy( shapeTypeName, "** UNKNOWN **", sizeof( shapeTypeName) - 1);
				break;

		}

		// Report on the shape file
		printf( "\n\n");
		printf( "Shape file information\n");
		printf( "======================\n");
		printf( "Shape count =  %lu\n", shpShapes.size());
		printf( "fileCode =     %d\n", shpHeader.fileCode);
		printf( "fileLength =   %d (actual bytes)\n", shpHeader.fileLength);
		printf( "version =      %d\n", shpHeader.version);
		printf( "shapeType =    %d (%s)\n", shpHeader.shapeType, shapeTypeName);
		printf( "bounding box = ( %-12.8f , %-12.8f ) to ( %-12.8f , %-12.8f )\n", shpHeader.boundingBox.Xmin, shpHeader.boundingBox.Ymin, shpHeader.boundingBox.Xmax, shpHeader.boundingBox.Xmax );
		printf( "Zmin =         %-12.8f\n", shpHeader.Zmin);
		printf( "Zmax =         %-12.8f\n", shpHeader.Zmax);
		printf( "Mmin =         %-12.8f\n", shpHeader.Mmin);
		printf( "Mmax =         %-12.8f\n", shpHeader.Mmax);

		// Open the database file
		libShape::dbTable shpTable( fDBFile);
		const libShape::CNT_FIELDS &shpFields = shpTable.getFields();
		int numFields = shpFields.size();

		// Report on the database
		printf( "\n\n");
		printf( "Database information\n");
		printf( "====================\n");
		printf( "DB version =   %d\n", shpTable.getVersion());
		printf( "Num fields =   %d\n", numFields);
		printf( "Records =      %lu\n", shpTable.getRecordCount());
		printf( "Rec size =     %lu\n", shpTable.getRecordSize());
		printf( "\n");

		// Loop over fields
		const char FIELD_FORMAT [] = "%5s   %-10s %-7s %-6s   %7s\n";
		const char FIELD_FORMAT_DATA [] = "%5d   %-10s %-7s %6lu   %7lu\n";
		printf( FIELD_FORMAT, "Field", "Name", "Type", "Length", "Decimal");
		printf( FIELD_FORMAT, "-----", "----", "----", "------", "-------");
		libShape::CITR_FIELDS itrFields = shpFields.begin();
		int nCurField = 1;
		for( ; shpFields.end() != itrFields; ++ itrFields, ++ nCurField) {
			const libShape::dbField &field = *itrFields;
			char type [10 + 1];
			memset( type, 0x0, sizeof( type));
			switch( field.getType()) {
				case libShape::dbField::FT_TEXT :
					strncpy( type, "TEXT", 10);
					break;
				case libShape::dbField::FT_NUMBER :
					strncpy( type, "NUMBER", 10);
					break;
				case libShape::dbField::FT_LOGICAL :
					strncpy( type, "LOGICAL", 10);
					break;
				case libShape::dbField::FT_INVALID :
					strncpy( type, "INVALID", 10);
					break;
				default:
					strncpy( type, "????", 10);
					break;
			}
			printf( FIELD_FORMAT_DATA, nCurField, field.getName(), type, field.getLength(), field.getDecimalCount());
		}

		// And done
		printf( "\n\n");
		nRetCode = EXIT_SUCCESS;

	}

	catch( libShape::dbException &e) {
		fprintf( stderr, "Caught a database exception: %s\n", e.excpMsg.c_str());
		nRetCode = EXIT_FAILURE;
	}

	catch( libShape::dbException *e) {
		fprintf( stderr, "Caught a database exception: %s\n", e->excpMsg.c_str());
		nRetCode = EXIT_FAILURE;
		delete e;
	}

	catch( const char *e) {
		fprintf( stderr, "Caught an exception: %s\n", e);
		nRetCode = EXIT_FAILURE;
	}

	catch( ...) {
		fprintf( stderr, "An unknown exception has been caught\n");
		nRetCode = EXIT_FAILURE;
	}

	// Anything need closing?
	if( (FILE *) 0x0 != fShapeFile) {
		fclose( fShapeFile);
		fShapeFile = (FILE *) 0x0;
	}
	if( (FILE *) 0x0 != fDBFile) {
		fclose( fDBFile);
		fDBFile = (FILE *) 0x0;
	}

	// And done
	return( nRetCode);

}

