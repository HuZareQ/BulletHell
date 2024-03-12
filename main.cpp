#define _USE_MATH_DEFINES
#include<math.h>
#include<stdio.h>
#include<string.h>
#include <stdlib.h>
#include <time.h>

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"

}

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768
#define SPEED 5
#define BACKGROUND_HEIGHT 4768
#define BACKGROUND_WIDTH 2392
#define FPS 60
#define BACKGROUND_SPEED 8
#define SIZETAB 200
#define TabRes 50
#define BULLET_SPEED 10
#define MaxExecutionNumber 100
#define ENEMY_SPEED 3
#define SHIPS_NUMBER 4

struct Position_t {
	int x;
	int y;
};

struct Holder_t{
	int indexTab;
	int indexPlayer;
	int executionNumber;
	int k;
	int execution;
	int wave;
	int type;
	int key;
	int HitsToPlayer;
	int HitsToEnemy;
	int time;
	int spawn;
	double worldTime;
	double timeToSpawn;
};

void Reset(Position_t* player, SDL_Rect* camera, Position_t* enemy, Holder_t* hold, double tab[SHIPS_NUMBER][SIZETAB]);
void DrawString(SDL_Surface* screen, int x, int y, const char* text, SDL_Surface* charset); 
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y);
void Drawbackground(SDL_Surface* screen, SDL_Surface* background, SDL_Rect* camera, Position_t* player);
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color);
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color);
void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k, Uint32 outlineColor, Uint32 fillColor);
void ShootSpam(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, Position_t* player, SDL_Surface* statek);
void ShootWave(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t* player, SDL_Surface* statek, Holder_t* hold);
void SimpleShoot(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t* player, SDL_Surface* statek, Holder_t* hold);
void Shoot(Position_t* enemy, double tab[SHIPS_NUMBER][SIZETAB], int speed, int type, int ind, Holder_t* hold, int interruption);
void enemy1(SDL_Surface* screen, Position_t* enemy, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, int atack, Position_t* player, SDL_Surface* statek);
void enemy2(SDL_Surface* screen, Position_t* enemy, Position_t* player, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, int atack);
void enemy3(SDL_Surface* screen, Position_t* enemy, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, SDL_Surface* sprite, int atack, Position_t* player, SDL_Surface* statek);
void ShootPlayer( Position_t* player, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold);
void SpawnEnemy(SDL_Surface* screen, SDL_Surface* przeciwnik, Position_t* enemy, SDL_Surface* statek, Position_t* player, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, SDL_Rect *camera);
void Bullet(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t* enemy, SDL_Surface* przeciwnik, Holder_t* hold);
void MaxX(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB]);
void MinX(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB]);
void move(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB]);

// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char **argv) {
	int t1, t2, quit, rc;
	double delta, tab[SHIPS_NUMBER][SIZETAB];
	SDL_Event event;
	SDL_Surface *screen, *charset, *background;
	SDL_Surface *statek, *przeciwnik[4], *naboje;
	SDL_Texture *scrtex;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect camera;
	Position_t player, enemy;
	Holder_t hold;

	if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		printf("SDL_Init error: %s\n", SDL_GetError());
		return 1;
		}

	// tryb pe³noekranowy / fullscreen mode
//	rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
//	                                 &window, &renderer);
	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
	                                 &window, &renderer);
	if(rc != 0) {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
		};
	
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_SetWindowTitle(window, "Zadanie nr 2");

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	// wy³¹czenie widocznoœci kursora myszy
	SDL_ShowCursor(SDL_DISABLE);

	// wczytanie obrazka cs8x8.bmp
	charset = SDL_LoadBMP("./cs8x8.bmp");
	
	SDL_SetColorKey(charset, true, 0x000000);

	statek = SDL_LoadBMP("./statek100.bmp");

	przeciwnik[1] = SDL_LoadBMP("./przeciwnik1.bmp");
	przeciwnik[2] = SDL_LoadBMP("./przeciwnik2.bmp");
	przeciwnik[3] = SDL_LoadBMP("./przeciwnik3.bmp");

	naboje = SDL_LoadBMP("./naboje.bmp");

	background = SDL_LoadBMP("./background4.bmp");

	char text[128];
	int czarny = SDL_MapRGB(screen->format, 0x00, 0x00, 0x00);
	int zielony = SDL_MapRGB(screen->format, 0x00, 0xFF, 0x00);
	int czerwony = SDL_MapRGB(screen->format, 0xFF, 0x00, 0x00);
	int niebieski = SDL_MapRGB(screen->format, 0x11, 0x11, 0xCC);

	t1 = SDL_GetTicks();

	quit = 0;
	Reset(&player, &camera, &enemy, &hold, tab);
	hold.wave = 0;

	while(!quit) {
		t2 = SDL_GetTicks();

		delta = (t2 - t1) * 0.001;
		t1 = t2;

		hold.worldTime += delta;

		SDL_FillRect(screen, NULL, czarny);

		switch (hold.wave)
		{
		case 0:
		{
			przeciwnik[0] = przeciwnik[1];
			hold.type = 1;
			break;
		}
		case 1:
		{
			przeciwnik[0] = przeciwnik[2];
			hold.type = 2;
			break;
		}
		case 2:
		{
			przeciwnik[0] = przeciwnik[3];
			hold.type = 3;
			break;
		}
		}
		
		Drawbackground(screen, background, &camera, &player);
		DrawSurface(screen, statek, player.x, player.y);
		SpawnEnemy(screen, przeciwnik[0], &enemy, statek, &player, naboje, tab, &hold, &camera);
		Bullet(screen, naboje, tab, &enemy, przeciwnik[0], &hold);

		// tekst informacyjny / info text
		DrawRectangle(screen, 4, 4, SCREEN_WIDTH - 8, 36, czerwony, niebieski);

		sprintf(text, "Czas trwania = %.1lf s ,Gracz trafiony %d razy, Przeciwnik trafiony %d razy", hold.worldTime, hold.HitsToPlayer, hold.HitsToEnemy);
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 10, text, charset);

		sprintf(text, "Esc - wyjscie, n - restart, \032 - lewo, \033 - prawo, \030 - gora, \031 - dol, spacja - strzal");
		DrawString(screen, screen->w / 2 - strlen(text) * 8 / 2, 26, text, charset);

		SDL_UpdateTexture(scrtex, NULL, screen->pixels, screen->pitch);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, scrtex, NULL, NULL);
		SDL_RenderPresent(renderer);
		// handling of events
		move(&player, &enemy, statek, &camera, tab);
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_SPACE) {
					if (hold.key == 0)
					{
						hold.key = 1;
						ShootPlayer(&player, tab, &hold);
					}
				}
				else if (event.key.keysym.sym == SDLK_ESCAPE) quit = 1;
				else if (event.key.keysym.sym == SDLK_n){
					Reset(&player, &camera, &enemy, &hold, tab);
					hold.wave = 0;
				}
				else if (event.key.keysym.sym == SDLK_w) {
					Reset(&player, &camera, &enemy, &hold, tab);
					if (hold.wave < 2)hold.wave++;
					else hold.wave = 0;
				}
				break;
			case SDL_KEYUP:
				hold.key = 0;
				break;
			case SDL_QUIT:
				quit = 1;
				break;
			};
		}
		if (1000/FPS>SDL_GetTicks()-t2)
		{
			SDL_Delay(1000 / FPS - (SDL_GetTicks() - t2));
		}
	}
		

	// zwolnienie powierzchni / freeing all surfaces
	SDL_FreeSurface(charset);
	SDL_FreeSurface(screen);
	SDL_DestroyTexture(scrtex);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
};

void Reset(Position_t* player, SDL_Rect* camera, Position_t* enemy, Holder_t* hold, double tab[SHIPS_NUMBER][SIZETAB])
{
	player->x = SCREEN_WIDTH / 2;
	player->y = SCREEN_HEIGHT / 1.2;
	enemy->x = SCREEN_WIDTH / 2;
	enemy->y = -200;
	camera->x = BACKGROUND_WIDTH/3;
	camera->y = BACKGROUND_HEIGHT - SCREEN_HEIGHT;
	camera->w = SCREEN_WIDTH;
	camera->h = SCREEN_HEIGHT;
	hold->worldTime = 0;
	hold->spawn = 5;
	hold->timeToSpawn = 0;
	hold->indexTab = 0;
	hold->indexPlayer = 0;
	hold->executionNumber = 0;
	hold->execution = 0;
	hold->k = 1;
	hold->type = 0;
	hold->key = 0;
	hold->HitsToPlayer=0;
	hold->HitsToEnemy=0;
	for (int i = 0; i < SHIPS_NUMBER; i++)
		for (int j = 0; j < TabRes; j++)
		{
			tab[i][2 * j] = '/0';			//"czyszczenie" tablicy (dla \0 == 0)
			tab[i][2 * j + 1] = '/0';
		}


}

// draw a text txt on surface screen, starting from the point (x, y)
// charset is a 128x128 bitmap containing character images
void DrawString(SDL_Surface* screen, int x, int y, const char* text,
	SDL_Surface* charset) {
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8;
	d.h = 8;
	while (*text) {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x;
		d.y = y;
		SDL_BlitSurface(charset, &s, screen, &d);
		x += 8;
		text++;
	};
};

// draw a surface sprite on a surface screen in point (x, y)
// (x, y) is the center of sprite on screen
void DrawSurface(SDL_Surface* screen, SDL_Surface* sprite, int x, int y) {
	SDL_Rect dest;
	int positionX = x - sprite->w / 2;
	int positionY = y - sprite->h / 2;
	dest.x = positionX;
	dest.y = positionY;

	dest.w = sprite->w;
	dest.h = sprite->h;
	SDL_BlitSurface(sprite, NULL, screen, &dest);
};

void Drawbackground(SDL_Surface* screen, SDL_Surface* background, SDL_Rect* camera, Position_t* player) {
	if (camera->y == SCREEN_HEIGHT / 4)
	{
		camera->y = BACKGROUND_HEIGHT - SCREEN_HEIGHT;
	}
	else
		camera->y -= BACKGROUND_SPEED;
	SDL_BlitSurface(background, camera, screen, NULL);
};

// draw a single pixel
void DrawPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;
	*(Uint32*)p = color;
};

// draw a vertical (when dx = 0, dy = 1) or horizontal (when dx = 1, dy = 0) line
void DrawLine(SDL_Surface* screen, int x, int y, int l, int dx, int dy, Uint32 color) {
	for (int i = 0; i < l; i++) {
		DrawPixel(screen, x, y, color);
		x += dx;
		y += dy;
	};
};

void DrawRectangle(SDL_Surface* screen, int x, int y, int l, int k,
	Uint32 outlineColor, Uint32 fillColor) {
	int i;
	DrawLine(screen, x, y, k, 0, 1, outlineColor);
	DrawLine(screen, x + l - 1, y, k, 0, 1, outlineColor);
	DrawLine(screen, x, y, l, 1, 0, outlineColor);
	DrawLine(screen, x, y + k - 1, l, 1, 0, outlineColor);
	for (i = y + 1; i < y + k - 1; i++)
		DrawLine(screen, x + 1, i, l - 2, 1, 0, fillColor);
};

void ShootSpam(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, Position_t* player, SDL_Surface* statek)
{
	for (int i = 0, j = -2; i < 2 * TabRes; i++, j += hold->k)
	{

		tab[3][2 * i] += 3 * j;
		tab[3][2 * i + 1] += BULLET_SPEED;
		if (hold->timeToSpawn <= hold->worldTime)
		{
			if (tab[3][2 * i] >= player->x - statek->w / 2 && tab[3][2 * i] <= player->x + statek->w / 2 &&
				tab[3][2 * i + 1] >= player->y - statek->h / 2 && tab[3][2 * i + 1] <= player->y + statek->h / 2)
			{
				hold->timeToSpawn = hold->spawn + hold->worldTime;
				hold->HitsToPlayer++;
				tab[3][2 * i] = '/0';
				tab[3][2 * i + 1] = '/0';
			}
		}
		DrawSurface(screen, naboje, tab[3][2 * i], tab[3][2 * i + 1]);
		if (hold->execution == 5)
		{
			hold->k *= -1;
			hold->execution = 0;
		}
		hold->execution++;
	}
}

void ShootWave(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t* player, SDL_Surface* statek, Holder_t* hold)
{
	for (int i = 0, j = -2; i < TabRes; i++, j += 1)
	{
		tab[2][2 * i] += 3 * j;
		tab[2][2 * i + 1] += BULLET_SPEED;
		if (j == 2)j = -3;
		if (hold->timeToSpawn <= hold->worldTime)
		{
			if (tab[2][2 * i] >= player->x - statek->w / 2 && tab[2][2 * i] <= player->x + statek->w / 2 &&
				tab[2][2 * i + 1] >= player->y - statek->h / 2 && tab[2][2 * i + 1] <= player->y + statek->h / 2)
			{
				hold->timeToSpawn = hold->spawn + hold->worldTime;
				hold->HitsToPlayer++;
				tab[2][2 * i] = '/0';
				tab[2][2 * i + 1] = '/0';
			}
		}
		DrawSurface(screen, naboje, tab[2][2 * i], tab[2][2 * i + 1]);
	}
}

void SimpleShoot(SDL_Surface* screen, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t *player, SDL_Surface* statek, Holder_t* hold)
{
	for (int i = 0; i < TabRes; i++)
	{
		tab[1][2 * i + 1] += BULLET_SPEED;

		if (hold->timeToSpawn <= hold->worldTime)
		{
			if (tab[1][2 * i] >= player->x - statek->w / 2 && tab[1][2 * i] <= player->x + statek->w / 2 &&
				tab[1][2 * i + 1] >= player->y - statek->h / 2 && tab[1][2 * i + 1] <= player->y + statek->h / 2)
			{
				hold->timeToSpawn = hold->spawn + hold->worldTime;
				hold->HitsToPlayer++;
				tab[1][2 * i] = '/0';
				tab[1][2 * i + 1] = '/0';
			}
		}
		DrawSurface(screen, naboje, tab[1][2 * i], tab[1][2 * i + 1]);  
	}
}

void Shoot(Position_t* enemy, double tab[SHIPS_NUMBER][SIZETAB], int speed, int type, int ind, Holder_t* hold, int interruption)
{
	if (hold->executionNumber < interruption)
	{
		if (hold->executionNumber % speed == 0)
		{
			for (int i = 0; i < type; i++)
			{
				tab[ind][2 * hold->indexTab] = enemy->x;
				tab[ind][2 * hold->indexTab + 1] = enemy->y;

				if (hold->indexTab < TabRes - 1) hold->indexTab++;
				else hold->indexTab = 0;
			}

		}
	}
	if (hold->executionNumber < MaxExecutionNumber)hold->executionNumber++;
	else hold->executionNumber = 0;
}

void enemy1(SDL_Surface* screen, Position_t* enemy, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, int atack, Position_t* player, SDL_Surface* statek)
{
	if (atack == 0) {
		Shoot(enemy, tab, 15, 1, 1, hold, MaxExecutionNumber);			//simple shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
	if (atack == 1) {
		Shoot(enemy, tab, 50, 5, 2, hold, MaxExecutionNumber);			//wave shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
	if (atack == 2) {
		Shoot(enemy, tab, 20, 1, 3, hold, MaxExecutionNumber);			//shoot spam
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
}

void enemy2(SDL_Surface* screen, Position_t* enemy, Position_t* player, SDL_Surface* naboje, 
	double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, int atack, SDL_Surface* statek)
{
	enemy->x = player->x;
	if (atack == 0) {
		Shoot(enemy, tab, 5, 1, 1, hold, 30);			//simple shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
	}
	if (atack == 1) {
		Shoot(enemy, tab, 50, 5, 2, hold, MaxExecutionNumber);			//wave shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
	}
	if (atack == 2) {
		Shoot(enemy, tab, 50, 5, 2, hold, MaxExecutionNumber);			//wave shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
	}
}

void moveEnemy(Position_t* enemy, SDL_Surface* sprite)
{
	int move;
	srand(time(NULL));
	move = rand() % 5;
	switch (move)
	{
	case 1:
	{
		if (enemy->x > 0 + sprite->w / 2) enemy->x -= ENEMY_SPEED;
		break;
	}
	case 2:
	{
		if (enemy->x < BACKGROUND_WIDTH - SCREEN_WIDTH - sprite->w / 2) enemy->x += ENEMY_SPEED;
		break;
	}
	case 3:
	{
		if (enemy->y < SCREEN_HEIGHT / 2) enemy->y += ENEMY_SPEED;
		break;
	}
	case 4:
	{
		if (enemy->y > sprite->w / 2) enemy->y -= ENEMY_SPEED;
		break;
	}
	}
}

void enemy3(SDL_Surface* screen, Position_t* enemy, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], 
	Holder_t* hold, SDL_Surface* sprite, int atack, Position_t* player, SDL_Surface* statek)
{
	moveEnemy(enemy, sprite);
	if (atack == 0) {
		Shoot(enemy, tab, 5, 1, 1, hold, 30);			//simple shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
	if (atack == 1) {
		Shoot(enemy, tab, 50, 5, 2, hold, MaxExecutionNumber);			//wave shoot
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
	if (atack == 2) {
		Shoot(enemy, tab, 20, 1, 3, hold, MaxExecutionNumber);			//shoot spam
		SimpleShoot(screen, naboje, tab, player, statek, hold);
		ShootWave(screen, naboje, tab, player, statek, hold);
		ShootSpam(screen, naboje, tab, hold, player, statek);
	}
}

void ShootPlayer(Position_t* player, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold)
{
	tab[0][2 * hold->indexPlayer] = player->x;
	tab[0][2 * hold->indexPlayer + 1] = player->y;

	if (hold->indexPlayer < TabRes - 1) hold->indexPlayer++;
	else hold->indexPlayer = 0;
}

void SpawnEnemy(SDL_Surface* screen, SDL_Surface* przeciwnik, Position_t* enemy, SDL_Surface* statek,
	Position_t* player, SDL_Surface* naboje, double tab[SHIPS_NUMBER][SIZETAB], Holder_t* hold, SDL_Rect *camera)
{
	double timeToSpawn;
	int atack;
	timeToSpawn = hold->spawn - hold->worldTime;

	if (timeToSpawn <= 0)
	{
		DrawSurface(screen, przeciwnik, enemy->x, enemy->y);
		if (enemy->y <= SCREEN_HEIGHT / 5)
		{
			enemy->y += 5;
		}
		else
		{
			srand(time(NULL));
			atack = rand() % 3;
			if (hold->type == 1) enemy1(screen, enemy, naboje, tab, hold, atack, player, statek);
			if (hold->type == 2) enemy2(screen, enemy, player, naboje, tab, hold, atack, statek);
			if (hold->type == 3) enemy3(screen, enemy, naboje, tab, hold, przeciwnik, atack, player, statek);

		}
	}
}

void Bullet(SDL_Surface* screen, SDL_Surface *naboje, double tab[SHIPS_NUMBER][SIZETAB], Position_t *enemy, SDL_Surface *przeciwnik, Holder_t *hold)
{
	for (int i = 0; i < TabRes; i++)
	{
		tab[0][2 * i + 1] -= BULLET_SPEED;
		if (tab[0][2 * i] >= enemy->x - przeciwnik->w / 2 && tab[0][2 * i] <= enemy->x + przeciwnik->w / 2 &&
			tab[0][2 * i + 1] >= enemy->y - przeciwnik->h / 2 && tab[0][2 * i + 1] <= enemy->y + przeciwnik->h / 2)
		{
			hold->HitsToEnemy++;
			tab[0][2 * i] = '/0';
			tab[0][2 * i + 1] = '/0';
		}
		DrawSurface(screen, naboje, tab[0][2 * i], tab[0][2 * i + 1]);
	}
}

void MaxX(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB])
{
	if (player->x < SCREEN_WIDTH - sprite->w / 2) {
		player->x += 1.8 * SPEED;
	}
	if (camera->x < BACKGROUND_WIDTH - SCREEN_WIDTH - sprite->w / 2)
	{
		camera->x += SPEED;
		enemy->x -= SPEED;
		player->x -= SPEED;
		for (int i = 0; i < SHIPS_NUMBER; i++)
			for (int j = 0; j < TabRes; j++)
				tab[i][2 * j] -= SPEED;
	}
}

void MinX(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB])
{
	if (player->x > sprite->w / 2) {
		player->x -= 1.8 * SPEED;
	}
	if (camera->x > 280)
	{
		camera->x -= SPEED;
		enemy->x += SPEED;
		player->x += SPEED;
		for (int i = 0; i < SHIPS_NUMBER; i++)
			for (int j = 0; j < TabRes; j++)
				tab[i][2 * j] += SPEED;
	}
}

void move(Position_t* player, Position_t* enemy, SDL_Surface* sprite, SDL_Rect* camera, double tab[SHIPS_NUMBER][SIZETAB])
{
	const Uint8* click = SDL_GetKeyboardState(NULL);
	if (click[SDL_SCANCODE_RIGHT])
	{
		MaxX(player, enemy, sprite, camera, tab);
	}
	if (click[SDL_SCANCODE_LEFT])
	{
		MinX(player, enemy, sprite, camera, tab);
	}
	if (click[SDL_SCANCODE_UP])
	{
		if (player->y > SCREEN_HEIGHT / 5) player->y    -= SPEED;
	}
	if (click[SDL_SCANCODE_DOWN])
	{
		if (player->y < SCREEN_HEIGHT - sprite->w / 2) player->y += SPEED;
	}
}