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
#include <context.h>

#if defined _WIN32 || defined __WIN32 || defined __WIN32__

#include <windows.h>
#include <configParser.h>
#include <concatf.h>

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
	if (!debug)
		freopen("last.log", "w", stdout);//Redirect stdout to 'last.log'
#endif
}

Sprite	loadSprite(char *path)
{
	Sprite	sprite;

	printf("%s: Loading file '%s'\n", INFO_BEG, path);
	memset(&sprite, 0, sizeof(sprite));
	sprite.path = path;
	sprite.texture = sfTexture_createFromFile(path, NULL);
	if (sprite.texture) {
		sprite.sprite = sfSprite_create();
		sfSprite_setTexture(sprite.sprite, sprite.texture, true);
	} else
		printf("%s: Cannot load '%s'\n", ERROR_BEG, path);
	return sprite;
}

bool	loadGridJson(char *path, Grid *grid)
{
	ParserResult	result = Parser_parseFile(path, NULL);
	char		*buffer;
	char		temp[32];
	Context		context;

	if (result.error) {
		printf("%s: %s: %s\n", ERROR_BEG, path, result.error);
		buffer = concatf("Error: Cannot load file %s: %s\nMore information can be found in the log file\n", path, result.error);
		dispMsg("Loading error", buffer, MB_OK | MB_ICONERROR);
		free(buffer);
		return false;
	} else if (result.type != ParserObjType) {
		printf("%s: %s: Invalid type\n", ERROR_BEG, path);
		buffer = concatf("Error: Cannot load file %s: Invalid type\n", path, result.error);
		dispMsg("Loading error", buffer, MB_OK | MB_ICONERROR);
		free(buffer);
		Parser_destroyData(result.data, result.type);
		return false;
	}

	if (setjmp(context.jumpBuffer)) {
		Parser_destroyData(result.data, result.type);
		printf("%s: %s: %s\n", ERROR_BEG, path, context.error);
		buffer = concatf(
			"Error: File '%s' contains invalid data:\n%s\n",
			path,
			context.error
		);
		dispMsg("Settings Error", buffer, MB_OK | MB_ICONERROR);
		free(buffer);
		return false;
	}

	context.onMissing = LONG_JUMP;
	context.onInvalidType = LONG_JUMP;
	context.onUseFail = LONG_JUMP;
	context.object = result.data;

	memset(temp, 0, sizeof(temp));
	context.data = temp;
	context.index = "mode";
	context.useElement = copyStringInBuffer;
	context.expectedType = ContextStringType;
	getObjectElement(&context);
	if (strncmp("normal", temp, sizeof(temp)) == 0)
		grid->jumpingMines = false;
	else if (strncmp("jumping", temp, sizeof(temp)) == 0)
		grid->jumpingMines = true;
	else {
		sprintf(context.error, "Invalid mode \'%s\'. Expected 'normal' or 'jumping'", temp);
		longjmp(context.jumpBuffer, true);
	}

	context.data = &grid->size;
	context.index = "map_size";
	context.useElement = getUintVector;
	context.expectedType = ContextObjType;
	getObjectElement(&context);
	if (grid->size.x > 159 || grid->size.y > 159) {
		sprintf(context.error, "Map size must be in range 0-159\n");
		longjmp(context.jumpBuffer, true);
	}

	context.data = &grid->boxSize;
	context.index = "box_size";
	context.useElement = getUintVector;
	context.expectedType = ContextObjType;
	getObjectElement(&context);
	if (grid->boxSize.x < 16 || grid->boxSize.x > 128 || grid->boxSize.y < 16 || grid->boxSize.y > 128) {
		sprintf(context.error, "Box size must be in range 16-128\n");
		longjmp(context.jumpBuffer, true);
	}

	context.data = &grid->total;
	context.index = "nb_mines";
	context.useElement = getPositiveInteger;
	context.expectedType = ContextIntType;
	getObjectElement(&context);
	if (grid->total >= grid->size.x * grid->size.y) {
		strcpy(context.error, "Too many mines to place in this battlefield");
		longjmp(context.jumpBuffer, true);
	}

	ParserObj_destroy(result.data);
	return true;
}

void	getGridInfos(int argc, char **args, Grid *grid)
{
	char	*filePath = "settings.json";
	int	buffer;

	memset(grid, 0, sizeof(*grid));
	*grid = DEFAULT_GRID;
	if (argc == 2 + game.debug)
		filePath = args[1 + game.debug];
	else if (argc == 5 + game.debug || argc == 7 + game.debug) {
		grid->jumpingMines = !strcmp(args[1 + game.debug], "jumping");
		buffer = atoi(args[2 + game.debug]);
		if (buffer > 0 && buffer < 160)
			grid->size.x = buffer;
		else {
			printf("%s: Invalid parameter #%i: %s is not a valid number in range 1-159\n", ERROR_BEG, 2 + game.debug, args[2 + game.debug]);
			exit(EXIT_FAILURE);
		}
		buffer = atoi(args[3 + game.debug]);
		if (buffer > 0 && buffer < 160)
			grid->size.y = buffer;
		else {
			printf("%s: Invalid parameter #%i: '%s' is not a valid number in range 1-159\n", ERROR_BEG, 3 + game.debug, args[3 + game.debug]);
			exit(EXIT_FAILURE);
		}
		buffer = atoi(args[4 + game.debug]);
		if (buffer > 0 && buffer < grid->size.x * grid->size.y)
			grid->total = buffer;
		else {
			printf("%s: Invalid parameter #%i: '%s' is not a valid number in range 1-%u\n", ERROR_BEG, 4 + game.debug, args[4 + game.debug], grid->size.x * grid->size.y);
			exit(EXIT_FAILURE);
		}
		if (argc == 7 + game.debug) {
			buffer = atoi(args[5 + game.debug]);
			if (buffer >= 16 && buffer <= 128)
				grid->boxSize.x = buffer;
			else {
				printf("%s: Invalid parameter #%i: '%s' is not a valid number in range 16-128\n", ERROR_BEG, 5 + game.debug, args[5 + game.debug]);
				exit(EXIT_FAILURE);
			}
			buffer = atoi(args[6 + game.debug]);
			if (buffer >= 16 && buffer <= 128)
				grid->boxSize.y = buffer;
			else {
				printf("%s: Invalid parameter #%i: '%s' is not a valid number in range 16-128\n", ERROR_BEG, 6 + game.debug, args[6 + game.debug]);
				exit(EXIT_FAILURE);
			}
		}
	}
	if (!loadGridJson(filePath, grid)) {
		*grid = DEFAULT_GRID;
	}
}

void	allocGrid(Grid *grid)
{
	grid->grid = my_malloc(grid->size.x * sizeof(*grid->grid));				//We create a double array
	*grid->grid = my_malloc(grid->size.x * grid->size.y * sizeof(**grid->grid));		//We reserve all the memory we need
	memset(*grid->grid, NO_MINE, grid->size.x * grid->size.y * sizeof(**grid->grid));	//We init the whole grid to the NO_MINE state
	for (unsigned i = 0; i < grid->size.x; i++)							//Now we split the big chunk of memory
		grid->grid[i] = &(*grid->grid)[i * grid->size.y];				//To be used as a double array later
}

void	initGame(bool debug, int argc, char **args)
{
	sfVideoMode	mode = sfVideoMode_getDesktopMode();
	sfVector2u	size;

	memset(&game, 0, sizeof(game));
	updateDiscordPresence("Loading game", NULL, 0, false, "icon", NULL);
	game.debug = debug;

	printf("%s: Creating grid information\n", INFO_BEG);
	getGridInfos(argc, args, &game.grid);
	allocGrid(&game.grid);

	printf("%s: Loading render objects\n", INFO_BEG);
	game.resources.rect = sfRectangleShape_create();
	game.resources.font = sfFont_createFromFile("data/arial.ttf");
	game.resources.text = sfText_create();
	sfText_setFont(game.resources.text, game.resources.font);

	printf("%s: Loading window icon\n", INFO_BEG);
	game.resources.icon = sfImage_createFromFile(ICON_PATH);

	printf("%s: Loading sprites\n", INFO_BEG);
	game.resources.sprite.length = sizeof(SPRITES) / sizeof(*SPRITES);
	game.resources.sprite.content = my_malloc(game.resources.sprite.length * sizeof(Sprite));
	for (unsigned i = 0; i < sizeof(SPRITES) / sizeof(*SPRITES); i++) {
		loaded_sprites[i] = loadSprite(SPRITES[i].path);
		loaded_sprites[i].rect.width = SPRITES[i].size.x;
		loaded_sprites[i].rect.height = SPRITES[i].size.y;
	}

	printf("%s: Opening game window\n", INFO_BEG);
	mode.width = game.grid.size.x * BOX_SIZE.x;
	mode.height = game.grid.size.y * BOX_SIZE.y + HUD_POS;
	game.resources.window = sfRenderWindow_create(mode, "Minesweeper", sfClose | sfTitlebar, NULL);

	printf("%s: Setting window icon\n", INFO_BEG);
	if (game.resources.icon) {
		size = sfImage_getSize(game.resources.icon);
		sfRenderWindow_setIcon(game.resources.window, size.x, size.y, sfImage_getPixelsPtr(game.resources.icon));
	}
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
	game.debug = (argc > 1 && !strcmp("debug", args[1]));
	setSignalHandler();
	closeConsole(game.debug);
	initDiscordRichPresence();
	initGame(game.debug, argc, args);
	launchGame();
	destroyGameElements();
	printf("%s: Goodbye !\n", INFO_BEG);
	return EXIT_SUCCESS;
}
