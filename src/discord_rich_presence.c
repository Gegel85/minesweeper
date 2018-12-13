#include <discord_rpc.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

void	initDiscordRichPresence(void)
{
	DiscordEventHandlers	handlers;

	memset(&handlers, 0, sizeof(handlers));
	Discord_Initialize("522544481531330560", &handlers, 1, NULL);
}

void	updateDiscordPresence(char const *details, char const *state, int timestamp, bool end, char const *large_image, char const *large_text)
{
	DiscordRichPresence	discordPresence;

	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.state = state;
	discordPresence.details = details;
	if (end)
		discordPresence.endTimestamp = timestamp;
	else
		discordPresence.startTimestamp = timestamp;
	discordPresence.largeImageKey = large_image;
	discordPresence.largeImageText = large_text;
	Discord_UpdatePresence(&discordPresence);
}