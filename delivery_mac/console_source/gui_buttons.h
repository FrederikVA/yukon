#ifndef GUI_BUTTONS_H
#define GUI_BUTTONS_H

#include <SDL.h>
#include <SDL_ttf.h>

void drawButtons(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
void handleButtonClick(int x, int y);
void drawExitGameButton(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
void handleExitButtonClick(int x, int y);
void drawPlayControls(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
int handlePlayControlClick(int x, int y);
void drawStartupSavedGames(SDL_Renderer *renderer, TTF_Font *font, int screenWidth, int screenHeight);
int handleSavedGameClick(int x, int y);

#endif
