build:
	gcc Main.c Array.c -o Platformer -ggdb -fms-extensions -std=c11 -Wall -Wextra -Wshadow -Wno-switch -Wno-missing-field-initializers -lSDL2 -lSDL2_ttf -lSDL2_image -lm

run:
	./Platformer

debug:
	gdb ./Platformer
