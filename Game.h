#ifndef __GAME__HEADER__INCLUDED
#define __GAME__HEADER__INCLUDED

#include "RenderManager.h"
#include "Block.h"
enum Direction
{
	invalid = -1,
	left,
	right,
	up,
	down
};

class Game
{
public:
	Game();
	~Game() = default;

	void runGame();

private:
	void init();
	void addEnemy();
	void movePlayer(Direction dir);
	SDL_Rect setAtFreeSpace();
	bool Collide(const SDL_Rect& first, const SDL_Rect& second);
	bool checkCollisions();
	int getHeuristicDist(int row, int col);
	Direction moveEnemy(SDL_Rect& currEnemy, unsigned int index);
	void finishMovingEnemy(SDL_Rect& currEnemy, unsigned int index);
	void moveEnemies();
	bool reachedPlayer(int row, int col);
	bool hasWon();
	bool moveMoneyBag();
	void playerDied();

private:
	std::vector<std::vector<Block>> map;
	SDL_Rect playerPos;
	std::vector<SDL_Rect> blackSpaces;
	std::vector<SDL_Rect> enemies;
	std::vector<bool> enemiesLevel;
	SDL_Rect money;
	std::vector<Direction> isMoving;
	//the vector used for the shortest path algorithm
	int lives;
	unsigned int blockCols; //the number of columns in the map
	unsigned int blockRows; //the number of rows in the map
	int blockWidth; //the width of each block
	int blockHeight; //the height of each block
	RenderManager renderManager; //responsible for the rendering
};
#endif // !__GAME__HEADER__INCLUDED
