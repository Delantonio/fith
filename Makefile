test:
	g++ main.cpp particle_effect.cpp program.cpp matrix.cpp obj_loader.cpp image.cpp image_io.cpp -lGL -lglut -lGLEW -lGLU -g -o toto

all:
	g++ main.cpp program.cpp matrix.cpp obj_loader.cpp image.cpp image_io.cpp -lGL -lglut -lGLEW -lGLU -g -o render
