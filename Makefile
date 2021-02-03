APPNAME=a.out
FLAGS=-Wall -Wextra -Wfatal-errors
SRC=main.cpp tsv.cpp 
HEADERS=tsv.hpp



$(APPNAME): $(SRC) $(HEADERS)
	g++ $(FLAGS) -o $(APPNAME) $(SRC)
