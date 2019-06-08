#include <time.h>
#include <random>
#include "Game.h"

const int MAX_LIVES = 3;
Game::Game()
	:lives(MAX_LIVES)
{
	init();
}

void Game::runGame()
{
	bool loop = true;
	bool isFirst = true;
	bool shouldFlip = false;
	bool shouldMoveMoneyBag = true;
	unsigned int cnt = 1;
	unsigned int movementCnt = 1;
	while (loop)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				loop = false;
			else if (event.type == SDL_KEYDOWN)
			{
				++cnt;
				switch (event.key.keysym.sym)
				{
				case SDLK_RIGHT:
					movePlayer(right);
					shouldFlip = true;
					break;
				case SDLK_LEFT:
					movePlayer(left);
					shouldFlip = false;
					break;
				case SDLK_UP:
					movePlayer(up);
					break;
				case SDLK_DOWN:
					movePlayer(down);
					break;
				default:
					break;
				}
			}
		}
		++movementCnt;
		if (checkCollisions())
		{
			if (!lives)
			{
				renderManager.gameOver(false);
				SDL_Delay(1500);
				std::exit(0);
			}
			renderManager.renderLivesLeft(lives);
			SDL_Delay(1500);
		}
		if (hasWon())
		{
			renderManager.gameOver(true);
			SDL_Delay(1500);
			std::exit(0);
		}
		if (movementCnt % 25 == 0)
		{
			shouldMoveMoneyBag = moveMoneyBag();
			moveEnemies();
		}
		if (cnt % 10 == 0)
		{
			++cnt;
			addEnemy();
		}
		renderManager.Render(playerPos, blackSpaces, enemies, money, enemiesLevel,
			isFirst, shouldFlip, shouldMoveMoneyBag);
		
		isFirst = false;
		
		SDL_Delay(30); //changing the number of fps
	}
}

void Game::init()
{
	map.push_back(std::vector<Block>());
	map[0].push_back(Block(0, 0, 0, 0, 0));

	blockWidth = map[0][0].position.w;
	blockHeight = map[0][0].position.h;
	blockCols = renderManager.getWindowWidth() / blockWidth;
	blockRows = renderManager.getWindowHeight() / blockHeight;

	for (unsigned int i = 0; i < blockRows; ++i)
	{
		if (i) //because we have already pushed back the first row
			map.push_back(std::vector<Block>());
		for (unsigned int j = 0; j < blockCols; ++j)
		{
			if (i != 0 || j != 0) //avoiding the first case again
			{
				if ((i == blockRows - 1) || (j == blockCols - 2 && i < blockRows - 1)
					|| (j == 0 && i < blockRows - 1))
					//the initial tunnel
				{
					map[i].push_back(Block(i, j, true, false, false)); //black spaces
					blackSpaces.push_back(map[i][j].position);
				}
				else
					map[i].push_back(Block(i, j, false, false, false));
			}
		}
	}
	//the initial place where the player will stand when the game starts
	SDL_Rect newPlayerPos = blackSpaces[0];
	unsigned int row = newPlayerPos.y / blockHeight;
	unsigned int col = newPlayerPos.x / blockWidth;
	playerPos = newPlayerPos;
	//where it is in the graph
	map[row][col].isPlayer = true;
	
	map[3][3].isMoneyBag = true;
	money = { 3 * blockWidth, 3 * blockHeight, blockWidth, blockHeight };

	addEnemy();
}

void Game::addEnemy()
{
	SDL_Rect newEnemy = setAtFreeSpace();
	map[newEnemy.y / blockWidth][newEnemy.x / blockHeight].isEnemy = true;

	enemies.push_back(map[newEnemy.y / blockWidth][newEnemy.x / blockHeight].position);
	enemiesLevel.push_back(false);
	isMoving.push_back(invalid); //for the shortest path algorithm
	//the enemy is not in the process of moving to another block
}

void Game::movePlayer(Direction dir)
{
	unsigned int row = playerPos.y / blockWidth;
	unsigned int col = playerPos.x / blockHeight;
	map[row][col].isPlayer = false;

	if (dir == left && col)
	{
		--col;
		playerPos.x -= blockWidth;
	}
	else if (dir == right && (playerPos.x + blockWidth < renderManager.getWindowWidth()))
	{
		++col;
		playerPos.x += blockWidth;
	}
	else if (dir == up && row)
	{
		--row;
		playerPos.y -= blockHeight;
	}
	else if (dir == down && (playerPos.y + blockHeight < renderManager.getWindowHeight()))
	{
		++row;
		playerPos.y += blockHeight;
	}
	if (!map[row][col].isDug)
	{
		map[row][col].isDug = true;
		blackSpaces.push_back(map[row][col].position);
	}
	map[row][col].isPlayer = true;
}

SDL_Rect Game::setAtFreeSpace()
{
	srand(time(NULL));

	unsigned int randomN = rand() % blackSpaces.size();
	SDL_Rect current = blackSpaces[randomN];
	unsigned int row = current.y / blockHeight;  
	unsigned int col = current.x / blockWidth; 
	while (map[row][col].isEnemy || map[row][col].isPlayer)
	{
		randomN = rand() % blackSpaces.size();
		current = blackSpaces[randomN];
		row = current.y / blockHeight;
		col = current.x / blockWidth;
	}
	return current;
}

bool Game::Collide(const SDL_Rect & first, const SDL_Rect & second) const
{
	unsigned int left1 = first.x;
	unsigned int right1 = first.x + first.w;
	unsigned int top1 = first.y;
	unsigned int bottom1 = first.y + first.h;

	unsigned int left2 = second.x;
	unsigned int right2 = second.x + second.w;
	unsigned int top2 = second.y;
	unsigned int bottom2 = second.y + second.h;

	if ((left1 >= right2) || (left2 >= right1)|| (bottom1 <= top2) || (bottom2 <= top1))
		return false;
	else
		return true;
}

bool Game::checkCollisions()
{
	for (SDL_Rect& curr : enemies)
	{
		if (Collide(curr, playerPos))
		{
			playerDied();
			--lives;
			return true; //if it has already collided with someone, the player will be placed
			//in a black space so he won't collide with anyone
		}
	}
	if (playerPos.x == money.x && playerPos.y == money.y)
	{
		playerDied();
		--lives;
		return true;
	}
	return false;
}

int Game::getHeuristicDist(int row, int col) const
{
	return abs((row - playerPos.y / blockHeight)) +
		abs((col - playerPos.x / blockWidth));
}

Direction Game::moveEnemy(SDL_Rect& currEnemy, unsigned int index)
{
	if (isMoving[index] != invalid)
	{
		finishMovingEnemy(currEnemy, index);
	}
	else
	{
		int row = currEnemy.y / blockHeight;
		int col = currEnemy.x / blockWidth;
		Direction dir = invalid;
		int addition = 0;
		int current = getHeuristicDist(row + 1, col);
		if (row + 1 < blockRows && !map[row + 1][col].isEnemy)
		{
			if (reachedPlayer(row + 1, col))
				return invalid;
			if (map[row + 1][col].isDug)
				addition = 1;
			else
				addition = 2;
			dir = down;
			current += addition;
		}
		if (row - 1 >= 0 && !map[row - 1][col].isEnemy)
		{
			if (reachedPlayer(row - 1, col))
				return invalid;
			if (map[row - 1][col].isDug)
				addition = 1;
			else
				addition = 2;
			if (current >= getHeuristicDist(row - 1, col) + addition)
			{
				dir = up;
				current = getHeuristicDist(row - 1, col) + addition;
			}
		}
		if (col - 1 >= 0 && !map[row][col - 1].isEnemy)
		{
			if (reachedPlayer(row, col - 1))
				return invalid;
			if (map[row][col - 1].isDug)
				addition = 1;
			else
				addition = 2;
			if (current > getHeuristicDist(row, col - 1) + addition)
			{
				dir = left;
				current = getHeuristicDist(row, col - 1) + addition;
			}
		}
		if (col + 1 < blockCols && !map[row][col + 1].isEnemy)
		{
			if (reachedPlayer(row, col + 1))
				return invalid;
			if (map[row][col + 1].isDug)
				addition = 1;
			else
				addition = 2;
			if (current > getHeuristicDist(row, col + 1) + addition)
			{
				dir = right;
				current = getHeuristicDist(row, col + 1) + addition;
			}
		}
		
		if (dir == invalid)
			return dir;
		else if (dir == left)
		{
			map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
			if (!map[currEnemy.y / blockHeight][(currEnemy.x / blockWidth) - 1].isDug)
				return dir;
			else
				currEnemy.x -= blockWidth;
		}
		else if (dir == right)
		{
			map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
			if (!map[currEnemy.y / blockHeight][(currEnemy.x / blockWidth) + 1].isDug)
				return dir;
			else
				currEnemy.x += blockWidth;
		}
		else if (dir == up)
		{
			map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
			if (!map[(currEnemy.y / blockHeight) - 1][currEnemy.x / blockWidth].isDug)
				return dir;
			else
				currEnemy.y -= blockHeight;
		}
		else if (dir == down)
		{
			map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
			if (!map[(currEnemy.y / blockHeight) + 1][currEnemy.x / blockWidth].isDug)
				return dir;
			else
				currEnemy.y += blockHeight;
		}
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = true;
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isDug = true;
		blackSpaces.push_back(map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].position);
	}
	return invalid;
}

void Game::finishMovingEnemy(SDL_Rect & currEnemy, unsigned int index)
{
	if (isMoving[index] == left)
	{
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
		currEnemy.x -= blockWidth;
	}
	else if (isMoving[index] == right)
	{
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
		currEnemy.x += blockWidth;
	}
	else if (isMoving[index] == up)
	{
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
		currEnemy.y -= blockHeight;
	}
	else if (isMoving[index] == down)
	{
		map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = false;
		currEnemy.y += blockHeight;
	}

	map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isEnemy = true;
	map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].isDug = true;
	blackSpaces.push_back(map[currEnemy.y / blockHeight][currEnemy.x / blockWidth].position);
}

void Game::moveEnemies()
{
	unsigned int cnt = 0;
	for (SDL_Rect & current : enemies)
	{
		isMoving[cnt] = moveEnemy(current, cnt);
		++cnt;
	}
}

bool Game::reachedPlayer(int row, int col)
{
	if (map[row][col].isPlayer)
	{
		--lives;
		if (!lives)
		{
			renderManager.gameOver(false);
			SDL_Delay(2000);
			std::exit(0);
		}
		renderManager.renderLivesLeft(lives);
		SDL_Delay(1500);
		playerDied();
		return true;
	}
	return false;
}

bool Game::hasWon()
{
	for (std::vector<Block> & current : map)
		for (Block & currBlock : current)
			if (!currBlock.isDug)
				return false;
	return true;
}

bool Game::moveMoneyBag()
{
	int row = money.y / blockHeight;
	int col = money.x / blockWidth;
	if (row + 1 >= blockRows)
	{
		return false;
	}

	unsigned int cnt = 0;
	for (SDL_Rect & current : enemies)
	{
		if (row == (current.y / blockHeight) &&
			col == (current.x / blockWidth))
		{
			enemiesLevel[cnt] = 1;
			return false;
		}
		++cnt;
	}

	if (map[row + 1][col].isDug)
	{
		map[row][col].isMoneyBag = false;
		map[row + 1][col].isMoneyBag = true;
		money.y += blockHeight;
	}
	return true;
}

void Game::playerDied()
{
	map[playerPos.y / blockWidth][playerPos.x / blockHeight].isPlayer = false;
	playerPos = setAtFreeSpace();
	map[playerPos.y / blockWidth][playerPos.x / blockHeight].isPlayer = true;
}
