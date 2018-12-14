//
// Created by Gegel85 on 12/12/2018.
//

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <discord_rp.h>
#include <discord_rpc.h>
#include <macros.h>
#include <utils.h>
#include "globals.h"
#include <fcntl.h>

#if defined _WIN32 || defined __WIN32 || defined __WIN32__

#include <windows.h>

void	setSignalHandler()
{
	signal(SIGINT,  &sighandler);
	signal(SIGILL,  &sighandler);
	signal(SIGABRT, &sighandler);
	signal(SIGFPE,  &sighandler);
	signal(SIGSEGV, &sighandler);
	signal(SIGTERM, &sighandler);
}

#else

void	setSignalHandler()
{
	signal(SIGINT,  &sighandler);
	signal(SIGQUIT, &sighandler);
	signal(SIGILL,  &sighandler);
	signal(SIGABRT, &sighandler);
	signal(SIGBUS,  &sighandler);
	signal(SIGFPE,  &sighandler);
	signal(SIGSEGV, &sighandler);
	signal(SIGPIPE, &sighandler);
	signal(SIGTERM, &sighandler);
}

#endif

void	closeConsole(bool debug)
{
#if defined _WIN32 || defined __WIN32 || defined __WIN32__

	if (!debug && !FreeConsole()) //Not in debug: close the console
		printf("%s: Cannot close main console\n", ERROR_BEG);
	else if (debug) {
		AllocConsole(); //In debug: Create a new console
		freopen("CONOUT$", "w", stdout); //Redirect stdout to the console
	} else
		freopen("last.log", "w", stdout);//Redirect stdout to 'last.log'

#else
	(void)debug;
#endif
}

void	getGridInfos(Grid *grid)
{
	int	fd = open("gridInfos.txt", O_RDONLY);
	size_t	n = 1;

	*grid = DEFAULT_GRID;
	if (fd < 0) {
		printf("%s: Cannot load grid infos. Loading default infos...\n", ERROR_BEG);
		return;
	}
}

void	allocGrid(Grid *grid)
{
	grid->grid = my_malloc(grid->size.x * sizeof(*grid->grid));				//We create a double array
	*grid->grid = my_malloc(grid->size.x * grid->size.y * sizeof(**grid->grid));		//We reserve all the memory we need
	memset(*grid->grid, NO_MINE, grid->size.x * grid->size.y * sizeof(**grid->grid));	//We init the whole grid to the NO_MINE state
	for (int i = 0; i < grid->size.x; i++)							//Now we split the big chunk of memory
		grid->grid[i] = &(*grid->grid)[i * grid->size.y];				//To be used as a double array later
}

void	initGame()
{
	sfVideoMode	mode = {640, 480, 32};

	memset(&game, 0, sizeof(game));
	updateDiscordPresence("Loading game", NULL, 0, false, "icon", NULL);

	printf("%s: Creating grid informations\n", INFO_BEG);
	getGridInfos(&game.grid);
	allocGrid(&game.grid);

	printf("%s: Loading render objects\n", INFO_BEG);
	game.resources.rect = sfRectangleShape_create();
	game.resources.font = sfFont_createFromFile("data/arial.ttf");
	game.resources.text = sfText_create();
	sfText_setFont(game.resources.text, game.resources.font);

	printf("%s: Opening game window\n", INFO_BEG);
	mode.width = game.grid.size.x * BOX_SIZE.x;
	mode.height = game.grid.size.y * BOX_SIZE.y;
	game.resources.window = sfRenderWindow_create(mode, "Minesweeper", sfClose | sfTitlebar, NULL);
}

void	destroyGameElements()
{
	sfRenderWindow_destroy(game.resources.window);
	if (game.grid.grid)
		free(*game.grid.grid);
	free(game.grid.grid);
	Discord_Shutdown();
}

int	main(int argc, char **args)
{
	bool	debug = true;//(argc == 2 && !strcmp("debug", args[1]));

	setSignalHandler();
	closeConsole(debug);
	initDiscordRichPresence();
	initGame();
	launchGame();
	destroyGameElements();
	printf("%s: Goodbye !\n", INFO_BEG);
	return EXIT_SUCCESS;
}
