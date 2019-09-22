#
# Makefile for libshape static library
#
# Copyright (c) 2019 Louis Gehrig. All rights reserved.
#

#
#	MIT License
#
#	Copyright (c) 2019 Louis Gehrig
#
#	Permission is hereby granted, free of charge, to any person obtaining a copy
#	of this software and associated documentation files (the "Software"), to deal
#	in the Software without restriction, including without limitation the rights
#	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
#	copies of the Software, and to permit persons to whom the Software is
#	furnished to do so, subject to the following conditions:
#
#	The above copyright notice and this permission notice shall be included in all
#	copies or substantial portions of the Software.
#
#	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
#	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
#	SOFTWARE.
#

# Certain defaults
AR = ar
CC = g++
DEFAULT_TARGET = release
INCLUDES = -I Include
TARGET ?= ${DEFAULT_TARGET}

# Specific to target
ifeq "$(TARGET)" "debug"
	BIN = bin/debug
	CC_OPTS = 
	TARGET_FILE = libShaped.a
else
	BIN = bin/release
	CC_OPTS = -O3 
	TARGET_FILE = libShape.a
endif

all: ${TARGET_FILE}

clean:
	rm -f ${TARGET_FILE} ${BIN}/* 

cleanall:
	rm -rf bin libShape.a libbShaped.a 
	mkdir bin
	mkdir bin/debug
	mkdir bin/release
	chmod 777 bin bin/debug bin/release

${TARGET_FILE} : ${BIN}/libShape.o ${BIN}/libShapeDB.o ${BIN}/libShapeFile.o
	cd ${BIN} && ${AR} -r -c ../../${TARGET_FILE} libShape.o libShapeDB.o libShapeFile.o

${BIN}/libShape.o : Include/libShape.hpp Include/libShapeDB.hpp Include/libShapeFile.hpp Src/libShape.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/libShape.o Src/libShape.cpp

${BIN}/libShapeDB.o : Include/libShapeDB.hpp Src/libShapeDB.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/libShapeDB.o Src/libShapeDB.cpp

${BIN}/libShapeFile.o : Include/libShapeFile.hpp Src/libShapeFile.cpp
	${CC} -c ${INCLUDES} ${CC_OPTS} -o ${BIN}/libShapeFile.o Src/libShapeFile.cpp
