/* src/runeio.c
 *
 * Library for doing various file IO stuff
 */

#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string.h>
#include <conio.h>
#include "runeio.h"

//-- freadNTS: reads a null terminated string from a file --//
void freadNTS(char *str, int num, FILE *stream) {
	int charsRead;
	for (charsRead = 0; charsRead < num; charsRead++) {
		fread(str + charsRead, 1, 1, stream);
		if(*(str + charsRead) == '\0')
			break;
	}
}

//-- fensureDir: ensures a directory structure from a filename exists --//
short fensureDir(char *filename) {
	// Copy only pathname, don't copy the filename+extension
	// i.e. - 'C:\hey\yo.bmp' -> 'C:\hey\'
	char *filenameCopy = malloc((strrchr(filename, '\\') - filename) + 2);
	strncpy(filenameCopy, filename, (strrchr(filename, '\\') - filename + 1));
	filenameCopy[(strrchr(filename, '\\') - filename + 1)] = '\0';
	printf("Creating %s...\n", filenameCopy);

	// Attempt to open file here to see if dir exists
	FILE *tmpFile = fopen(filename, "w");
	if (tmpFile == NULL) { // Create Directory
		char *cmd = malloc(strlen(filenameCopy) + 9);
		sprintf(cmd, "mkdir \"%s\"", filenameCopy);
		system(cmd);
		free(cmd);
		free(filenameCopy);
		return 0;
	} else { // Directory already exists
		fclose(tmpFile);
		free(filenameCopy);
		return 0;
	}
}
