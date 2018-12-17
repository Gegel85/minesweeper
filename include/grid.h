//
// Created by Gegel85 on 13/12/2018.
//

#ifndef MINESWEEPER_GRID_H
#define MINESWEEPER_GRID_H

#include <SFML/Graphics.h>

enum	state {	//Enumeration for the grid state
	NO_MINE = 0,
	MINE	= 1 << 0,
	OPENED	= 1 << 1,
};

enum	content {	//The content of a grid box
	NOTHING,
	ONE_MINE_CLOSE,
	TWO_MINES_CLOSE,
	THREE_MINES_CLOSE,
	FOUR_MINES_CLOSE,
	FIVE_MINES_CLOSE,
	SIX_MINES_CLOSE,
	SEVEN_MINES_CLOSE,
	EIGHT_MINES_CLOSE,
	MINE_REMAINING,
	MINE_BLOWN,
	WRONG_FLAG,
	FLAG,
	UNKNOWN,
	DIFFUSED_BOMB,
};

enum	directions {
	NONE,
	TOP_LEFT,
	TOP,
	TOP_RIGHT,
	RIGHT,
	BOTTOM_RIGHT,
	BOTTOM,
	BOTTOM_LEFT,
	LEFT
};

struct	Grid {
	bool		jumpingMines;	//Whether it is a regular game or a jumping mines game
	sfVector2u	size;		//The size of the grid
	sfVector2f	boxSize;	//The size of a box
	unsigned	total;		//The total number of mines
	unsigned	flagsPlaced;	//The number of flag the player has placed
	unsigned	openedBoxes;	//The number of boxes opened in the grid
	char		**grid;		//The grid itself
	bool		isGenerated;	//Has the game grid been generated or not
};

enum	directions	getCorrespondingDirection(int x, int y, int dir);
void			changeBoxContent(int x, int y);
int			numberOfFlagsAround(int x, int y);
void			openAdjacentBoxs(int x, int y);

#endif //MINESWEEPER_GRID_H
