#ifndef GUI_DRAG_H
#define GUI_DRAG_H

#include <SDL.h>
#include <SDL_ttf.h>
#include "structures.h"
#include "card_images.h"


extern DragState dragging;

void startDragFromPile(Card *card, int pileIndex, int isFoundation);
void startDragFromColumn(Card *card, int columnIndex);
void stopDrag(int x, int y);
void drawDraggedCards(SDL_Renderer *r, TTF_Font *font, CardTextures *t, int x, int y);
void attemptDropAt(int x, int y);  

#endif
