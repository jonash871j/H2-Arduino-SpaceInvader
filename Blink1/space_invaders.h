#pragma once

namespace SpaceInvader
{
	struct Sprite
	{
		short color;
		short width;
		short height;
		char* pixels;

		Sprite(short color, short width, short height, char* pixels);
	};

	struct MapObject
	{
		short x, y;
		short px, py;
		Sprite* sprite = nullptr;

		MapObject(short x, short y, Sprite* sprite);
		void UpdatePreviewsPos();
	};

	struct Player : public MapObject
	{
		Player(short x, short y, Sprite* sprite);
	};
	struct Projectile : public MapObject
	{
		bool isShoted = false;

		Projectile(short x, short y, Sprite* sprite);

		void Reset();
	};

	struct Invader : public MapObject
	{
		bool isDead;
		bool isRight = false;
		

		Invader(short x, short y, Sprite* sprite);
		void Update();
	};

	struct Game
	{
		short mapWidth = 8;
		short mapHeight = 8;

		Player* player;
		Projectile* projectile;

		short invaderColums;
		short invaderRows;
		short invaderAmount;
		short invaderMaxTravel = 10;
		bool invaderDirection = false;
		Invader** invaders;

		Game(short invaderColums, short invaderRows);

		void Reset();
		void Shoot();
		void UpdateInvaders();
		bool UpdateProjectile();
		bool CheckWin();
		bool CheckLose();
		Invader* GetInvader(short x, short y);
	};

}
