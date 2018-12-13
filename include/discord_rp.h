#ifndef __DISCORD_RICH_PRESENCE_HEADER_
#define __DISCORD_RICH_PRESENCE_HEADER_

#include <stdbool.h>

void	initDiscordRichPresence(void);
void	updateDiscordPresence(char const *details, char const *state, int timestamp, bool end, char const *large_image, char const *large_text);

#endif