APPNAME=a.out
FLAGS=-Wall -Wextra
SRC=main.cpp



$(APPNAME): $(SRC)
	g++ $(FLAGS) -o $(APPNAME) $(SRC)
