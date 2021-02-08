APPNAME=a.out
FLAGS=-Wall -Wextra -Wfatal-errors
SRC=main.cpp tsv.cpp edit_with_ncurses.cpp
HEADERS=tsv.hpp edit_with_ncurses.hpp
LIBS=-lm -lncurses



$(APPNAME): $(SRC) $(HEADERS)
	g++ $(FLAGS) -o $(APPNAME) $(SRC) $(LIBS)
