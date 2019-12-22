#ifndef SCORE_H
#define SCORE_H

#include "GameObjects.h"
#include "Game.h"

Background background;
char scoreLeftchar = background.CHARACTERS[player.XPos + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 2))];
char scoreRightchar = background.CHARACTERS[(player.XPos + (player.WIDTH - 1)) + SCREEN_WIDTH * (player.YPos + (player.HEIGHT - 2))];
#endif SCORE_H