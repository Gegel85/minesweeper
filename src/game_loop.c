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
	sfVector2i		size = *(sfVector2i *)&game.grid.size;

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
	if (event->y < HUD_POS)
		return;
	if (game.grid.isGenerated && game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)((event->y - HUD_POS) / BOX_SIZE.y)] & OPENED) {
		if (sfTime_asSeconds(sfClock_getElapsedTime(clock)) < 0.5 && numberOfFlagsAround(event->x / BOX_SIZE.x, (event->y - HUD_POS) / BOX_SIZE.y) == game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)((event->y - HUD_POS) / BOX_SIZE.y)] >> 2)
			openAdjacentBoxs(event->x / BOX_SIZE.x, (event->y - HUD_POS) / BOX_SIZE.y);
		sfClock_restart(clock);
	} else {
		if (
			event->button == sfMouseLeft &&
			(
				((game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)((event->y - HUD_POS) / BOX_SIZE.y)] >> 2) != FLAG) ||
				(!game.grid.isGenerated)
			)
		) {
			openGridBox((int)(event->x / BOX_SIZE.x), (int)((event->y - HUD_POS) / BOX_SIZE.y));
		} else if (game.grid.isGenerated && event->button == sfMouseRight && !(game.grid.grid[(int)(event->x / BOX_SIZE.x)][(int)((event->y - HUD_POS) / BOX_SIZE.y)] & OPENED)){
			changeBoxContent((int)(event->x / BOX_SIZE.x), (int)((event->y - HUD_POS) / BOX_SIZE.y));
		}
	}
}

void	allocGrid(Grid *grid);
void	getGridInfos(int argc, char **args, Grid *grid);

void	manageEvents()
{
	sfEvent	event;
	bool	keyPressed = false;

	while (sfRenderWindow_pollEvent(game.resources.window, &event)) {
		if (event.type == sfEvtClosed) {			//The user clicked the red cross
			sfRenderWindow_close(game.resources.window);	//Close the window
		} else if (event.type == sfEvtMouseButtonPressed) {
			manageMouseClick(&event.mouseButton);
		} else if (event.type == sfEvtKeyPressed) {
			if (keyPressed)
				continue;
			keyPressed = true;
			if (dispMsg("Reload ?", "Do you want to reload settings from file settings.json ?", MB_YESNO) == IDYES) {
				free(*game.grid.grid);
				free(game.grid.grid);
				getGridInfos(0, NULL, &game.grid);
				allocGrid(&game.grid);
			}
		}
	}
}

void displayShadedRect(int additionalLayers, sfColor color, sfVector2f pos, sfVector2u size)
{
	for (int i = 0; i <= additionalLayers; i++) {
		sfRectangleShape_setSize(game.resources.rect, (sfVector2f){size.x, size.y});
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
	for (unsigned x = 0; x < game.grid.size.x; x++) {
		for (unsigned y = 0; y < game.grid.size.y; y++) {
			sfRectangleShape_setOutlineThickness(game.resources.rect, 1);
			sfRectangleShape_setOutlineColor(game.resources.rect, (sfColor){0, 0, 0, 255});
			if (game.grid.grid[x][y] & OPENED) {
				displayShadedRect(0, BG_COLORS[game.grid.grid[x][y] >> 2], (sfVector2f){x * BOX_SIZE.x, y * BOX_SIZE.y + HUD_POS}, BOX_SIZE);
			} else
				displayShadedRect(8, (sfColor){155, 155, 155, 255}, (sfVector2f){x * BOX_SIZE.x, y * BOX_SIZE.y + HUD_POS}, BOX_SIZE);
			if (game.grid.grid[x][y] >> 2 && !loaded_sprites[OBJECTS_SPRITE].sprite) {
				char *buffer = concatf("%c", CHARACTERS[(game.grid.grid[x][y] >> 2) - 1]);
				sfText_setString(game.resources.text, buffer);
				sfText_setColor(game.resources.text, COLORS[(game.grid.grid[x][y] >> 2) - 1]);
				sfText_setCharacterSize(game.resources.text, 10);
				sfText_setPosition(game.resources.text, (sfVector2f){x * BOX_SIZE.x + 5, y * BOX_SIZE.y + 31});
				sfRenderWindow_drawText(game.resources.window, game.resources.text, NULL);
				free(buffer);
			} else if (game.grid.grid[x][y] >> 2) {
				setObjectRect(game.grid.grid[x][y] >> 2);
				displaySprite(loaded_sprites[OBJECTS_SPRITE], x * BOX_SIZE.x, y * BOX_SIZE.y + HUD_POS);
			}
		}
	}
}

char	*getTimeSinceStarted()
{
	static	char	buffer[1000];
	long unsigned	dis_time;

	memset(buffer, 0, sizeof(buffer));
	if (!game.end)
		dis_time = time(NULL) - game.start;
	else
		dis_time = game.end - game.start;
	if (!dis_time)
		return "0 second";
	if (dis_time / 86400) {
		sprintf(buffer, "%lu day%s", dis_time / 86400, dis_time / 86400 > 1 ? "s" : "");
		dis_time %= 86400;
	}
	if (dis_time / 3600) {
		sprintf(buffer, "%s %lu hour%s", buffer, dis_time / 3600, dis_time / 3600 > 1 ? "s" : "");
		dis_time %= 3600;
	}
	if (dis_time / 60) {
		sprintf(buffer, "%s %lu minute%s", buffer, dis_time / 60, dis_time / 60 > 1 ? "s" : "");
		dis_time %= 60;
	}
	if (dis_time)
		sprintf(buffer, "%s%s%lu second%s", buffer, *buffer ? " and " : "", dis_time, dis_time > 1 ? "s" : "");
	return buffer;
}

void	displayHud()
{
	char	buffer[1000];

	sfText_setColor(game.resources.text, (sfColor){0, 0, 0, 255});

	sprintf(buffer, "Gamemode: %s\n", game.grid.jumpingMines ? "Jumping mines" : "Normal");
	sfText_setString(game.resources.text, buffer);
	sfText_setCharacterSize(game.resources.text, 15);
	sfText_setPosition(game.resources.text, (sfVector2f){0, 0});
	sfRenderWindow_drawText(game.resources.window, game.resources.text, NULL);

	sprintf(buffer, "Mines founds: %u/%u\n", game.grid.flagsPlaced, game.grid.total);
	sfText_setString(game.resources.text, buffer);
	sfText_setCharacterSize(game.resources.text, 15);
	sfText_setPosition(game.resources.text, (sfVector2f){0, 17});
	sfRenderWindow_drawText(game.resources.window, game.resources.text, NULL);

	sprintf(buffer, "Time spent: %s\n", game.grid.isGenerated || game.end ? getTimeSinceStarted() : "0 second");
	sfText_setString(game.resources.text, buffer);
	sfText_setCharacterSize(game.resources.text, 15);
	sfText_setPosition(game.resources.text, (sfVector2f){0, 34});
	sfRenderWindow_drawText(game.resources.window, game.resources.text, NULL);
}

void	launchGame()
{
	srand(time(NULL));
	updateDiscordPresence("Preparing", NULL, 0, false, "icon", NULL);
	while (sfRenderWindow_isOpen(game.resources.window)) {	//If the window is closed, leave
		sfRenderWindow_clear(game.resources.window, (sfColor){205, 205, 200, 255});
		displayHud();
		displayGrid();
		sfRenderWindow_display(game.resources.window);
		manageEvents();
	}
}