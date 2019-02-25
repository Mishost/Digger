#include <SDL_image.h>
#include "RenderManager.h"

bool RenderManager::InitEverything()
{
	if (!InitSDL())
		return false;

	if (!CreateWindow())
		return false;

	if (!CreateRenderer())
		return false;

	if (!SetupTTF("STIXGeneral.ttf"))
		return false;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "Error with the music.\n";
		return false;
	}

	SetupRenderer();

	return true;
}
bool RenderManager::InitSDL()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1)
	{
		std::cout << " Failed to initialize SDL : " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}
bool RenderManager::CreateWindow()
{
	window = SDL_CreateWindow("Digger", posX, posY, sizeX, sizeY, 0);

	if (window == nullptr)
	{
		std::cout << "Failed to create window : " << SDL_GetError();
		return false;
	}

	return true;
}
bool RenderManager::CreateRenderer()
{
	renderer = SDL_CreateRenderer(window, -1, 0);

	if (renderer == nullptr)
	{
		std::cout << "Failed to create renderer : " << SDL_GetError();
		return false;
	}

	return true;
}
void RenderManager::SetupRenderer()
{
	// Set size of renderer to the same as window
	SDL_RenderSetLogicalSize(renderer, sizeX, sizeY);

	// Set color of renderer to black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void RenderManager::Render(const SDL_Rect& player, std::vector<SDL_Rect>& blackSpaces,
	std::vector<SDL_Rect>& enemies, SDL_Rect& moneyBag, std::vector<bool>& enemiesLevel,
	bool isFirstRender, bool shouldFlip, bool shouldRenderMoneyBag)
{
	if (isFirstRender)
	{
		playerTexture = LoadTexture(playerImage);
		enemyTexture = LoadTexture(enemyImage);
		backgroundTexture = LoadTexture(backgroundImage);
		moneyBagTexture = LoadTexture(moneyBagImage);
		nextEnemyTexture = LoadTexture(nextEnemyImage);
		backgroundMusic = Mix_LoadMUS("background.wav");
		Mix_PlayMusic(backgroundMusic, -1);
	}

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, backgroundTexture, NULL, &backgroundPos);

	for (SDL_Rect& blackSpace : blackSpaces)
		SDL_RenderFillRect(renderer, &blackSpace);

	if (!shouldFlip)
		SDL_RenderCopy(renderer, playerTexture, NULL, &player);
	else
	{
		SDL_RendererFlip flipPlayer = SDL_FLIP_HORIZONTAL;
		SDL_RenderCopyEx(renderer, playerTexture, NULL, &player, 0, NULL, flipPlayer);
	}

	unsigned int cnt = 0;
	for (SDL_Rect& enemy : enemies)
	{
		if (enemiesLevel[cnt] == 0)
			SDL_RenderCopy(renderer, enemyTexture, NULL, &enemy);
		else
			SDL_RenderCopy(renderer, nextEnemyTexture, NULL, &enemy);
		++cnt;
	}
	if (shouldRenderMoneyBag)
		SDL_RenderCopy(renderer, moneyBagTexture, NULL, &moneyBag);

	SDL_RenderPresent(renderer);
}
bool RenderManager::SetupTTF(const std::string& fontName)
{
	if (TTF_Init() == -1)
	{
		std::cout << " Failed to initialize TTF : " << SDL_GetError() << std::endl;
		return false;
	}

	// Load our fonts, with a huge size
	font = TTF_OpenFont(fontName.c_str(), 90);

	// Error check
	if (font == nullptr)
	{
		std::cout << " Failed to load font : " << SDL_GetError() << std::endl;
		return false;
	}

	return true;
}

SDL_Texture* RenderManager::SurfaceToTexture(SDL_Surface* surf)
{
	SDL_Texture* text;

	text = SDL_CreateTextureFromSurface(renderer, surf);

	SDL_FreeSurface(surf);

	return text;
}

void RenderManager::renderLivesLeft(int lives)
{
	SDL_RenderClear(renderer);

	std::string text = "You have ";
	char temp = lives + '0';
	text.push_back(temp);
	if (lives == 1)
		text.append(" life ");
	else
		text.append(" lives ");
	text.append("left.");
	SDL_Surface* blendedText = TTF_RenderText_Blended(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SurfaceToTexture(blendedText);

	SDL_Rect textRect = { sizeX / 2, sizeY / 2, 50, 50 };
	SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
	textRect.x = 0;
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_RenderPresent(renderer);

	Mix_HaltMusic();
	Mix_PlayMusic(backgroundMusic, -1);
}

void RenderManager::gameOver(bool won)
{
	SDL_RenderClear(renderer);
	std::string text;
	if (!won)
		text = "GAME OVER";
	else
		text = "YOU WON!";
	SDL_Surface* blendedText = TTF_RenderText_Blended(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SurfaceToTexture(blendedText);

	SDL_Rect textRect = { sizeX / 4, sizeY / 4, 50, 50 };
	SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
	textRect.x -= 50;
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_RenderPresent(renderer);
}

unsigned int RenderManager::getWindowHeight()
{
	return sizeY;
}
unsigned int RenderManager::getWindowWidth()
{
	return sizeX;
}

SDL_Texture* RenderManager::LoadTexture(const std::string& str)
{
	// Load image as SDL_Surface
	SDL_Surface* surface = IMG_Load(str.c_str());

	// SDL_Surface is just the raw pixels
	// Convert it to a hardware-optimzed texture so we can render it
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

	// Don't need the orignal texture, release the memory
	SDL_FreeSurface(surface);

	return texture;
}
