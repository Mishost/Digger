#ifndef __BLOCK__HEADER__INCLUDED
#define __BLOCK__HEADER__INCLUDED

#include <SDL.h>

const unsigned int width = 50;
const unsigned int height = 50;
struct Block
{
	bool isDug;
	bool isEnemy;
	bool isPlayer;
	bool isMoneyBag;
	SDL_Rect position;
	Block(unsigned int row, unsigned int col, bool is_dug, bool is_enemy, bool is_player);
};

#endif // !__BLOCK__HEADER__INCLUDED
