CC=g++
CFLAGS =-Wall -std=c++11 -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD)

PROGRAMS=eval_ga.o eval_es.o eval_example.o
OBJECTS=compile/tinyxml2.o compile/WindScenario.o compile/WindFarmLayoutEvaluator.o compile/KusiakLayoutEvaluator.o

all:$(PROGRAMS)
.PHONY:eval_ga.o eval_es.o

eval_ga.o:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/main_ga.cpp -o $@ src/GA.cpp $(INCLUDES) $(OPTFLAGS)
eval_es.o:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/main_es.cpp -o $@ src/ES.cpp $(INCLUDES) $(OPTFLAGS)
eval_example.o:$(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) src/GA_example.cpp src/main_example.cpp -o $@ $(INCLUDES) $(OPTFLAGS)

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
