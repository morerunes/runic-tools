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
	char *newName = calloc(strlen(filename) + 1, 1);
	strcpy(newName, filename);

	char *lastSlash = strrchr(newName, '/');

	if (lastSlash == NULL) {
		lastSlash = strrchr(newName, '\\');
	}

	if (lastSlash == NULL) {
		printf("Error! Not a pathname?\n");
		return 1;
	}

	*(lastSlash + 1) = '\0';

	char *cmd = calloc(FILENAME_MAX, 1);
	char *fullpath = calloc(FILENAME_MAX, 1);
	_fullpath(fullpath, newName, FILENAME_MAX);

	char *tmp = calloc(strlen(fullpath) + 1, 1);
	strcpy(tmp, fullpath);
	strcat(tmp, "tmp");
	FILE *tmpFile = fopen(tmp, "w");
	if (tmpFile == NULL) {
		free(tmp);
	} else {
		fclose(tmpFile);
		remove(tmp);
		free(cmd);
		free(newName);
		free(fullpath);
		free(tmp);
		return 0;
	}

	sprintf(cmd, "mkdir %s", fullpath);

	system(cmd);

	free(cmd);
	free(newName);
	free(fullpath);

	return 0;
}
