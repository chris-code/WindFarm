CC=g++
CFLAGS =-Wall -std=c++11 -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD)

PROGRAMS=ga_1.o ga_2.o ga_example.o
OBJECTS=compile/tinyxml2.o compile/WindScenario.o compile/WindFarmLayoutEvaluator.o compile/KusiakLayoutEvaluator.o

all:$(PROGRAMS)
.PHONY:ga_1.o ga_2.o

ga_1.o:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/GA_1_main.cpp -o $@ src/GA_1.cpp $(INCLUDES) $(OPTFLAGS)
ga_2.o:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/GA_2_main.cpp -o $@ src/GA_2.cpp $(INCLUDES) $(OPTFLAGS)
ga_example.o:$(OBJECTS)
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
