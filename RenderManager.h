#ifndef __RENDERING__HEADER__INCLUDED
#define __RENDERING__HEADER__INCLUDED
#include <iostream>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "Block.h"

class RenderManager
{
public:
	RenderManager() { InitEverything(); };
	~RenderManager() = default;

	void Render(const SDL_Rect& player, std::vector<SDL_Rect>& blackSpaces,
		std::vector<SDL_Rect>& enemies, SDL_Rect& moneyBag, std::vector<bool>& enemiesLevel,
		bool isFirstRender, bool shouldFlip, bool shouldRenderMoneyBag);

	void renderLivesLeft(int lives);
	void gameOver(bool won);

	unsigned int getWindowHeight();
	unsigned int getWindowWidth();
private:
	bool InitEverything();
	bool InitSDL();
	bool CreateWindow();
	bool CreateRenderer();
	void SetupRenderer();
	bool SetupTTF(const std::string& fontName);

	SDL_Texture* LoadTexture(const std::string& str);
	SDL_Texture* SurfaceToTexture(SDL_Surface* surf);

private:
	SDL_Rect backgroundPos{ 0, 0, 800, 500 };
	SDL_Texture* playerTexture;
	SDL_Texture* backgroundTexture;
	SDL_Texture* enemyTexture;
	SDL_Texture* moneyBagTexture;
	SDL_Texture* nextEnemyTexture;

	SDL_Window* window;
	SDL_Renderer* renderer;

	TTF_Font* font;
	SDL_Color textColor = { 255, 255, 255, 255 }; //white
	Mix_Music* backgroundMusic;

	const std::string playerImage = "knight.png";
	const std::string enemyImage = "enemy.png";
	const std::string backgroundImage = "grass.png";
	const std::string moneyBagImage = "moneyBag.png";
	const std::string nextEnemyImage = "nextEnemy.png";

	int posX = 300; //where the window will be
	int posY = 150;
	int sizeX = 800; //width of window
	int sizeY = 500; //height of window

};

#endif // !__RENDERING__HEADER__INCLUDED
