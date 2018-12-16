//
// Created by Gegel85 on 12/12/2018.
//

#ifndef MINESWEEPER_GAME_H
#define MINESWEEPER_GAME_H

#include <SFML/Graphics.h>
#include <time.h>
#include <stdbool.h>
#include "array.h"
#include "sprite.h"
#include "grid.h"
#include "typedef.h"

void	launchGame();
void	openGridBox(int x, int y);

struct	Resources {
	sfRenderWindow	*window;//Main window
	sfRectangleShape*rect;	//Rectangle object
	sfText		*text;	//Text object
	sfFont		*font;	//Font loaded
	Array		sprite;	//All sprites loaded
	sfImage		*icon;	//The window icon
};

struct	game_s {
	bool		debug;		//If in debug mode or not
	Grid		grid;		//The grid
	Resources	resources;	//Loaded
	Array		buttons;	//All buttons loaded
	Array		slider;		//All slider loaded
	time_t		start;		//The timestamp at which the game started
};

#endif //MINESWEEPER_GAME_H
