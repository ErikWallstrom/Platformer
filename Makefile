build:
	gcc Main.c Array.c -o Platformer -ggdb -std=c11 -Wall -Wextra -Wshadow -Wno-switch -lSDL2 -lSDL2_image -lm

run:
	./Platformer

debug:
	gdb ./Platformer
