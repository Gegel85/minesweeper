//
// Created by Gegel85 on 13/12/2018.
//

#include <time.h>
#include <concatf.h>
#include <macros.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "globals.h"
#include "discord_rp.h"

void	endGame()
{
	char	*array = *game.grid.grid;
	int	found = 0;
	int	errors = 0;
	char	*buffer;

	for (int i = 0; i < game.grid.size.x * game.grid.size.y; i++) {
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

	for (int x = 0; x < game.grid.size.x; x++)
		for (int y = 0; y < game.grid.size.y; y++)
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
	sfVector2u	size = game.grid.size;

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

void	openAdjacentBoxs(int x, int y)
{
	sfVector2u	size = game.grid.size;

	if (
		x > 0 &&
		!(game.grid.grid[x - 1][y] & OPENED) &&
		(game.grid.grid[x - 1][y] >> 2) != FLAG
	)
		openGridBox(x - 1, y);

	if (
		x > 0 && y < size.y - 1 &&
		!(game.grid.grid[x - 1][y + 1] & OPENED) &&
		(game.grid.grid[x - 1][y + 1] >> 2) != FLAG
	)
		openGridBox(x - 1, y + 1);

	if (
		x > 0 && y > 0 &&
		!(game.grid.grid[x - 1][y - 1] & OPENED) &&
		(game.grid.grid[x - 1][y - 1] >> 2) != FLAG
	)
		openGridBox(x - 1, y - 1);

	if (
		x < size.x - 1 &&
		!(game.grid.grid[x + 1][y] & OPENED) &&
		(game.grid.grid[x + 1][y] >> 2) != FLAG
	)
		openGridBox(x + 1, y);

	if (
		x < size.x - 1 && y < size.y - 1 &&
		!(game.grid.grid[x + 1][y + 1] & OPENED) &&
		(game.grid.grid[x + 1][y + 1] >> 2) != FLAG
	)
		openGridBox(x + 1, y + 1);

	if (
		x < size.x - 1 && y > 0 &&
		!(game.grid.grid[x + 1][y - 1] & OPENED) &&
		(game.grid.grid[x + 1][y - 1] >> 2) != FLAG
	)
		openGridBox(x + 1, y - 1);

	if (
		y < size.y - 1 &&
		!(game.grid.grid[x][y + 1] & OPENED) &&
		(game.grid.grid[x][y + 1] >> 2) != FLAG
	)
		openGridBox(x, y + 1);

	if (
		y > 0 &&
		!(game.grid.grid[x][y - 1] & OPENED) &&
		(game.grid.grid[x][y - 1] >> 2) != FLAG
	)
		openGridBox(x, y - 1);
}

void	openGridBox(int x, int y)
{
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
		openAdjacentBoxs(x, y);
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
			for (int x = 0; x < game.grid.size.x; x++)
				for (int y = 0; y < game.grid.size.y; y++)
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
	sfVector2u	size = game.grid.size;

	nb += x > 0 &&					(game.grid.grid[x - 1][y] >> 2) == FLAG;
	nb += x > 0 &&		y < size.y - 1 &&	(game.grid.grid[x - 1][y + 1] >> 2) == FLAG;
	nb += x > 0 &&		y > 0 &&		(game.grid.grid[x - 1][y - 1] >> 2) == FLAG;
	nb += x < size.x - 1 &&				(game.grid.grid[x + 1][y] >> 2) == FLAG;
	nb += x < size.x - 1 &&	y < size.y - 1 &&	(game.grid.grid[x + 1][y + 1] >> 2) == FLAG;
	nb += x < size.x - 1 &&	y > 0 &&		(game.grid.grid[x + 1][y - 1] >> 2) == FLAG;
	nb += 			y < size.y - 1 &&	(game.grid.grid[x][y + 1] >> 2) == FLAG;
	nb += 			y > 0 &&		(game.grid.grid[x][y - 1] >> 2) == FLAG;
	return nb;
}

void	manageMouseClick(sfMouseButtonEvent *event)
{
	static	sfClock	*clock = NULL;

	if (!clock)
		clock = sfClock_create();
	if (game.grid.isGenerated && game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)(event->y / BOX_SIZE.y)] & OPENED) {
		if (sfTime_asSeconds(sfClock_getElapsedTime(clock)) < 0.5 && numberOfFlagsAround(event->x / BOX_SIZE.x, event->y / BOX_SIZE.y) == game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)(event->y / BOX_SIZE.y)] >> 2)
			openAdjacentBoxs(event->x / BOX_SIZE.x, event->y / BOX_SIZE.y);
		sfClock_restart(clock);
	} else {
		if (
			event->button == sfMouseLeft &&
			(
				((game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)(event->y / BOX_SIZE.y)] >> 2) != FLAG) ||
				(!game.grid.isGenerated)
			)
		) {
			openGridBox((int)(event->x / BOX_SIZE.x), (int)(event->y / BOX_SIZE.y));
		} else if (game.grid.isGenerated && event->button == sfMouseRight && !(game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)(event->y / BOX_SIZE.y)] & OPENED)){
			changeBoxContent((int)(event->x / BOX_SIZE.x), (int)(event->y / BOX_SIZE.y));
		}
	}
}

void	manageEvents()
{
	sfEvent	event;

	while (sfRenderWindow_pollEvent(game.resources.window, &event)) {
		if (event.type == sfEvtClosed) {			//The user clicked the red cross
			sfRenderWindow_close(game.resources.window);	//Close the window
		} else if (event.type == sfEvtMouseButtonPressed) {
			manageMouseClick(&event.mouseButton);
		}
	}
}

void displayShadedRect(int additionalLayers, sfColor color, sfVector2f pos, sfVector2f size)
{
	for (int i = 0; i <= additionalLayers; i++) {
		sfRectangleShape_setSize(game.resources.rect, size);
		sfRectangleShape_setFillColor(game.resources.rect, color);
		sfRectangleShape_setPosition(game.resources.rect, pos);
		sfRenderWindow_drawRectangleShape(game.resources.window, game.resources.rect, NULL);
		pos.x++;
		pos.y++;
		size.x -= 2 + sfRectangleShape_getOutlineThickness(game.resources.rect);
		size.y -= 2 + sfRectangleShape_getOutlineThickness(game.resources.rect);
		color.b -= 10;
		color.g -= 10;
		color.r -= 10;
		sfRectangleShape_setOutlineThickness(game.resources.rect, 0);
	}
}

void	displayGrid()
{
	for (int x = 0; x < game.grid.size.x; x++) {
		for (int y = 0; y < game.grid.size.y; y++) {
			sfRectangleShape_setOutlineThickness(game.resources.rect, 1);
			sfRectangleShape_setOutlineColor(game.resources.rect, (sfColor){0, 0, 0, 255});
			if (game.grid.grid[x][y] & OPENED) {
				displayShadedRect(0, BG_COLORS[game.grid.grid[x][y] >> 2], (sfVector2f){x * BOX_SIZE.x, y * BOX_SIZE.y}, BOX_SIZE);
			} else
				displayShadedRect(8, (sfColor){155, 155, 155, 255}, (sfVector2f){x * BOX_SIZE.x, y * BOX_SIZE.y}, BOX_SIZE);
			if (game.grid.grid[x][y] >> 2) {
				char *buffer = concatf("%c", CHARACTERS[(game.grid.grid[x][y] >> 2) - 1]);
				sfText_setString(game.resources.text, buffer);
				sfText_setColor(game.resources.text, COLORS[(game.grid.grid[x][y] >> 2) - 1]);
				sfText_setCharacterSize(game.resources.text, 15);
				sfText_setPosition(game.resources.text,
						   (sfVector2f){x * BOX_SIZE.x + 8, y * BOX_SIZE.y + 1});
				sfRenderWindow_drawText(game.resources.window, game.resources.text, NULL);
				free(buffer);
			}
		}
	}
}

void	launchGame()
{
	srand(time(NULL));
	updateDiscordPresence("Preparing", NULL, 0, false, "icon", NULL);
	while (sfRenderWindow_isOpen(game.resources.window)) {	//If the window is closed, leave
		sfRenderWindow_clear(game.resources.window, (sfColor){0, 0, 0, 255});
		displayGrid();
		sfRenderWindow_display(game.resources.window);
		manageEvents();
	}
}