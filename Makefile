CFLAGS=-std=c++11 -I. -Wall
DEPS=color.h vec3.h

inOneWeekend: main.cc $(DEPS)
	g++ -o $@ main.cc $(CFLAGS)

clean:
	rm inOneWeekend
