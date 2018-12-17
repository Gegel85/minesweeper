NAME =	minesweeper

SRC =	alloc.c			\
	context_callbacks.c	\
	discord_rich_presence.c	\
	disp_msg.c		\
	game_loop.c		\
	globals.c		\
	grid_mgr.c		\
	main.c			\
	sighandlers.c		\

OBJ =	$(SRC:%.c=src/%.o)

INC =	-Iinclude			\
	-Ilib/concatf/include		\
	-Ilib/configParser/include	\

CSFML = -lcsfml-audio		\
	-lcsfml-graphics	\
	-lcsfml-network		\
	-lcsfml-system		\
	-lcsfml-window		\

LDFLAGS =			\
	-L lib/concatf		\
	-L lib/configParser	\
	-lm			\
	-lconcatf		\
	-lconfigParser		\
	-ldiscord-rpc		\

CFLAGS= $(INC)			\
	-W			\
	-Wall			\
	-Wextra			\

CC =	gcc

RULE =	all

LIBS =	lib/concatf/libconcatf.a		\
	lib/configParser/libconfigParser.a	\

RES =

all:	$(LIBS) $(NAME)

windows:RES = icon.res
windows:LDFLAGS += -mwindows
windows:icon.res all

icon.res:
	windres icon.rc -O coff -o icon.res

lib/concatf/libconcatf.a:
	$(MAKE) -C lib/concatf $(RULE)

lib/configParser/libconfigParser.a:
	$(MAKE) -C lib/configParser $(RULE)

$(NAME):$(OBJ)
	g++ -o $(NAME) $(OBJ) $(LDFLAGS) $(CSFML) $(RES)

clean:
	$(MAKE) -C lib/concatf clean
	$(MAKE) -C lib/configParser clean
	$(RM) $(OBJ)
	$(RM) icon.res

fclean:	clean
	$(RM) $(NAME) $(NAME).exe

ffclean:fclean
	$(MAKE) -C lib/concatf fclean
	$(MAKE) -C lib/configParser fclean

re:	fclean all

dbg:	CFLAGS += -g -O0
dbg:	RULE = dbg
dbg:	ffclean all
