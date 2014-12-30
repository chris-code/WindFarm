CC=g++
CFLAGS =-Wall -std=c++0x -g
OPTFLAGS=-O2
INCLUDES=-I$(PWD)

SRC_FOLDER=src
COMPILE_FOLDER=compile

SOURCES=$(SRC_FOLDER)/tinyxml2.cpp $(SRC_FOLDER)/WindScenario.cpp $(SRC_FOLDER)/WindFarmLayoutEvaluator.cpp $(SRC_FOLDER)/KusiakLayoutEvaluator.cpp $(SRC_FOLDER)/GA.cpp
OBJECTS=$(COMPILE_FOLDER)/tinyxml2.o $(COMPILE_FOLDER)/WindScenario.o $(COMPILE_FOLDER)/WindFarmLayoutEvaluator.o $(COMPILE_FOLDER)/KusiakLayoutEvaluator.o $(COMPILE_FOLDER)/GA.o

main:$(OBJECTS)
	$(CC) $(CFLAGS) -o eval.o $(OBJECTS) $(SRC_FOLDER)/main.cpp $(INCLUDES) $(OPTFLAGS)

$(COMPILE_FOLDER)/tinyxml2.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/tinyxml2.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

$(COMPILE_FOLDER)/WindScenario.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/WindScenario.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

$(COMPILE_FOLDER)/WindFarmLayoutEvaluator.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/WindFarmLayoutEvaluator.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

$(COMPILE_FOLDER)/KusiakLayoutEvaluator.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/KusiakLayoutEvaluator.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

$(COMPILE_FOLDER)/GA.o:
	$(CC) $(CFLAGS) -c $(SRC_FOLDER)/GA.cpp -o $@ -w $(INCLUDES) $(OPTFLAGS)

clean:
	rm $(OBJECTS) eval.o 2> /dev/null
