//
// Created by Gegel85 on 12/12/2018.
//

#ifndef MINESWEEPER_MACROS_H
#define MINESWEEPER_MACROS_H

#include "display_messagebox.h"

#define INFO_BEG	"[INFO]   "
#define ERROR_BEG	"[ERROR]  "
#define FATAL_BEG	"[FATAL]  "
#define WARNING_BEG	"[WARNING]"

enum	spriteIndex	{
	OBJECTS_SPRITE,
};

#define COLORS	(sfColor[]){\
	{0, 0, 255, 255},\
	{0, 120, 0, 255},\
	{255, 0, 0, 255},\
	{0, 0, 155, 255},\
	{155, 0, 0, 255},\
	{0, 255, 255, 255},\
	{0, 0, 0, 255},\
	{100, 100, 100, 255},\
	{0, 0, 0, 255},\
	{0, 0, 0, 255},\
	{255, 0, 0, 255},\
	{0, 255, 0, 255},\
	{0, 0, 0, 255},\
	{0, 0, 0, 255},\
}

#define BG_COLORS	(sfColor[]){\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{205, 205, 205, 255},\
	{255, 0, 0, 255},\
	{205, 205, 205, 255},\
	{0, 255, 255, 255},\
	{0, 0, 0, 255},\
	{0, 255, 0, 255},\
}

#define CHARACTERS	"12345678XXFF?X"

#define BOX_SIZE	game.grid.boxSize
#define HUD_POS		60

#define ICON_PATH	"data/icon.png"

#define loaded_sprites	((Sprite *)game.resources.sprite.content)

#define setObjectRect(value)	loaded_sprites[OBJECTS_SPRITE].rect.top = 0;\
				loaded_sprites[OBJECTS_SPRITE].rect.left = ((value) - 1) * loaded_sprites[OBJECTS_SPRITE].rect.width

#define displaySprite(sprite_struct, x_pos, y_pos)\
	sfSprite_setPosition(sprite_struct.sprite, (sfVector2f){x_pos + 1, y_pos + 1});\
	sfSprite_setTextureRect(sprite_struct.sprite, sprite_struct.rect);\
	sfSprite_setScale(sprite_struct.sprite, (sfVector2f){\
		(BOX_SIZE.x - 3) / loaded_sprites[OBJECTS_SPRITE].rect.width,\
		(BOX_SIZE.y - 3) / loaded_sprites[OBJECTS_SPRITE].rect.height\
	});\
	sfRenderWindow_drawSprite(game.resources.window, sprite_struct.sprite, NULL)

#define DEFAULT_GRID	(Grid){false, (sfVector2u){40, 40}, (sfVector2u){20, 20}, 250}

#define ALLOC_ERROR_MSG		"An error occurred when trying to reserve memory.\n\
If you are playing on a low memory machine, this can cause this kind of error.\n\
Try closing some programs.\n\
More informations can be found in last.log\n\n\
Click OK to close the application"

#endif //MINESWEEPER_MACROS_H
