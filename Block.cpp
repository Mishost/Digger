#include "Block.h"

Block::Block(unsigned int row, unsigned int col, bool is_dug, bool is_enemy, bool is_player)
	:isDug(is_dug), isEnemy(is_enemy), isPlayer(is_player)
{
	isMoneyBag = false;
	position.w = width;
	position.h = height;
	position.x = col * height;
	position.y = row * width;
}