#include "space_invaders.h"

#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

namespace SpaceInvader
{
	char rawInvader[] = {
			' ',' ','#',' ',' ',' ',' ',' ','#',' ',' ',
			' ',' ',' ','#',' ',' ',' ','#',' ',' ',' ',
			' ',' ',' ','#','#','#','#','#','#',' ',' ',
			' ','#','#',' ','#','#','#',' ','#','#',' ',
			'#','#','#','#','#','#','#','#','#','#','#',
			'#',' ','#','#','#','#','#','#','#',' ','#',
			'#',' ','#',' ',' ',' ',' ',' ','#',' ','#',
			' ',' ',' ','#','#',' ','#','#',' ',' ',' ',
	};

	Sprite* sprInvaders[]
	{
		new Sprite(RED, 11, 8, rawInvader),
		new Sprite(BLUE, 11, 8, rawInvader),
		new Sprite(GREEN, 11, 8, rawInvader),
	};
	Sprite sprPlayer(
		WHITE, 11, 8, new char[11 * 8]{
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ','#','#','#',' ',' ',' ',' ',
			' ',' ',' ',' ','#','#','#',' ',' ',' ',' ',
			' ',' ',' ','#','#','#','#','#',' ',' ',' ',
			' ','#','#','#','#','#','#','#','#','#',' ',
			' ','#','#',' ',' ',' ',' ',' ','#','#',' ',
			' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
		}
	);
	Sprite sprProjectile(
		RED, 11, 8, new char[11 * 8]{
			' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','*',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ','#',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
			' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
		}
	);

	Game::Game(short invaderColums, short invaderRows)
	{
		this->invaderColums = invaderColums;
		this->invaderRows = invaderRows;
		this->invaderAmount = invaderColums * invaderRows;

		player = new Player(invaderColums / 2, invaderRows + 4, &sprPlayer);
		projectile = new Projectile(-1, -1, &sprProjectile);
		invaders = new Invader * [invaderAmount];

		for (short y = 0; y < invaderRows; y++)
		{
			for (short x = 0; x < invaderColums; x++)
			{
				invaders[y * invaderColums + x] = new Invader(x, y, sprInvaders[y]);
			}
		}
	}
	Invader::Invader(short x, short y, Sprite* sprite)
		: MapObject(x, y, sprite)
	{
		isDead = false;
	}

	void Invader::Update()
	{
		UpdatePreviewsPos();
		if (x > 6)
		{
			//Set the invaders lower to ground and reset location 
			y++;
			x = 0;
		}
		else
		{
			//If not reached max horizontal yet, go more horizontal
			x++;

		}
	}

	Player::Player(short x, short y, Sprite* sprite)
		: MapObject(x, y, sprite)
	{
	}

	MapObject::MapObject(short x, short y, Sprite* sprite)
	{
		this->x = x;
		this->y = y;
		this->px = -1;
		this->py = -1;
		this->sprite = sprite;
	}
	void MapObject::UpdatePreviewsPos()
	{
		px = x;
		py = y;
	}
	Projectile::Projectile(short x, short y, Sprite* sprite)
		: MapObject(x, y, sprite)
	{
	}
	void Projectile::Reset()
	{
		x = -1;
		y = -1;
		isShoted = false;
	}
	Sprite::Sprite(short color, short width, short height, char* pixels)
	{
		this->color = color;
		this->width = width;
		this->height = height;
		this->pixels = pixels;
	}
	bool Game::UpdateProjectile()
	{
		if (projectile->isShoted)
		{
			projectile->UpdatePreviewsPos();
			projectile->y--;

			Invader* invader = GetInvader(projectile->x, projectile->y);

			if (invader != nullptr && !invader->isDead)
			{
				invader->isDead = true;
				projectile->Reset();
				return true;
			}
			else if (projectile->y < -1)
			{
				projectile->Reset();
			}
		}
		return false;
	}
	Invader* Game::GetInvader(short x, short y)
	{
		for (short i = 0; i < invaderAmount; i++)
		{
			Invader* invader = invaders[i];

			if (invader->x == x && invader->y == y)
			{
				return invader;
			}
		}
		return nullptr;
	}
	void Game::Shoot()
	{
		if (!projectile->isShoted)
		{
			projectile->isShoted = true;
			projectile->x = player->x;
			projectile->y = player->y - 1;
		}
	}
	void Game::UpdateInvaders()
	{
		for (short i = 0; i < invaderAmount; i++)
		{
			Invader& invader = *invaders[i];

			invader.UpdatePreviewsPos();
			if (invader.x > invaderMaxTravel)
			{
				//Set the invaders lower to ground and reset location 
				invader.y++;
				invader.x = 0;
			}
			else
			{
				//If not reached max horizontal yet, go more horizontal
				invader.x++;

			}
		}
	}
	bool Game::CheckWin()
	{
		short deadInvaders = 0;
		for (short i = 0; i < invaderAmount; i++)
		{
			if (invaders[i]->isDead)
				deadInvaders++;
		}
		if (deadInvaders >= invaderAmount)
			return true;

		//Else
		return false;
	}
	bool Game::CheckLose()
	{
		for (short i = 0; i < invaderAmount; i++)
		{
			if (!invaders[i]->isDead && invaders[i]->y == player->y)
			{
				return true;
			}
		}

		return false;
	}
	void Game::Reset()
	{
		player->x = invaderColums / 2;
		projectile->Reset();

		for (short y = 0; y < invaderRows; y++)
		{
			for (short x = 0; x < invaderColums; x++)
			{
				invaders[y * invaderColums + x]->isDead = false;
				invaders[y * invaderColums + x]->x = x;
				invaders[y * invaderColums + x]->y = y;
			}
		}
	}
}
