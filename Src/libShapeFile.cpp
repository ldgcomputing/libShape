//
//  libShapeFile.cpp
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// STL includes
#include <list>

// Project includes
#include <libShapeFile.hpp>

namespace libShape {

	// Factory functions
	AbstractShape * buildShape(const int recordNum, const BYTE *pBuffer, const size_t bufSize) {

		// Must be a minimum of 4 bytes
		if( 4 > bufSize) {
			return ((AbstractShape *) 0x0);
		}

		// Decode the shape type
		AbstractShape *pRetValue = (AbstractShape *) 0x0;
		int nShapeType = * ((std::int32_t *) pBuffer);
		E_SHAPE_TYPE eShapeType = convertIntToShape(nShapeType);

		// And build
		switch(eShapeType) {

			case SHAPE_NULL:
				pRetValue = new ShapeNull(recordNum);
				break;

			case SHAPE_POINT :
				pRetValue = new ShapePoint( recordNum, pBuffer + 4, bufSize - 4);
				break;

			case SHAPE_POLYLINE:
				pRetValue = new ShapePolyline( recordNum, pBuffer + 4, bufSize - 4);
				break;

			case SHAPE_POLYGON:
				pRetValue = new ShapePolygon(recordNum, pBuffer + 4, bufSize - 4);
				break;

			case SHAPE_INVALID:
			default:
				pRetValue = new ShapeInvalid(recordNum);
				break;

		}

		// And done
		return(pRetValue);

	}

	///////////////////////
	// Utility functions //
	///////////////////////

	bool operator<( const S_BOUNDING_BOX &left, const S_BOUNDING_BOX &right) {

		bool bLessThan = false;

		if( left.Xmin == right.Xmin)
			bLessThan = left.Ymin < right.Ymin;
		else
			bLessThan = left.Xmin < right.Xmin;

		return( bLessThan);

	}

	E_SHAPE_TYPE convertIntToShape( const int nShapeValue) {

		E_SHAPE_TYPE eShapeType = SHAPE_INVALID;
		switch(nShapeValue) {
			case SHAPE_NULL:
				eShapeType = SHAPE_NULL;
				break;
			case SHAPE_POINT:
				eShapeType = SHAPE_POINT;
				break;
			case SHAPE_POLYLINE:
				eShapeType = SHAPE_POLYLINE;
				break;
			case SHAPE_POLYGON:
				eShapeType = SHAPE_POLYGON;
				break;
			case SHAPE_MULTIPOINT:
				eShapeType = SHAPE_MULTIPOINT;
				break;
			case SHAPE_POINTZ:
				eShapeType = SHAPE_POINTZ;
				break;
			case SHAPE_POLYLINE_Z:
				eShapeType = SHAPE_POLYLINE_Z;
				break;
			case SHAPE_POLYGON_Z:
				eShapeType = SHAPE_POLYGON_Z;
				break;
			case SHAPE_MULTIPOINT_Z:
				eShapeType = SHAPE_MULTIPOINT_Z;
				break;
			case SHAPE_POINT_M:
				eShapeType = SHAPE_POINT_M;
				break;
			case SHAPE_POLYLINE_M:
				eShapeType = SHAPE_POLYLINE_M;
				break;
			case SHAPE_POLYGON_M:
				eShapeType = SHAPE_POLYGON_M;
				break;
			case SHAPE_MULTIPOINT_M:
				eShapeType = SHAPE_MULTIPOINT_M;
				break;
			case SHAPE_MULTIPATH:
				eShapeType = SHAPE_MULTIPATH;
				break;
			default:
				eShapeType = SHAPE_INVALID;
				break;

		}
		return(eShapeType);

	}

	const char * getShapeTypeText( const E_SHAPE_TYPE eShape) {
		const char *output = "<Unknown>";
		switch(eShape) {
			case SHAPE_NULL:
				output = "SHAPE_NULL";
				break;
			case SHAPE_POINT:
				output = "SHAPE_POINT";
				break;
			case SHAPE_POLYLINE:
				output = "SHAPE_POLYLINE";
				break;
			case SHAPE_POLYGON:
				output = "SHAPE_POLYGON";
				break;
			case SHAPE_MULTIPOINT:
				output = "SHAPE_MULTIPOINT";
				break;
			case SHAPE_POINTZ:
				output = "SHAPE_POINTZ";
				break;
			case SHAPE_POLYLINE_Z:
				output = "SHAPE_POLYLINE_Z";
				break;
			case SHAPE_POLYGON_Z:
				output = "SHAPE_POLYGON_Z";
				break;
			case SHAPE_MULTIPOINT_Z:
				output = "SHAPE_MULTIPOINT_Z";
				break;
			case SHAPE_POINT_M:
				output = "SHAPE_POINT_M";
				break;
			case SHAPE_POLYLINE_M:
				output = "SHAPE_POLYLINE_M";
				break;
			case SHAPE_POLYGON_M:
				output = "SHAPE_POLYGON_M";
				break;
			case SHAPE_MULTIPOINT_M:
				output = "SHAPE_MULTIPOINT_M";
				break;
			case SHAPE_MULTIPATH:
				output = "SHAPE_MULTIPATH";
				break;
			case SHAPE_INVALID:
				output = "SHAPE_INVALID";
				break;
		}
		return(output);
	}

	int onLeftSide( const S_POINT vertex1, const S_POINT vertex2, const S_POINT ptToCheck) {

		double part1 = (vertex2.x - vertex1.x) * (ptToCheck.y - vertex1.y);
		double part2 = (ptToCheck.x - vertex1.x) * (vertex2.y - vertex1.y);
		bool sameValue = dblEquals( part1, part2);
		int retValue = (sameValue ? 0x0 : ((part1 > part2) ? 1 : -1));
		return( retValue);

	}

	bool onRightSide( const S_POINT vertex1, const S_POINT vertex2, const S_POINT ptToCheck) {

		// Calculate angle between the vectors
		// cos(theta) = (dot product) / ( (length 1) * (length 2) )

		double vct1_x = vertex2.x - vertex1.x;
		double vct1_y = vertex2.y - vertex1.y;
		double vct2_x = ptToCheck.x - vertex1.x;
		double vct2_y = ptToCheck.y - vertex1.y;

		double dotProduct = (vct1_x * vct2_x) + (vct1_y * vct2_y);
		double length1 = sqrt( pow( vct1_x, 2.0) + pow( vct1_y, 2.0));
		double length2 = sqrt( pow( vct2_x, 2.0) + pow( vct2_y, 2.0));

		double cosValue = dotProduct / (length1 * length2);
		double theta = acos( cosValue);

		// Calculate angle of each vector
		double angleVct1 = ( dblEquals( 0.0, vct1_x) ? M_PI / 2.0 : atan( vct1_y / vct1_x));
		double angleVct2 = ( dblEquals( 0.0, vct2_x) ? M_PI / 2.0 : atan( vct2_y / vct2_x));

		// Right or left?
		double angleRight = angleVct1 - theta;
		bool onRight = dblEquals( angleRight, angleVct2);

		return( onRight);

	}

	int getWindingNumber( const POLYGON polygon, const S_POINT ptToCheck, const bool debug) {

		// Loop over all points
		int windingNumber = 0x0;
		CITR_POINTS itrPoints = polygon.begin();
		long nPos = 0;
		for( ; polygon.end() != itrPoints; ++ itrPoints, ++ nPos) {

			// Get current and next point
			const S_POINT & curPoint = *itrPoints;
			CITR_POINTS nextItr = itrPoints;
			++ nextItr;
			if( polygon.end() == nextItr) {
				nextItr = polygon.begin();
			}
			const S_POINT & nextPoint = *nextItr;

			// Winding logic (http://geomalgorithms.com/a03-_inclusion.html)
			if( curPoint.y <= ptToCheck.y) {
				// Upward crossing?
				if( nextPoint.y > ptToCheck.y) {
					if( onLeftSide( curPoint, nextPoint, ptToCheck) > 0) {
						++ windingNumber;	// valid up intersect
					}
				}
			}
			else {
				// Downward crossing?
				if( nextPoint.y <= ptToCheck.y) {
					if( onLeftSide( curPoint, nextPoint, ptToCheck) < 0) {
						-- windingNumber;	// valid down intersect
					}
				}
			}

		} // endfor loop over points

		// And done
		return( windingNumber);

	}

	// Construction of shape exception
	ShapeException::ShapeException(std::string msg): excpMsg(msg) {

	}

	// Destruction of shape exception
	ShapeException::~ShapeException() {

	}

	// Simple construction for shape class
	AbstractShape::AbstractShape(const int recordNum, const E_SHAPE_TYPE shapeType) : nRecordNum(recordNum), eShapeType(shapeType) {
		memset( &boundingBox, 0x0, sizeof( boundingBox));
	}

	// Destruction of the shape
	AbstractShape::~AbstractShape() {

	}

	// Construction of reader class
	const unsigned long Reader::MAXIMUM_RECORD_SIZE = 16 * 1024 * 1024 - 100;
	const unsigned long Reader::SHAPES_RESERVE_SIZE = 7500;
	Reader::Reader( FILE *fShapeFile, const bool bStrict) {

		// Allocate a very large buffer
		shapes.reserve(SHAPES_RESERVE_SIZE);

		// Clear the header information
		memset(&header, 0x0, sizeof(header));

		// Error?
		if( (FILE *) 0x0 == fShapeFile) {
			throw( new ShapeException( std::string("NULL shape file")));
		}

		// Attempt to read the raw header information
		BYTE headerBuff[100];
		memset(headerBuff, 0x0, sizeof(headerBuff));
		if( 100 != fread( headerBuff, sizeof(BYTE), 100, fShapeFile)) {
			throw( new ShapeException( std::string("Insufficient bytes to gather shape file header")));
		}
		header.fileCode = getInteger(headerBuff + 0);
		header.unused_1 = getInteger(headerBuff + 4);
		header.unused_2 = getInteger(headerBuff + 8);
		header.unused_3 = getInteger(headerBuff + 12);
		header.unused_4 = getInteger(headerBuff + 16);
		header.unused_5 = getInteger(headerBuff + 20);
		header.fileLength = 2 * getInteger(headerBuff + 24);
		header.version = * ((std::int32_t *) (headerBuff + 28));
		header.shapeType = * ((std::int32_t *) (headerBuff + 32));
		header.boundingBox.Xmin = * ((double *) (headerBuff + 36));
		header.boundingBox.Ymin = * ((double *) (headerBuff + 44));
		header.boundingBox.Xmax = * ((double *) (headerBuff + 52));
		header.boundingBox.Ymax = * ((double *) (headerBuff + 60));
		header.Zmin = * ((double *) (headerBuff + 68));
		header.Zmax = * ((double *) (headerBuff + 76));
		header.Mmin = * ((double *) (headerBuff + 84));
		header.Mmax = * ((double *) (headerBuff + 92));

		// Allocate the giant read buffer
		BYTE *pBuffer = new BYTE[MAXIMUM_RECORD_SIZE];
		if( (BYTE *) 0x0 == pBuffer) {
			char msg[1024 + 1];
			sprintf( msg, "Unable to read allocate read buffer with size %lu", MAXIMUM_RECORD_SIZE);
			throw( new ShapeException( std::string( msg)));
		}

		// Now read all the records
		unsigned long nCurRec = 0;
		while(!feof(fShapeFile)) {

			int nRecordNumber = 0x0;
			int nRecordSize = 0x0;
			BYTE recStart[8];
			memset(recStart, 0x0, sizeof(recStart));

			// Read the record number and size
			size_t tRead = fread( recStart, sizeof(BYTE), 8, fShapeFile);
			if( (0x0 == tRead) && feof(fShapeFile))
				break;
			else if( 8 != tRead) {
				char msg[1024 + 1];
				sprintf( msg, "Unable to read record header at current record %lu - expected 8 bytes got %lu bytes", nCurRec, tRead);
				throw( new ShapeException( std::string( msg)));
			}
			nRecordNumber = getInteger( recStart + 0);
			nRecordSize = 2 * getInteger( recStart + 4);

			// Read in this value
			tRead = fread(pBuffer, sizeof(BYTE), nRecordSize, fShapeFile);
			if(tRead != nRecordSize) {
				char msg[1024 + 1];
				sprintf( msg, "At current record %lu, expected to read %d but only read %lu", nCurRec, nRecordSize, tRead);
				throw( new ShapeException( std::string( msg)));
			}

			// Build the shape
			AbstractShape *nextShape = buildShape(nRecordNumber, pBuffer, tRead);
			shapes.push_back(nextShape);

			// And increment the count
			++nCurRec;

		}

		// And clear the buffer
		delete [] pBuffer;
		pBuffer = (BYTE *) 0x0;

	}

	// Destruction of reader class
	Reader::~Reader() {

		// Clear the accumulated shapes
		ITR_SHAPES itr = shapes.begin();
		for(; shapes.end() != itr; ++ itr) {
			AbstractShape *pShape = *itr;
			delete pShape;
		}
		shapes.clear();

	}

	// Take ownership of the shapes
	CNT_SHAPES * Reader::takeShapes() {

		// Make a copy of the polygons
		CNT_SHAPES *pRetValue = new CNT_SHAPES();
		if( (CNT_SHAPES *) 0x0 == pRetValue) return( (CNT_SHAPES *) 0x0);

		// Copy values over
		pRetValue->insert( pRetValue->begin(), shapes.begin(), shapes.end());

		// Clear old shapes queue
		shapes.clear();

		// And done
		return(pRetValue);

	}

	////////////
	// SHAPES //
	////////////

	ShapeInvalid::ShapeInvalid(const int recordNum) : AbstractShape( recordNum, SHAPE_INVALID) {

	}

	ShapeInvalid::~ShapeInvalid() {

	}

	ShapeNull::ShapeNull(const int recordNum) : AbstractShape( recordNum, SHAPE_NULL) {

	}

	ShapeNull::~ShapeNull() {

	}

	ShapePoint::ShapePoint( const int recordNum, const BYTE *pBuffer, const size_t bufSize) : AbstractShape( recordNum, SHAPE_POINT) {

		// Must be at least 8 bytes
		if( 8 > bufSize) {
			throw( new ShapeException( std::string( "Insufficient bytes for point shape")));
		}

		// Capture the point
		sPoint.x = * ((double *) (pBuffer + 0));
		sPoint.y = * ((double *) (pBuffer + 4));

	}

	ShapePoint::ShapePoint(const ShapePoint &copyShape) : AbstractShape( copyShape.nRecordNum, SHAPE_POINT) {
		sPoint.x = copyShape.sPoint.x;
		sPoint.y = copyShape.sPoint.y;
	}

	ShapePoint & ShapePoint::operator=( const ShapePoint &copyShape) {
		sPoint.x = copyShape.sPoint.x;
		sPoint.y = copyShape.sPoint.y;
		return(*this);
	}

	ShapePoint::~ShapePoint( ) {

	}

	ShapePolyline::ShapePolyline(const int recordNum, const BYTE *pBuffer, const size_t bufSize) : AbstractShape( recordNum, SHAPE_POLYLINE) {

		// Must be at least 40 bytes
		if( 40 > bufSize) {
			throw( new ShapeException( std::string( "Insufficient bytes for polyline shape")));
		}

		// Get the bounding box
		boundingBox.Xmin = * ((double *) (pBuffer + 0));
		boundingBox.Ymin = * ((double *) (pBuffer + 8));
		boundingBox.Xmax = * ((double *) (pBuffer + 16));
		boundingBox.Ymax = * ((double *) (pBuffer + 24));

		// Get the number of parts and points
		numParts = * ((int *) (pBuffer + 32));
		numPoints = * ((int *) (pBuffer + 36));
		cntPolylines.reserve(numParts);
		if(bufSize < (40 + (4 * numParts) + (8 * numPoints))) {
			throw( new ShapeException( std::string( "Exceeded structure size reading points")));
		}

		// Get the number of points in each line
		std::vector<int> lstPoints;
		std::vector<int>::iterator itrPoints;
		lstPoints.reserve(numParts);
		size_t curPos = 40;
		int nLastIndex = numPoints;
		for( int nPart = (numParts - 1); 0 <= nPart; -- nPart) {
			int nIndexPt = * ((int *) (pBuffer + curPos + (4 * nPart)));
			lstPoints.insert( lstPoints.begin(), (nLastIndex - nIndexPt));
			nLastIndex = nIndexPt;
		}

		// Now build the polylines
		curPos += (4 * numParts);
		for( itrPoints = lstPoints.begin(); lstPoints.end() != itrPoints; ++ itrPoints) {
			const int nPoints = *itrPoints;
			POLYLINE nextPolyLine;
			nextPolyLine.reserve(nPoints);
			for( int nPoint = 0; nPoints > nPoint; ++ nPoint, curPos += 16) {
				if(bufSize < curPos) {
					throw( new ShapeException( std::string( "Exceeded structure size reading points")));
				}
				S_POINT nextPoint;
				nextPoint.x = * ((double *) (pBuffer + curPos + 0x0));
				nextPoint.y = * ((double *) (pBuffer + curPos + 0x8));
				nextPolyLine.push_back(nextPoint);
			}
			cntPolylines.push_back(nextPolyLine);
		}

	}

	ShapePolyline::ShapePolyline(const ShapePolyline &copyShape) : AbstractShape( copyShape.nRecordNum, SHAPE_POLYGON) {

		// Set the bounding box
		boundingBox.Xmin = copyShape.boundingBox.Xmin;
		boundingBox.Ymin = copyShape.boundingBox.Ymin;
		boundingBox.Xmax = copyShape.boundingBox.Xmax;
		boundingBox.Ymax = copyShape.boundingBox.Ymax;

		// Get the number of parts and points
		numParts = copyShape.numParts;
		numPoints = copyShape.numPoints;

		// Copy the points and polylines
		CITR_POLYLINE itrLines = copyShape.cntPolylines.begin();
		for( ; copyShape.cntPolylines.end() != itrLines; ++ itrLines) {
			const POLYLINE &pl = *itrLines;
			POLYLINE nextPL( pl);
			cntPolylines.push_back( nextPL);
		}
		
	}

	ShapePolyline & ShapePolyline::operator=( const ShapePolyline &copyShape) {

		// Set the bounding box
		boundingBox.Xmin = copyShape.boundingBox.Xmin;
		boundingBox.Ymin = copyShape.boundingBox.Ymin;
		boundingBox.Xmax = copyShape.boundingBox.Xmax;
		boundingBox.Ymax = copyShape.boundingBox.Ymax;

		// Get the number of parts and points
		numParts = copyShape.numParts;
		numPoints = copyShape.numPoints;
		cntPolylines.clear();
		cntPolylines.insert( cntPolylines.begin(), copyShape.cntPolylines.begin(), copyShape.cntPolylines.end());

		// And done
		return(*this);

	}

	ShapePolyline::~ShapePolyline() {

	}

	ShapePolygon::ShapePolygon(const int recordNum, const BYTE *pBuffer, const size_t bufSize) : AbstractShape( recordNum, SHAPE_POLYGON) {

		// Must be at least 40 bytes
		if( 40 > bufSize) {
			throw( new ShapeException( std::string( "Insufficient bytes for polygon shape")));
		}

		// Get the bounding box
		boundingBox.Xmin = * ((double *) (pBuffer + 0));
		boundingBox.Ymin = * ((double *) (pBuffer + 8));
		boundingBox.Xmax = * ((double *) (pBuffer + 16));
		boundingBox.Ymax = * ((double *) (pBuffer + 24));

		// Get the number of parts and points
		numParts = * ((int *) (pBuffer + 32));
		numPoints = * ((int *) (pBuffer + 36));
		cntPolygons.reserve(numParts);
		if(bufSize < (40 + (4 * numParts) + (8 * numPoints))) {
			throw( new ShapeException( std::string( "Exceeded structure size reading points")));
		}

		// Get the number of points in each polygon
		std::vector<int> lstPoints;
		std::vector<int>::iterator itrPoints;
		lstPoints.reserve(numParts);
		size_t curPos = 40;
		int nLastIndex = numPoints;
		for( int nPart = (numParts - 1); 0 <= nPart; -- nPart) {
			int nIndexPt = * ((int *) (pBuffer + curPos + (4 * nPart)));
			lstPoints.insert( lstPoints.begin(), (nLastIndex - nIndexPt));
			nLastIndex = nIndexPt;
		}

		// Now build the polygons
		curPos += (4 * numParts);
		for( itrPoints = lstPoints.begin(); lstPoints.end() != itrPoints; ++ itrPoints) {
			const int nPoints = *itrPoints;
			POLYGON nextPolygon;
			nextPolygon.reserve(nPoints);
			for( int nPoint = 0; nPoints > nPoint; ++ nPoint, curPos += 16) {
				if(bufSize < curPos) {
					throw( new ShapeException( std::string( "Exceeded structure size reading points")));
				}
				S_POINT nextPoint;
				nextPoint.x = * ((double *) (pBuffer + curPos + 0x0));
				nextPoint.y = * ((double *) (pBuffer + curPos + 0x8));
				nextPolygon.push_back(nextPoint);
			}
			cntPolygons.push_back(nextPolygon);
		}

	}

	ShapePolygon::ShapePolygon(const int recordNum, const CNT_POLYGON &polygons) : AbstractShape( recordNum, SHAPE_POLYGON) {

		cntPolygons.insert( cntPolygons.begin(), polygons.begin(), polygons.end());

	}

	ShapePolygon::~ShapePolygon() {

	}

	bool ShapePolygon::containsPoint( double x, double y) const {

		// Variables
		bool atLeastOneContained = false;
		bool noLeftCircles = true;
		S_POINT checkPt = { x , y } ;

		// Loop thru the polygons
		CITR_POLYGON itrPolygon = cntPolygons.begin();
		for(; cntPolygons.end() != itrPolygon; ++ itrPolygon) {

			// Winding number of current point must be negative (right circling)
			const POLYGON &curPolygon = *itrPolygon;
			int checkPtWindingNumber = getWindingNumber( curPolygon, checkPt);

			// Need at least one contained inside, and no left circling
			noLeftCircles &= (0 >= checkPtWindingNumber);
			atLeastOneContained |= (0 > checkPtWindingNumber);

		} // endfor loop thru polygons

		// And done
		return( noLeftCircles && atLeastOneContained);

	}

}

