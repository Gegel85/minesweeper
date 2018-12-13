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

struct	Grid {
	bool		isGenerated;	//Has the game grid been generated or not
	sfVector2u	size;		//The size of the grid
	int		total;		//The total number of mines
	int		flagsPlaced;	//The number of flag the player has placed
	int		openedBoxes;	//The number of boxes openned in the grid
	char		**grid;		//The grid itself
};

#endif //MINESWEEPER_GRID_H
