# Minesweeper
A minesweeper game

Settings can only be changed in settings.json
Pressing a key in game will ask you if you want to reload settings from settings.json

mode:	String|"normal" or "jumping"
The game mode. "normal" is for a classical minesweeper game.
"jumping" is for the special mode, Jumping mines.
As it name suggest, the mines will move in this mode.
They move on a close box every time a box is openned.
They can move in diagonal and can also stay in place.
If a mine moves, all numbers around will update according to the new number of mine around it.
Mines can only move on blanck boxs (not on flagged, question marked or opened boxes).
Placing a mark (flag or question mark) on a box will lock it meaning that no mine can escape from this box or go to it.
Default: "normal"

nb_mines:	Positive integer in range 0, map_size.x * map_size.y - 1
The number of mines randomly added the battlefield.
Of course, there can't be more mines that boxes.
Default: 250

map_size:	Positive integer vector in range 1-159
The size of the map in boxes. The limit is 159 boxes for each dimension.
Default: (40, 40)

box_size:	Positive integer vector in range 16-128
The size of one box in pixels. A box can't be smaller than 16px and bigger that 128px.
Default: (20, 20)


If anything prevents the file be to be completely loaded (invalid values, invalid json format file, ...),
all settings are discarded and default values are used instead.
