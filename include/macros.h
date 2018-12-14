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
	{255, 0, 0, 255},\
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
	{255, 0, 0, 255},\
	{0, 0, 0, 255},\
	{0, 0, 0, 255},\
	{0, 0, 255, 255},\
	{0, 0, 0, 255},\
	{0, 255, 0, 255},\
}

#define CHARACTERS	"12345678XXFF?X"

#define BOX_SIZE	(sfVector2f){24, 24}

#define DEFAULT_GRID	(Grid){false, (sfVector2u){40, 40}, 40}

#define ALLOC_ERROR_MSG		"An error occurred when trying to reserve memory.\n\
If you are playing on a low memory machine, this can cause this kind of error.\n\
Try closing some programs.\n\
More informations can be found in last.log\n\n\
Click OK to close the application"

#endif //MINESWEEPER_MACROS_H
