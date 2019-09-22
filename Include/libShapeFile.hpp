//
//  libShapeFile.hpp
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

#ifndef ShapeType_hpp
#define ShapeType_hpp

// Standard includes
#include <stdio.h>

// STL includes
#include <list>
#include <string>
#include <vector>

// Project includes
#include <libShape.hpp>

namespace libShape {

	// Enumerated list of shape types and values
	enum e_shape_types {

		SHAPE_NULL = 0,             // Null shape
		SHAPE_POINT = 1,            // Point shape (X, Y)
		SHAPE_POLYLINE = 3,         // MBR, Number of parts, Number of points, Parts, Points
		SHAPE_POLYGON = 5,          // MBR, Number of parts, Number of points, Parts, Points
		SHAPE_MULTIPOINT = 8,       // MBR, Number of points, Points
		SHAPE_POINTZ = 11,          // Point shape (X, Y, Z)
		SHAPE_POLYLINE_Z = 13,
		SHAPE_POLYGON_Z = 15,
		SHAPE_MULTIPOINT_Z = 18,
		SHAPE_POINT_M = 21,
		SHAPE_POLYLINE_M = 23,
		SHAPE_POLYGON_M = 25,
		SHAPE_MULTIPOINT_M = 28,
		SHAPE_MULTIPATH = 31,
		SHAPE_INVALID = 255

	};
	typedef e_shape_types E_SHAPE_TYPE;

	// A bounding box
	struct s_bounding_box {
		double Xmin;
		double Ymin;
		double Xmax;
		double Ymax;
	};
	typedef struct s_bounding_box S_BOUNDING_BOX;
	bool operator<( const S_BOUNDING_BOX &left, const S_BOUNDING_BOX &right);

	// A simple point
	struct s_point {
		double x;
		double y;
		double z;
		double m;
	};
	typedef struct s_point S_POINT;
	typedef std::vector<S_POINT> CNT_POINTS;
	typedef CNT_POINTS::const_iterator CITR_POINTS;
	typedef CNT_POINTS::iterator ITR_POINTS;

	// Polyline definition
	typedef CNT_POINTS POLYLINE;
	typedef POLYLINE::const_iterator CITR_POLYLINE_PTS;
	typedef POLYLINE::iterator ITR_POLYLINE_PTS;
	typedef std::vector<POLYLINE> CNT_POLYLINE;
	typedef CNT_POLYLINE::const_iterator CITR_POLYLINE;
	typedef CNT_POLYLINE::iterator ITR_POLYLINE;

	// Polygon definition
	typedef CNT_POINTS POLYGON;
	typedef POLYGON::const_iterator CITR_POLYGON_PTS;
	typedef POLYGON::iterator ITR_POLYGON_PTS;
	typedef std::vector<POLYGON> CNT_POLYGON;
	typedef CNT_POLYGON::const_iterator CITR_POLYGON;
	typedef CNT_POLYGON::iterator ITR_POLYGON;

	// The header information - converted to native format
	struct s_shape_header {
		int fileCode;
		int unused_1;
		int unused_2;
		int unused_3;
		int unused_4;
		int unused_5;
		int fileLength;	// converted to actual bytes, not 16-bit words
		int version;
		int shapeType;
		S_BOUNDING_BOX boundingBox;
		double Zmin;
		double Zmax;
		double Mmin;
		double Mmax;
	};
	typedef struct s_shape_header S_SHAPE_HEADER;

	// A shape exception
	class ShapeException {

	public:
		ShapeException(std::string msg);
		virtual ~ShapeException();
		const std::string excpMsg;
	};

	// The shape class
	class AbstractShape {

	public:

		// Construction
		AbstractShape(const int recordNum, const E_SHAPE_TYPE shapeType);

		// Destruction
		virtual ~AbstractShape();

		// Get the shape record number
		int getRecordNumber() const { return nRecordNum; }

		// Get the shape type
		E_SHAPE_TYPE getShapeType() const { return( eShapeType); }

		// Bounding box variables
		const S_BOUNDING_BOX & getBoundingBox() const { return boundingBox; }

		// See if a point is contained within
		virtual bool containsPoint( double x, double y) const = 0;

	protected:

		// The record number
		int nRecordNum;

		// The shape type
		E_SHAPE_TYPE eShapeType;

		// The bounding box
		S_BOUNDING_BOX boundingBox;

	};
	typedef std::vector<AbstractShape *> CNT_SHAPES;
	typedef CNT_SHAPES::const_iterator CITR_SHAPES;
	typedef CNT_SHAPES::iterator ITR_SHAPES;

	// A reader class for shape files
	class Reader {

	public:

		// The maximum record size to allocate for
		const static unsigned long MAXIMUM_RECORD_SIZE;

		// The default number of shapes to allocate for
		const static unsigned long SHAPES_RESERVE_SIZE;

		// Construction
		Reader( FILE *fShapeFile, const bool bStrict = false);

		// Destruction
		virtual ~Reader();

		// Get the header information
		const S_SHAPE_HEADER & getShapeHeader() const { return header; }

		// Get the list of shapes
		const CNT_SHAPES & getShapes() const { return shapes; }

		// Take ownership of the list of shapes
		CNT_SHAPES * takeShapes();

	protected:

		// The header file for the shapes
		S_SHAPE_HEADER header;

		// The actual shapes
		CNT_SHAPES shapes;

	};

	// Factory function - build shape
	AbstractShape * buildShape(const int recordNum, const BYTE *pBuffer, const size_t bufSize);

	// Utilitu function - convert integer to shape type
	E_SHAPE_TYPE convertIntToShape( const int nShapeValue);

	// Utility function - shape type to text
	const char * getShapeTypeText( const E_SHAPE_TYPE eShape);

	// Utility - see if a point is on the left side of a vertex line
	// > 0 means ptToCheck is on the left side
	// 0 means ptToCheck is on the line
	// < 0 means ptToCheck is on the right side
	int onLeftSide( const S_POINT vertex1, const S_POINT vertex2, const S_POINT ptToCheck);

	// Utility - see if a point is on the right side of a vertex line
	bool onRightSide( const S_POINT vertex1, const S_POINT vertex2, const S_POINT ptToCheck);

	// Calculate winding number
	int getWindingNumber( const POLYGON polygon, const S_POINT ptToCheck, const bool debug = false);

	///////////////////
	// SHAPE CLASSES //
	///////////////////

	// The invalid shape
	class ShapeInvalid : public AbstractShape {

	public:

		// Construction and destruction
		ShapeInvalid(const int recordNum);
		virtual ~ShapeInvalid();

		// Overrides
		virtual bool containsPoint( double x, double y) const {
			return( false);
		}

	};

	// The null shape
	class ShapeNull : public AbstractShape {

	public:

		// Construction and destruction
		ShapeNull(const int recordNum);
		virtual ~ShapeNull();

		// Overrides
		virtual bool containsPoint( double x, double y) const {
			return( false);
		}

	};

	// The point shape
	class ShapePoint : public AbstractShape {

	public:

		// Construction - from byte buffer
		ShapePoint(const int recordNum, const BYTE *pBuffer, const size_t bufSize);

		// Construction - copy constructor
		ShapePoint(const ShapePoint &copyShape);

		// Construction - operator
		ShapePoint & operator=( const ShapePoint &copyShape);

		// Destruction
		virtual ~ShapePoint();

		// Get the point
		const S_POINT & getPoint() const { return( sPoint); }

		// Overrides
		virtual bool containsPoint( double x, double y) const {
			bool bSame = dblEquals( sPoint.x, x) && dblEquals( sPoint.y, y);
			return( bSame);
		}

	protected:

		// The point
		S_POINT sPoint;

	};

	// The polyline shape
	class ShapePolyline : public AbstractShape {

	public:

		// Construction - from byte buffer
		ShapePolyline(const int recordNum, const BYTE *pBuffer, const size_t bufSize);

		// Construction - copy constructor
		ShapePolyline(const ShapePolyline &copyShape);

		// Construction - operator
		ShapePolyline & operator=( const ShapePolyline &copyShape);

		// Destruction
		virtual ~ShapePolyline();

		// Get the lines
		const CNT_POLYLINE & getLines() const { return( cntPolylines); }

		// Overrides
		virtual bool containsPoint( double x, double y) const { return( false); }

	protected:

		// The number of parts
		int numParts;

		// The number of points
		int numPoints;

		// The container of polygons
		CNT_POLYLINE cntPolylines;

	};

	// The polygon shape
	class ShapePolygon : public AbstractShape {

	public:

		// Construction - from byte buffer
		ShapePolygon(const int recordNum, const BYTE *pBuffer, const size_t bufSize);

		// Construction - from container of polygons
		ShapePolygon(const int recordNum, const CNT_POLYGON &polygons);

		// Destruction
		virtual ~ShapePolygon();

		// Overrides
		virtual bool containsPoint( double x, double y) const;

	protected:

		// The number of parts
		int numParts;

		// The number of points
		int numPoints;

		// The container of polygons
		CNT_POLYGON cntPolygons;

	};


};

#endif /* ShapeType_hpp */
