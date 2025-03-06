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

CXXFLAGS			=	-std=c++20 -Wall -Wextra -I.

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
						./Sequencials/Counter.cpp \
						./Components/C2716.cpp \
						./Components/C4008.cpp \
						./Components/C4013.cpp \
						./Components/C4017.cpp \
						./Components/C4040.cpp \
						./Components/C4069.cpp \
						./Components/C4094.cpp \
						./Components/C4512.cpp \
						./Components/QuadGate.cpp

BONUS_SOURCES		=	./Bonus/Bonus.cpp \
						./Bonus/Matrix.cpp \
						./Bonus/DigitalInput.cpp \
						./External/ImGui/imgui_draw.cpp \
						./External/ImGui/imgui_tables.cpp \
						./External/ImGui/imgui_widgets.cpp \
						./External/ImGui/imgui_stdlib.cpp \
						./External/ImGui/imgui-SFML.cpp \
						./External/ImGui/imgui.cpp \
						./External/ImGui/ImNodes.cpp \
						./External/ImGui/ImNodesEz.cpp

OBJECTS				=	$(SOURCES:.cpp=.o)
BONUS_OBJECTS		=	$(BONUS_SOURCES:.cpp=.o)

EXTERNAL_LIBS		=	-IExternal/SFML/include \
						-IExternal/ImGui \
						-DSFML_STATIC \
						-LExternal/SFML/build/lib \
						-lsfml-audio-s \
						-lsfml-graphics-s \
						-lsfml-window-s \
						-lsfml-system-s \
						-lGL \
						-lz \
						-lm \
						-lX11 \
						-lXrandr \
						-lXcursor \
						-lXinerama \
						-ludev \
						-lXi \
						-lfreetype \
						-lFLAC \
						-logg \
						-lvorbis \
						-lvorbisfile \
						-lvorbisenc

SFML_COMPILATION	:=	cd External/SFML && \
						cmake -S . -B build -DBUILD_SHARED_LIBS=OFF \
						> /dev/null && cmake --build build > /dev/null

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

external:
	$(SFML_COMPILATION)

bonus: CXXFLAGS	+=	$(EXTERNAL_LIBS) -DNTS_BONUS
bonus: SOURCES += $(BONUS_SOURCES)
bonus: external $(BONUS_OBJECTS) build

clean:
	rm -rf $(OBJECTS) $(BONUS_OBJECTS)

fclean: clean
	rm -f $(TARGET)

re: fclean build

.PHONY: all build debug clean fclean re

deps:
	@echo "Installing dependencies..."
	@set -e; \
	if command -v apt > /dev/null; then \
		echo "Detected apt-based system"; \
		sudo apt update && sudo apt install -y \
			libflac-dev \
			libvorbis-dev \
			libfreetype6-dev \
			libudev-dev \
			libgl1-mesa-dev \
			libx11-dev \
			libxrandr-dev \
			libxcursor-dev \
			libxinerama-dev \
			libxi-dev \
			zlib1g-dev \
			libogg-dev \
			libvorbisfile3 \
			libvorbisenc2; \
	elif command -v dnf > /dev/null; then \
		echo "Detected dnf-based system"; \
		sudo dnf check-update || true; \
		sudo dnf install -y \
			flac-devel \
			libvorbis-devel \
			freetype-devel \
			libudev-devel \
			mesa-libGL-devel \
			libX11-devel \
			libXrandr-devel \
			libXcursor-devel \
			libXinerama-devel \
			libXi-devel \
			zlib-devel \
			libogg-devel \
			libvorbisfile-devel \
			libvorbisenc-devel; \
	else \
		echo "No supported package manager found."; \
		exit 1; \
	fi
