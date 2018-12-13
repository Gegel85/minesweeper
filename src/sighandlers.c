#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <discord_rpc.h>
#include <SFML/Graphics.h>
#include "macros.h"
#include "globals.h"
#include "concatf.h"
#include "discord_rp.h"

#if defined _WIN32 || defined __WIN32 || defined __WIN32__

char	*strsignal(int signum)
{
	switch (signum) {
	case 2:
		return ("Interrupted");
	case 4:
		return ("Illegal hardware instruction");
	case 6:
		return ("Aborted");
	case 7:
		return ("Bus error");
	case 8:
		return ("Floating point exception");
	case 11:
		return ("Segmentation fault");
	case 13:
		return ("Broken pipe");
	case 15:
		return ("Terminated");
	}
	return ("Unknown signal");
}

#endif

void	handleFatalSignals(int signum)
{
	printf("%s: Caught signal %i (%s). Aborting !\n", FATAL_BEG, signum, strsignal(signum));
	updateDiscordPresence("Game crashed", strsignal(signum), 0, false, "icon", strsignal(signum));
	dispMsg("Fatal Error", concatf("Fatal: Caught signal %i (%s)\n\n\nClick OK to close the program", signum, strsignal(signum)), MB_OK | MB_ICONERROR);
	Discord_Shutdown();
	exit(signum + 128);
}

void	handleSigInt(int signum)
{
	printf("%s: Caught signal %i (%s). Exiting.\n", INFO_BEG, signum, strsignal(signum));
	if (game.resources.window && sfRenderWindow_isOpen(game.resources.window))
		sfRenderWindow_close(game.resources.window);
	else {
		Discord_Shutdown();
		exit(EXIT_SUCCESS);
	}
}

void	sighandler(int signum)
{
	static int	nbSignals = 0;	//The number of times the program crashed (or at least recieved a signal)

	nbSignals++;
	if (nbSignals >= 6) {
		//6th time or more -> try to exit asap
		signal(signum, NULL);
		raise(signum);
		exit(EXIT_FAILURE);
	}
	if (nbSignals >= 3)	//3rd time, following code might just be crashing
		return;
	if (signum == SIGINT || signum == SIGTERM)
		//User requests program end
		handleSigInt(signum);
	else
		//Program crashed (rip)
		handleFatalSignals(signum);
}
