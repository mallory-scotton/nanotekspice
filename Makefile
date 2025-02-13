###############################################################################
###
### MIT License
###
### Copyright(c) 2025 TekyoDrift
###
### Permission is hereby granted, free of charge, to any person obtaining a
### copy of this software and associated documentation files (the "Software"),
### to deal in the Software without restriction, including without limitation
### the rights to use, copy, modify, merge, publish, distribute, sublicense,
### and/or sell copies of the Software, and to permit persons to whom the
### Software is furnished to do so, subject to the following coditions:
###
### The above copyright notice and this permission notice shall be included
### in all copies or substantial portions of the Software?
###
### THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
### IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
### FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
### AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
### LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
### FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
### DEALINGS IN THE SOFTWARE.
###
###############################################################################

###############################################################################
## Configuration
###############################################################################

TARGET				=	nanotekspice

CXX					=	g++

CXXFLAGS			=	-std=c++20 -Wall -Wextra -Iinclude

###############################################################################
## Sources
###############################################################################

SOURCES				=	source/AComponent.cpp \
						source/ComponentFactory.cpp \
						source/Circuit.cpp \
						source/CircuitParser.cpp \
						source/CircuitSimulator.cpp \
						source/components/AndGateComponent.cpp \
						source/components/C4081Component.cpp \
						source/components/C4001Component.cpp \
						source/components/C4011Component.cpp \
						source/components/C4030Component.cpp \
						source/components/C4069Component.cpp \
						source/components/C4071Component.cpp \
						source/components/C4008Component.cpp \
						source/components/C4013Component.cpp \
						source/components/C4017Component.cpp \
						source/components/C4040Component.cpp \
						source/components/C4094Component.cpp \
						source/components/C4512Component.cpp \
						source/components/C4514Component.cpp \
						source/components/C4801Component.cpp \
						source/components/C2716Component.cpp \
						source/components/ClockComponent.cpp \
						source/components/FalseComponent.cpp \
						source/components/GateComponent.cpp \
						source/components/InputComponent.cpp \
						source/components/NotGateComponent.cpp \
						source/components/OrGateComponent.cpp \
						source/components/OutputComponent.cpp \
						source/components/TrueComponent.cpp \
						source/components/XorGateComponent.cpp \
						source/main.cpp

OBJECTS				=	$(SOURCES:.cpp=.o)

###############################################################################
## Makefile logic
###############################################################################

all: build

%.o: %.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

build: $(OBJECTS)
	$(CXX) -o $(TARGET) $(OBJECTS) $(CXXFLAGS)

debug: CXXFLAGS += -g3
debug: build

clean:
	find . -type f -iname "*.o" -delete
	find . -type f -iname "*.d" -delete
	find . -type f -iname "*.gcda" -delete
	find . -type f -iname "*.gcno" -delete
	find . -type f -iname "*.html" -delete
	find . -type f -iname "*.css" -delete

fclean: clean
	rm -f $(TARGET)

re: fclean build

.PHONY: all build debug clean fclean re
