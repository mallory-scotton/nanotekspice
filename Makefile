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

CXXFLAGS			=	-std=c++20 -Wall -Wextra -I. -g3

###############################################################################
## Sources
###############################################################################

SOURCES				=	./Main.cpp \
						./Factory.cpp \
						./Tristate.cpp \
						./AComponent.cpp \
						./Pin.cpp \
						./Errors/ComponentException.cpp \
						./Shell/Circuit.cpp \
						./Shell/Parser.cpp \
						./Shell/Shell.cpp \
						./Specials/Input.cpp \
						./Specials/False.cpp \
						./Specials/True.cpp \
						./Specials/Clock.cpp \
						./Specials/Output.cpp \
						./Gates/Or.cpp \
						./Gates/Nor.cpp \
						./Gates/Not.cpp \
						./Gates/Nand.cpp \
						./Gates/And.cpp \
						./Gates/Xor.cpp \
						./Sequencials/FlipFlop.cpp \
						./Components/C4008.cpp \
						./Components/C4013.cpp \
						./Components/C4069.cpp \
						./Components/QuadGate.cpp

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
