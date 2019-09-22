# libShape
A library to handle the processing of ESRI shapefiles.

# Purpose
Many government files, such as those provided by the
United States Census Bureau and by many local
municipalities are provided as ESRI shapefiles.  This
format allows the agency an easy and portable method
to communicate both geographical and database details.

# CAUTIONS
At this time the included library is only for
LITTLE-ENDIAN systems.

# Supported Shape Types
At this time only the following shape types are supported:

* Null shape
* Point
* Polyline
* Polygon

Unsupported shape types will be returned as

	libShape::ShapeInvalid

which will still allow access to the database functions.
