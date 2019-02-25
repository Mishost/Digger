#include <string>
#include "Block.h"
const unsigned int width = 50;
const unsigned int height = 50;

Block::Block(unsigned int row, unsigned int col,
	bool is_dug = false, bool is_enemy = false, bool is_player = false)
	:isDug(is_dug)
	,isEnemy(is_enemy)
	,isPlayer(is_player)
{
	isMoneyBag = false;
	position.w = width;
	position.h = height;
	position.x = col * height;
	position.y = row * width;
}

bool Block::is_dug() const
{
	return isDug;
}

bool Block::is_enemy() const
{
	return isEnemy;
}

bool Block::is_player() const
{
	return isPlayer;
}

void Block::becomesDug()
{
	isDug = true;
}

void Block::becomesEnemy(bool newEnemy)
{
	isEnemy = newEnemy;
}

void Block::becomesPlayer(bool newPlayer)
{
	isPlayer = newPlayer;
}

void Block::becomesMoneyBag(bool newBag)
{
	isMoneyBag = newBag;
}

SDL_Rect Block::getPosition() const
{
	return position;
}

const unsigned int Block::getWidth() const
{
	return width;
}

const unsigned int Block::getHeight() const
{
	return height;
}
