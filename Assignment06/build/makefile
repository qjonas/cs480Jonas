# Linux
CC=g++ 
LIBS= -lglut -lGLEW -lGL -lassimp 'Magick++-config --cppflags --cxxflags --ldflags --libs'

# For Macs uncomment the next line and comment out the previous one
#CC=clang++
#LIBS=  -framework OpenGL -framework GLUT -framework Cocoa -lGLEW -stdlib=libc++
# Assuming you want to use a recent compiler

# Compiler flags
CXXFLAGS= -g -Wall -std=c++0x $(DEFS)

all: ../bin/Matrix
 
../bin/Matrix: ../src/loader.cpp ../src/main.cpp 
	$(CC) $(CXXFLAGS) ../src/loader.cpp ../src/main.cpp -o ../bin/Matrix $(LIBS) 

make clean: 
	rm *.o ../src/Matrix



