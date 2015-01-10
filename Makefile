CC=g++
CFLAGS =-Wall -std=c++11 -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD)

PROGRAMS=main example
OBJECTS=compile/tinyxml2.o compile/WindScenario.o compile/WindFarmLayoutEvaluator.o compile/KusiakLayoutEvaluator.o

all:$(PROGRAMS)
.PHONY:main

main:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/main.cpp -o $@ src/GA_1.cpp src/GA_2.cpp src/GA_3.cpp $(INCLUDES) $(OPTFLAGS)
example:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/GA_example_main.cpp src/GA_example.cpp -o $@ $(INCLUDES) $(OPTFLAGS)

compile/tinyxml2.o:
	$(CC) $(CFLAGS) -c src/eval/tinyxml2.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)
compile/WindScenario.o:
	$(CC) $(CFLAGS) -c src/eval/WindScenario.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)
compile/WindFarmLayoutEvaluator.o:
	$(CC) $(CFLAGS) -c src/eval/WindFarmLayoutEvaluator.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)
compile/KusiakLayoutEvaluator.o:
	$(CC) $(CFLAGS) -c src/eval/KusiakLayoutEvaluator.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

clean:
	rm $(PROGRAMS) 2>/dev/null
