all:
	gcc -I include -L lib -o pong pong.c -lmingw32 -lSDL2main -lSDL2