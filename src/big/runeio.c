/* src/runeio.c
 *
 * Library for doing various file IO stuff
 */

#include <stdio.h>
#include "runeio.h"

//-- freadNTS: reads a null terminated string from a file --//
void freadNTS(CHAR *str, int num, FILE *stream) {
	int charsRead;
	for (charsRead = 0; charsRead < num; charsRead++) {
		fread(str + charsRead, 1, 1, stream);
		if(*(str + charsRead) == '\0')
			break;
	}
}
