#ifndef __BLOCK__HEADER__INCLUDED
#define __BLOCK__HEADER__INCLUDED

#include <SDL.h>

class Block
{
public:
	Block(unsigned int row, unsigned int col, bool is_dug, bool is_enemy, bool is_player);
	Block(const Block & other) = default;
	Block & operator=(const Block & other) = default;
	~Block() = default;

	bool is_dug() const;
	bool is_enemy() const;
	bool is_player() const;

	void becomesDug();
	void becomesEnemy(bool newEnemy);
	void becomesPlayer(bool newPlayer);
	void becomesMoneyBag(bool newBag);

	SDL_Rect getPosition() const;
	const unsigned int getWidth() const;
	const unsigned int getHeight() const;

private:
	bool isDug;
	bool isEnemy;
	bool isPlayer;
	bool isMoneyBag;
	SDL_Rect position;
};

#endif // !__BLOCK__HEADER__INCLUDED
