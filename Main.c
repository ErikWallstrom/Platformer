#include "Array.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#undef main

typedef struct
{
	SDL_Rect s_rect;
	SDL_Rect d_rect;
	SDL_Texture* texture;

	int speed, frame, direction, faced_left, force, hp;
	unsigned time, delay;
} Character;

typedef struct
{
	SDL_Rect d_rect;
	SDL_Texture* texture;
	int speed, faced_left, range, start_x, friendly;
} Bullet;

typedef enum
{
	SINGLE_PLAYER,
	MULTI_PLAYER

} Mode;

int main(void)
{
	srand((unsigned)time(0));
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Window* window = SDL_CreateWindow(
		"Platformer 0.1",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800, 600, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(
		window, -1,
		SDL_RENDERER_ACCELERATED |
		SDL_RENDERER_PRESENTVSYNC |
		SDL_RENDERER_TARGETTEXTURE);
	SDL_Texture* background_image = IMG_LoadTexture(
		renderer,
		"background.png");
	SDL_Texture* ground_image = IMG_LoadTexture(
		renderer,
		"ground.png");
	SDL_Texture* bullet_image = IMG_LoadTexture(
		renderer,
		"bullet.png");

	int done = 0;
/*
	while(!done)
	{

	}
*/
	Character player = {
		{0, 0, 40, 50},
		{300, 492, 40, 50},
		IMG_LoadTexture(
			renderer,
			"player.png"),
		.speed = 5,
		.delay = 100,
		.frame = 5,
		.hp = 140
	};

	Character enemy = {
		{0, 0, 40, 50},
		{380, 492, 40, 50},
		IMG_LoadTexture(
			renderer,
			"enemy.png"),
		.speed = 2,
		.delay = 800,
		.frame = 5,
		.hp = 80
	};

	Character medkit = {
		{0, 0, 522, 462},
		{1000, 0, 30, 20},
		IMG_LoadTexture(
			renderer,
			"medkit.png"),
		.speed = 0
	};

	Array* bullets = Array_create();
	const Uint8* key_down = SDL_GetKeyboardState(NULL);
	SDL_Event event;
	int enemies_killed = 0;
	int get_medkit = 0;
	int gravity = 1;
	done = 0;
	while(!done)
	{
		//!SDL_Delay(32);
		//Rendering
		SDL_RenderClear(renderer);
		SDL_RenderCopy(
			renderer,
			background_image,
			NULL,
			&(SDL_Rect){0, 0, 800, 600});
		SDL_RenderCopy(
			renderer,
			ground_image,
			NULL,
			&(SDL_Rect){0, 530, 800, 70});
		SDL_RenderCopy(
			renderer,
			medkit.texture,
			&medkit.s_rect,
			&medkit.d_rect
		);
		SDL_RenderCopyEx(
			renderer,
			player.texture,
			&player.s_rect,
			&player.d_rect,
			0.0, NULL,
			player.faced_left ?
				SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
		SDL_RenderCopyEx(
			renderer,
			enemy.texture,
			&enemy.s_rect,
			&enemy.d_rect,
			0.0, NULL,
			enemy.faced_left ?
				SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

		foreach(i, bullets)
		{
			Bullet* bullet = Array_get(bullets, i);
			if(bullet->faced_left)
				bullet->d_rect.x -= bullet->speed;
			else
				bullet->d_rect.x += bullet->speed;

			SDL_RenderCopyEx(
				renderer,
				bullet->texture,
				NULL,
				&bullet->d_rect,
				0.0, NULL,
				bullet->faced_left ?
					SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);

			if(abs(bullet->d_rect.x - bullet->start_x) >=
				bullet->range)
			{
				free(bullet);
				Array_remove(bullets, i);
			}

			if(bullet->friendly)
			{
				if(SDL_HasIntersection(&bullet->d_rect, &enemy.d_rect))
				{
					enemy.hp -= 16;
					free(bullet);
					Array_remove(bullets, i);
				}
			}
			else
			{
				if(SDL_HasIntersection(&bullet->d_rect, &player.d_rect))
				{
					player.hp -= 20;
					free(bullet);
					Array_remove(bullets, i);
				}
			}
		}

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer, &(SDL_Rect){
			(int)(800 - 20 - enemy.hp / 80.0 * 200), 20, (int)(enemy.hp / 80.0 * 200), 20
		});
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderFillRect(renderer, &(SDL_Rect){
			20, 20, (int)(player.hp / 140.0 * 200), 20
		});
		SDL_RenderPresent(renderer);

		//Events
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_QUIT:
				done = 1;
			break;

			case SDL_KEYUP:
				switch(event.key.keysym.scancode)
				{
				case SDL_SCANCODE_A:
				case SDL_SCANCODE_D:
					player.direction = 0;
					player.frame = 5;
				break;
				case SDL_SCANCODE_P:
					SDL_ShowSimpleMessageBox(
						SDL_MESSAGEBOX_INFORMATION,
						"Game Paused", "Press OK to resume game", NULL);
				}
			break;
			}
		}

		//Keyboardhttp://stackoverflow.com/questions/1485505/representing-dynamic-typing-in-c
		if(key_down[SDL_SCANCODE_W])
		{
			if(player.d_rect.y == 492)
				player.force = -15;
		}
		if(key_down[SDL_SCANCODE_A])
		{
			player.direction = -1;
			player.faced_left = 1;

			if(SDL_GetTicks() - player.delay > player.time)
			{
				player.time = SDL_GetTicks();
				player.frame++;

				if(player.frame >= 4)
				{
					player.frame = 0;
				}
			}
		}
		if(key_down[SDL_SCANCODE_D])
		{
			player.direction = 1;
			player.faced_left = 0;
			if(SDL_GetTicks() - player.delay > player.time)
			{
				player.time = SDL_GetTicks();
				player.frame++;
				if(player.frame >= 4)
				{
					player.frame = 0;
				}
			}
		}
		if(key_down[SDL_SCANCODE_SPACE])
		{
			static unsigned shoot_time = 0;
			if(SDL_GetTicks() / 400 > shoot_time)
			{
				shoot_time = SDL_GetTicks() / 400;
				Bullet* bullet = malloc(sizeof(Bullet));
				*bullet = (Bullet){
					{
						player.d_rect.x + player.d_rect.w / 2 + 4,
						player.d_rect.y + player.d_rect.h / 2 - 6,
						8, 8
					},
					bullet_image,
					.speed = 9,
					.faced_left = player.faced_left,
					.range = 800,
					.start_x = player.d_rect.x,
					.friendly = 1
				};
				Array_insert(bullets, 0, bullet);
			}
		}

		//AI
		if(SDL_GetTicks() - enemy.time > enemy.delay)
		{
			int action = rand() % 10000;
			if(action > 7000)
			{
				enemy.time = SDL_GetTicks();
				Bullet* e_bullet = malloc(sizeof(Bullet));
				*e_bullet = (Bullet){
					{
						enemy.d_rect.x + enemy.d_rect.w / 2 + 4,
						enemy.d_rect.y + enemy.d_rect.h / 2 - 6,
						8, 8
					},
					bullet_image,
					.speed = 5,
					.faced_left = enemy.faced_left,
					.range = 800,
					.start_x = enemy.d_rect.x,
					.friendly = 0
				};
				Array_insert(bullets, 0, e_bullet);
			}
			else if(action > 6000)
			{
				if(enemy.d_rect.y == 492)
					enemy.force = -15;
				enemy.faced_left = (rand() % 1000 > 500) ? 0 : 1;
			}
			else
			{
				int r = rand() % 10000;
				if(r > 7000)
					enemy.direction = 0;
				else if(r > 4000)
					enemy.direction = 1;
				else
					enemy.direction = -1;
			}
		}

		//Gravity
		medkit.force += gravity;
		medkit.d_rect.y += medkit.force;
		if(medkit.d_rect.y >= 517)
		{
			medkit.d_rect.y = 517;
			medkit.force = 0;
		}

		player.force += gravity;
		player.d_rect.y += player.force;
		if(player.d_rect.y >= 492)
		{
			player.d_rect.y = 492;
			player.force = 0;
		}

		enemy.force += gravity;
		enemy.d_rect.y += enemy.force;
		if(enemy.d_rect.y >= 492)
		{
			enemy.d_rect.y = 492;
			enemy.force = 0;
		}

		//Movement
		player.d_rect.x += player.direction * player.speed;
		enemy.d_rect.x += enemy.direction * enemy.speed;

		//Animation
		player.s_rect.x = player.frame * 40;
		enemy.s_rect.x = enemy.frame * 40;

		//Collision detection
		if(enemy.d_rect.x + enemy.d_rect.w > 800)
		{
			enemy.d_rect.x = 800 - enemy.d_rect.w;
		}
		else if(enemy.d_rect.x < 0)
		{
			enemy.d_rect.x = 0;
		}

		if(player.d_rect.x + player.d_rect.w > 800)
		{
			player.d_rect.x = 800 - player.d_rect.w;
		}
		else if(player.d_rect.x < 0)
		{
			player.d_rect.x = 0;
		}

		if(SDL_HasIntersection(&medkit.d_rect, &player.d_rect))
		{
			player.hp += 40;
			if(player.hp > 140)
				player.hp = 140;
			medkit.d_rect.y = 0;
			medkit.d_rect.x = 1000;
		}
		if(!(enemies_killed % 4) && enemies_killed)
		{
			static int gotten = 0;
			if(enemies_killed != gotten)
			{
				gotten = enemies_killed;
				get_medkit = 1;
			}
		}

		if(get_medkit)
		{
			get_medkit = 0;
			medkit.d_rect.y = 0;
			medkit.d_rect.x = rand() % 700 + 50;
		}

		if(player.hp <= 0)
		{
			int len = snprintf(NULL, 0,
				"You died! You killed %i enemies!",
				enemies_killed);
			char buffer[len];
			sprintf(buffer,
				"You died! You killed %i enemies!",
				enemies_killed);
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"Game Over", buffer, NULL);
			done = 1;
		}
		else if(enemy.hp <= 0)
		{
			enemy.d_rect.x = rand() % 800;
			enemy.d_rect.y = rand() % 600 - 200;
			enemy.hp = 80;
			enemy.delay -= 25;
			enemies_killed++;
			if(!(enemies_killed % 2))
				enemy.speed++;
		}
	}

	foreach(i, bullets)
	{
		free(Array_get(bullets, i));
	}
	Array_destroy(&bullets);

	SDL_DestroyTexture(background_image);
	SDL_DestroyTexture(ground_image);
	SDL_DestroyTexture(player.texture);
	SDL_DestroyTexture(bullet_image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}
