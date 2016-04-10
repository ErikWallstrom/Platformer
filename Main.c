#include "Array.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

typedef struct
{
	SDL_Rect s_rect;
	SDL_Rect d_rect;
	SDL_Texture* texture;

	int speed, frame, direction, faced_left, force;
	unsigned time, delay;
} Character;

typedef struct
{
	SDL_Rect d_rect;
	SDL_Texture* texture;
	int speed, faced_left, range, start_x;
} Bullet;

int main(void)
{
	SDL_Init(SDL_INIT_EVERYTHING);
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

	Character player = {
		{0, 0, 40, 50},
		{380, 492, 40, 50},
		IMG_LoadTexture(
			renderer,
			"sprite_sheet.png"),
		.speed = 4,
		.delay = 100,
		.frame = 5,
	};

	Array* bullets = Array_create();
	const Uint8* key_down = SDL_GetKeyboardState(NULL);
	SDL_Event event;
	int gravity = 1;
	int done = 0;
	while(!done)
	{
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
				}
			break;
			}
		}

		//Keyboard
		if(key_down[SDL_SCANCODE_SPACE])
		{
			static unsigned fire_time = 0;
			if(SDL_GetTicks() / 250 > fire_time)
			{
				fire_time = SDL_GetTicks() / 250;
				Bullet* bullet = malloc(sizeof(Bullet));
				*bullet = (Bullet){
					{
						player.d_rect.x + player.d_rect.w / 2 + 4, 
						player.d_rect.y + player.d_rect.h / 2 - 6, 
						8, 8
					},
					bullet_image,
					10, player.faced_left, 500, player.d_rect.x
				};
				Array_insert(bullets, 0, bullet);
			}
		}
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

		//Gravity
		player.force += gravity;
		player.d_rect.y += player.force;
		if(player.d_rect.y >= 492)
		{
			player.d_rect.y = 492;
			player.force = 0;
		}

		//Movement
		player.d_rect.x += player.direction * player.speed;

		//Animation
		player.s_rect.x = player.frame * 40;

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
		SDL_RenderCopyEx(
			renderer, 
			player.texture, 
			&player.s_rect,
			&player.d_rect,
			0.0, NULL,
			player.faced_left ? 
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
		}
		SDL_RenderPresent(renderer);
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
	SDL_Quit();
}
