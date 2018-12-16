//
// Created by Gegel85 on 12/12/2018.
//

#include <stdlib.h>
#include <stdio.h>
#include "macros.h"

void	*my_malloc(size_t size)
{
	void	*ptr = malloc(size);

	if (!ptr) {
		//No memory left :/
		printf("%s: Allocation failure (%luB)\n", FATAL_BEG, (long unsigned int)size);
		dispMsg("Memory allocation error", ALLOC_ERROR_MSG, MB_ICONERROR | MB_OK);
		exit(EXIT_FAILURE);
	}
	return (ptr);
}