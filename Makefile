CFLAGS=-std=c++11 -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow -I.
DEPS=color.h vec3.h

inOneWeekend: main.cc $(DEPS)
	g++ -o $@ main.cc $(CFLAGS)

run: inOneWeekend
	./inOneWeekend > image.ppm

.PHONY: clean

clean:
	rm inOneWeekend
