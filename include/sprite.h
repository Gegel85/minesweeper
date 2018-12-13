//
// Created by Gegel85 on 12/12/2018.
//

#ifndef MINESWEEPER_SPRITE_H
#define MINESWEEPER_SPRITE_H

#include <SFML/Graphics.h>

struct	Sprite {
	sfSprite	*sprite;
	sfTexture	*texture;
	char		*path;
	sfIntRect	rect;
};

#endif //MINESWEEPER_SPRITE_H
