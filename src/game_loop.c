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

enum	directions	getCorrespondingDirection(int x, int y, int dir)
{
	enum	directions	direction = 0;
	sfVector2u		size = game.grid.size;

	direction += x > 0 &&		y > 0 &&		!(game.grid.grid[x - 1][y - 1] & MINE) &&	!(game.grid.grid[x - 1][y - 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += 			y > 0 &&		!(game.grid.grid[x][y - 1] & MINE) &&		!(game.grid.grid[x    ][y - 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += x < size.x - 1 &&	y > 0 &&		!(game.grid.grid[x + 1][y - 1] & MINE) &&	!(game.grid.grid[x + 1][y - 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += x < size.x - 1 &&				!(game.grid.grid[x + 1][y] & MINE) &&		!(game.grid.grid[x + 1][y    ] >> 1) ? dir-- > 0 : dir > 0;
	direction += x < size.x - 1 &&	y < size.y - 1 &&	!(game.grid.grid[x + 1][y + 1] & MINE) &&	!(game.grid.grid[x + 1][y + 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += 			y < size.y - 1 &&	!(game.grid.grid[x][y + 1] & MINE) &&		!(game.grid.grid[x    ][y + 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += x > 0 &&		y < size.y - 1 &&	!(game.grid.grid[x - 1][y + 1] & MINE) &&	!(game.grid.grid[x - 1][y + 1] >> 1) ? dir-- > 0 : dir > 0;
	direction += x > 0 &&					!(game.grid.grid[x - 1][y] & MINE) &&		!(game.grid.grid[x - 1][y    ] >> 1) && dir > 0;
	return direction;
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