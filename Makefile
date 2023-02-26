all:
	g++ -I include -L lib -o 01 01.cpp -lmingw32 -lSDL2main -lSDL2