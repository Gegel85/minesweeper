cmake_minimum_required(VERSION 3.13)
project(minesweeper C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
include_directories(lib/concatf/include)

add_library(
	concatf
	lib/concatf/include/concatf.h
	lib/concatf/include/flags.h
	lib/concatf/sources/concat.c
	lib/concatf/sources/concatf.c
	lib/concatf/sources/float_to_str.c
	lib/concatf/sources/get_nbrlen.c
	lib/concatf/sources/int_to_str.c
	lib/concatf/sources/my_compute_power_rec_long.c
	lib/concatf/sources/my_putbase.c
	lib/concatf/sources/my_putnbr_base.c
	lib/concatf/sources/my_putstar.c
	lib/concatf/sources/my_putstr_stderr.c
	lib/concatf/sources/my_showstr.c
	lib/concatf/sources/my_strlen_rec.c
	lib/concatf/sources/str.c
	lib/concatf/sources/sub_strings.c
	lib/concatf/sources/uint_to_str.c
	include/grid.h)

add_executable(
	minesweeper
	icon.rc
	include/configs.h
	include/game.h
	include/globals.h
	include/typedef.h
	src/globals.c
	src/main.c
	src/sighandlers.c
	include/array.h
	include/macros.h
	src/alloc.c
	src/discord_rich_presence.c
	include/discord_rp.h
	include/display_messagebox.h
	src/disp_msg.c
	include/utils.h src/game_loop.c)

target_link_libraries(
	${PROJECT_NAME}
	discord-rpc
	csfml-graphics
	csfml-audio
	csfml-system
	csfml-window
	csfml-network
	concatf
	m
)