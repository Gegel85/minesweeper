NAME =	VEDA

SRC =	main.c			\
	globals.c		\
	discord_rich_presence.c	\
	sighandlers.c		\

OBJ =	$(SRC:%.c=src/%.o)

INC =	-Iinclude			\
	-Ilib/concatf/include		\

CSFML = -lcsfml-audio		\
	-lcsfml-graphics	\
	-lcsfml-network		\
	-lcsfml-system		\
	-lcsfml-window		\

LDFLAGS =			\
	-L lib/concatf		\
	-lm			\
	-lconcatf		\
	-ldiscord-rpc		\

CFLAGS= $(INC)			\
	-W			\
	-Wall			\
	-Wextra			\

CC =	gcc

RULE =	all

LIBS =	lib/concatf/libconcatf.a		\

RES =

all:	$(LIBS) $(NAME)

windows:RES = icon.res
windows:LDFLAGS += -mwindows
windows:icon.res all

icon.res:
	windres icon.rc -O coff -o icon.res

lib/concatf/libconcatf.a:
	$(MAKE) -C lib/concatf $(RULE)

$(NAME):$(OBJ)
	g++ -o $(NAME) $(OBJ) $(LDFLAGS) $(CSFML) $(RES)

clean:
	$(MAKE) -C lib/concatf clean
	$(RM) $(OBJ)
	$(RM) icon.res

fclean:	clean
	$(RM) $(NAME) $(NAME).exe

ffclean:fclean
	$(MAKE) -C lib/concatf fclean

re:	fclean all

dbg:	CFLAGS += -g -O0
dbg:	RULE = dbg
dbg:	ffclean all
