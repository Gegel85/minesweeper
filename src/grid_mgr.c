//
// Created by Gegel85 on 15/12/2018.
//

#include <SFML/Graphics.h>
#include <concatf.h>
#include <discord_rp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "macros.h"
#include "globals.h"

void	endGame()
{
	char	*array = *game.grid.grid;
	int	found = 0;
	int	errors = 0;
	char	*buffer;

	for (unsigned i = 0; i < game.grid.size.x * game.grid.size.y; i++) {
		if (!(array[i] & OPENED) && array[i] >> 2 != FLAG && array[i] & MINE) {
			array[i] += (MINE_REMAINING << 2) - ((array[i] >> 2) << 2);
			array[i] += OPENED;
		} else if (array[i] >> 2 == FLAG && !(array[i] & MINE)) {
			array[i] -= 1 << 2;
			array[i] += OPENED;
			errors++;
		} else if (array[i] >> 2 == FLAG && array[i] & MINE)
			found++;
	}
	buffer = concatf("%i missplaced flags, found %i mines / %i, %i seconds total", errors, found, game.grid.total, time(NULL) - game.start);
	updateDiscordPresence("Exploded", buffer, 0, false, "icon", NULL);
	free(buffer);
	game.grid.isGenerated = false;
}

void	winGame()
{
	char	*buffer;

	for (unsigned x = 0; x < game.grid.size.x; x++)
		for (unsigned y = 0; y < game.grid.size.y; y++)
			if (!(game.grid.grid[x][y] & OPENED)) {
				if (game.grid.grid[x][y] >> 2 != FLAG) {
					game.grid.grid[x][y] -= (game.grid.grid[x][y] << 2) >> 2;
					game.grid.grid[x][y] += (DIFFUSED_BOMB << 2) + OPENED;
				} else {
					game.grid.grid[x][y] += OPENED;
				}
			}
	buffer = concatf("Found all %i mines in %i seconds", game.grid.total, time(NULL) - game.start);
	updateDiscordPresence("Exploded", buffer, 0, false, "icon", NULL);
	free(buffer);
	game.grid.isGenerated = false;
}

void	generateGrid(int x, int y)
{
	char	*array = *game.grid.grid;
	int	remainingSpace = game.grid.size.x * game.grid.size.y;
	int	random;
	int	index;
	char	*buffer = concatf("Mines found: 0 of %i", game.grid.total);

	game.start = time(NULL);
	updateDiscordPresence("Unmining battlefield", buffer, game.start, false, "icon", NULL);
	game.grid.flagsPlaced = 0;
	game.grid.openedBoxes = 0;
	free(buffer);
	memset(*game.grid.grid, NO_MINE, game.grid.size.x * game.grid.size.y);
	game.grid.grid[x][y] = OPENED;
	for (int i = 0; i < game.grid.total; i++) {
		random = rand() % remainingSpace;
		for (index = 0; array[index] != NO_MINE; index++);			//Skip the first indexs that contains a mine
		for (; random > 0 && (random -= array[index] == NO_MINE); index++);	//When random gets to 0 this loop breaks
		array[index] = MINE;
		remainingSpace--;
	}
	game.grid.isGenerated = true;
}

int	getNbOfMinesNear(int x, int y)
{
	int		nb = 0;
	sfVector2i	size = *(sfVector2i *)&game.grid.size;

	nb += x > 0 &&					game.grid.grid[x - 1][y] & MINE;
	nb += x > 0 &&		y < size.y - 1 &&	game.grid.grid[x - 1][y + 1] & MINE;
	nb += x > 0 &&		y > 0 &&		game.grid.grid[x - 1][y - 1] & MINE;
	nb += x < size.x - 1 &&				game.grid.grid[x + 1][y] & MINE;
	nb += x < size.x - 1 &&	y < size.y - 1 &&	game.grid.grid[x + 1][y + 1] & MINE;
	nb += x < size.x - 1 &&	y > 0 &&		game.grid.grid[x + 1][y - 1] & MINE;
	nb += 			y < size.y - 1 &&	game.grid.grid[x][y + 1] & MINE;
	nb += 			y > 0 &&		game.grid.grid[x][y - 1] & MINE;
	return nb;
}

int	getNbOfFreeBoxsAround(int x, int y)
{
	int		nb = 0;
	sfVector2i	size = *(sfVector2i *)&game.grid.size;

	nb += x > 0 &&					!(game.grid.grid[x - 1][y] & MINE) &&		!(game.grid.grid[x - 1][y] >> 1);
	nb += x > 0 &&		y < size.y - 1 &&	!(game.grid.grid[x - 1][y + 1] & MINE) &&	!(game.grid.grid[x - 1][y + 1] >> 1);
	nb += x > 0 &&		y > 0 &&		!(game.grid.grid[x - 1][y - 1] & MINE) &&	!(game.grid.grid[x - 1][y - 1] >> 1);
	nb += x < size.x - 1 &&				!(game.grid.grid[x + 1][y] & MINE) &&		!(game.grid.grid[x + 1][y] >> 1);
	nb += x < size.x - 1 &&	y < size.y - 1 &&	!(game.grid.grid[x + 1][y + 1] & MINE) &&	!(game.grid.grid[x + 1][y + 1] >> 1);
	nb += x < size.x - 1 &&	y > 0 &&		!(game.grid.grid[x + 1][y - 1] & MINE) &&	!(game.grid.grid[x + 1][y - 1] >> 1);
	nb += 			y < size.y - 1 &&	!(game.grid.grid[x][y + 1] & MINE) &&		!(game.grid.grid[x][y + 1] >> 1);
	nb += 			y > 0 &&		!(game.grid.grid[x][y - 1] & MINE) &&		!(game.grid.grid[x][y - 1] >> 1);
	return nb;
}

void	openAdjacentBoxs(int x, int y)
{
	sfVector2i	size = *(sfVector2i *)&game.grid.size;
	char		elem = game.grid.grid[x][y];

	if (
		x > 0 &&
		!(game.grid.grid[x - 1][y] & OPENED) &&
		(game.grid.grid[x - 1][y] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x - 1, y);

	if (
		x > 0 && y < size.y - 1 &&
		!(game.grid.grid[x - 1][y + 1] & OPENED) &&
		(game.grid.grid[x - 1][y + 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x - 1, y + 1);

	if (
		x > 0 && y > 0 &&
		!(game.grid.grid[x - 1][y - 1] & OPENED) &&
		(game.grid.grid[x - 1][y - 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x - 1, y - 1);

	if (
		x < size.x - 1 &&
		!(game.grid.grid[x + 1][y] & OPENED) &&
		(game.grid.grid[x + 1][y] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x + 1, y);

	if (
		x < size.x - 1 && y < size.y - 1 &&
		!(game.grid.grid[x + 1][y + 1] & OPENED) &&
		(game.grid.grid[x + 1][y + 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x + 1, y + 1);

	if (
		x < size.x - 1 && y > 0 &&
		!(game.grid.grid[x + 1][y - 1] & OPENED) &&
		(game.grid.grid[x + 1][y - 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x + 1, y - 1);

	if (
		y < size.y - 1 &&
		!(game.grid.grid[x][y + 1] & OPENED) &&
		(game.grid.grid[x][y + 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x, y + 1);

	if (
		y > 0 &&
		!(game.grid.grid[x][y - 1] & OPENED) &&
		(game.grid.grid[x][y - 1] >> 2) != FLAG &&
		elem == game.grid.grid[x][y] &&
		game.grid.isGenerated
	)
		openGridBox(x, y - 1);
}

void	moveMine(int x, int y, enum directions dir)
{
	game.grid.grid[x][y] -= game.grid.grid[x][y] & MINE;
	switch (dir) {
		case NONE:
			game.grid.grid[x][y] += MINE;
			break;
		case TOP_LEFT:
			game.grid.grid[x - 1][y - 1] += MINE;
			break;
		case TOP:
			game.grid.grid[x][y - 1] += MINE;
			break;
		case TOP_RIGHT:
			game.grid.grid[x + 1][y - 1] += MINE;
			break;
		case RIGHT:
			game.grid.grid[x + 1][y] += MINE;
			break;
		case BOTTOM_RIGHT:
			game.grid.grid[x + 1][y + 1] += MINE;
			break;
		case BOTTOM:
			game.grid.grid[x][y + 1] += MINE;
			break;
		case BOTTOM_LEFT:
			game.grid.grid[x - 1][y + 1] += MINE;
			break;
		case LEFT:
			game.grid.grid[x - 1][y] += MINE;
			break;
	}
}

void	updateBoxesIndicator()
{
	for (unsigned x = 0; x < game.grid.size.x; x++) {
		for (unsigned y = 0; y < game.grid.size.x; y++) {
			if (game.grid.grid[x][y] & OPENED && !(game.grid.grid[x][y] & MINE)) {
				game.grid.grid[x][y] -= (game.grid.grid[x][y] >> 2) << 2;
				game.grid.grid[x][y] += getNbOfMinesNear(x, y) << 2;
			}
		}
	}
}

void	changeBoxContent(int x, int y)
{
	if (game.grid.grid[x][y] >> 2 == NOTHING) {
		char *buffer = concatf("Mines found: %i of %i", ++game.grid.flagsPlaced, game.grid.total);

		updateDiscordPresence("Unmining battlefield", buffer, game.start, false, "icon", NULL);
		free(buffer);
		game.grid.grid[x][y] += FLAG << 2;
		if (game.grid.flagsPlaced == game.grid.total) {
			for (unsigned x = 0; x < game.grid.size.x; x++)
				for (unsigned y = 0; y < game.grid.size.y; y++)
					if (!(game.grid.grid[x][y] & OPENED) && game.grid.grid[x][y] >> 2 != FLAG) {
						game.grid.isGenerated = true;
						openGridBox(x, y);
					}
		}
	} else if (game.grid.grid[x][y] >> 2 == FLAG) {
		char *buffer = concatf("Mines found: %i of %i", --game.grid.flagsPlaced, game.grid.total);

		updateDiscordPresence("Unmining battlefield", buffer, game.start, false, "icon", NULL);
		free(buffer);
		game.grid.grid[x][y] += 1 << 2;
	} else {
		game.grid.grid[x][y] -= (game.grid.grid[x][y] >> 2) << 2;
	}
}

int	numberOfFlagsAround(int x, int y)
{
	int		nb = 0;
	sfVector2i	size = *(sfVector2i *)&game.grid.size;

	nb += x > 0 &&		y > 0 &&		(game.grid.grid[x - 1][y - 1] >> 2) == FLAG;
	nb += 			y > 0 &&		(game.grid.grid[x][y - 1] >> 2) == FLAG;
	nb += x < size.x - 1 &&	y > 0 &&		(game.grid.grid[x + 1][y - 1] >> 2) == FLAG;
	nb += x < size.x - 1 &&				(game.grid.grid[x + 1][y] >> 2) == FLAG;
	nb += x < size.x - 1 &&	y < size.y - 1 &&	(game.grid.grid[x + 1][y + 1] >> 2) == FLAG;
	nb += 			y < size.y - 1 &&	(game.grid.grid[x][y + 1] >> 2) == FLAG;
	nb += x > 0 &&		y < size.y - 1 &&	(game.grid.grid[x - 1][y + 1] >> 2) == FLAG;
	nb += x > 0 &&					(game.grid.grid[x - 1][y] >> 2) == FLAG;
	return nb;
}

void	moveMines()
{
	int	random;

	for (unsigned x = 0; x < game.grid.size.x; x++) {
		for (unsigned y = 0; y < game.grid.size.x; y++) {
			if (game.grid.grid[x][y] & MINE && !(game.grid.grid[x][y] >> 2)) {
				random = rand() % (getNbOfFreeBoxsAround(x, y) + 1);
				moveMine(x, y, getCorrespondingDirection(x, y, random));
			}
		}
	}
}

void	openGridBox(int x, int y)
{
	bool	jumpingMines = game.grid.jumpingMines;

	printf("%s: Opening box at pos %i, %i\n", INFO_BEG, x, y);
	if (!game.grid.isGenerated)
		generateGrid(x, y);
	else {
		game.grid.grid[x][y] -= (game.grid.grid[x][y] >> 2) << 2;
		game.grid.grid[x][y] += OPENED;
	}
	game.grid.openedBoxes++;
	if (game.grid.grid[x][y] & MINE) {
		endGame();
		game.grid.grid[x][y] += MINE_BLOWN << 2;
	} else  {
		game.grid.grid[x][y] += getNbOfMinesNear(x, y) << 2;
		if (game.grid.openedBoxes == game.grid.size.x * game.grid.size.y - game.grid.total)
			winGame();
	}
	if (!(game.grid.grid[x][y] & MINE) && (game.grid.grid[x][y] >> 2) == 0) {
		game.grid.jumpingMines = false;
		openAdjacentBoxs(x, y);
		game.grid.jumpingMines = jumpingMines;
	}
	if (jumpingMines && game.grid.isGenerated) {
		moveMines();
		updateBoxesIndicator();
	}
}